#include "dxf_reader.h"

#include <stdlib.h>
#include "geometry.h"

#include <stdio.h>
#include <qdebug.h>

enum {
	ENT_FIRST,
	ENT_LINE,
	ENT_POINT,
	ENT_LWPOLYLINE,
	ENT_CIRCLE,
	ENT_ARC,
	ENT_3DFACE,
	ENT_LAST
};



static void readData(int *code,char *data,FILE *fp)
{
	fgets(data,1024,fp);
	(*code)=atoi(data);
	fgets(data,1024,fp);
}

static void extrudePoint(double x[3],double Z[3])
{
	double X[3],Y[3];
	static double cc=1./64.;

	if (Z[0]==0 && Z[1]==0 && Z[2]>=0) return; /*The coordinate system is WCS*/
	

	if (Z[0]>-cc && Z[0]<cc && Z[1]>-cc && Z[1]<cc) {
		/*X= [0 1 0] x Z*/
		X[0]=Z[2]; X[1]=0; X[2]=-Z[0];
	} else {
		/*X= [0 0 1] x Z*/
		X[0]=-Z[1]; X[1]=Z[0]; X[2]=0;
	}
	Y[0]=Z[1]*X[2]-X[1]*Z[2];
	Y[1]=Z[2]*X[0]-X[2]*Z[0];
	Y[2]=Z[0]*X[1]-X[0]*Z[1];

	double s;
	s=sqrt(X[0]*X[0]+X[1]*X[1]+X[2]*X[2]);
	X[0]/=s; X[1]/=s; X[2]/=s;
	s=sqrt(Y[0]*Y[0]+Y[1]*Y[1]+Y[2]*Y[2]);
	Y[0]/=s; Y[1]/=s; Y[2]/=s;
	s=sqrt(Z[0]*Z[0]+Z[1]*Z[1]+Z[2]*Z[2]);
	Z[0]/=s; Z[1]/=s; Z[2]/=s;
	

	double nX[3];
	nX[0]=X[0]*x[0]+X[1]*x[1]+X[2]*x[2];
	nX[1]=Y[0]*x[0]+Y[1]*x[1]+Y[2]*x[2];
	nX[2]=Z[0]*x[0]+Z[1]*x[1]+Z[2]*x[2];

	x[0]=nX[0]; x[1]=nX[1]; x[2]=nX[2];

}

void readDXF(Geometry *geom,const char *name)
{
	FILE *fp=fopen(name,"rb");
	if (!fp) return;

	int code;
	char buffer[1024];
	int inSection=0;
	int inEntities=0;
	int entityCode,preEntityCode,entityDone;

	myStack<double> codeDouble[2000];
	myStack<int> codeInt[2000];
	int k;


	while (!feof(fp)) {
		readData(&code,buffer,fp);

		if (!inSection) {
			if (code==0 && !strncmp(buffer,"SECTION",7)) {
				inSection=1;
				qDebug("Entering Section");
				continue;
			}
		} else {
			if (code==0 && !strncmp(buffer,"ENDSEC",6)) {
				inSection=0;
				inEntities=0;
				qDebug("Out of section");
				continue;
			}
		}

		if (!inSection) continue;

		/*In SECTION*/
		if (!inEntities && code==2 && !strncmp(buffer,"ENTITIES",8)) {
			inEntities=1;
			qDebug("Entering entities");
			entityCode=ENT_FIRST; 
			continue;
		}
		
		if (inEntities) {
			entityDone=0;
			preEntityCode=entityCode;

			if (code==0) {
				entityDone=1;
				//qDebug("Code: %s",buffer);
				if (!strncmp(buffer,"LINE",4)) entityCode=ENT_LINE;
				else if (!strncmp(buffer,"POINT",5)) entityCode=ENT_POINT;
				else if (!strncmp(buffer,"LWPOLYLINE",10)) entityCode=ENT_LWPOLYLINE;
				else if (!strncmp(buffer,"CIRCLE",6)) entityCode=ENT_CIRCLE;
				else if (!strncmp(buffer,"ARC",3)) entityCode=ENT_ARC;
				else if (!strncmp(buffer,"3DFACE",5)) entityCode=ENT_3DFACE;
				else entityCode=ENT_LAST;
			}
			double dvalue=atof(buffer);
			int ivalue=atoi(buffer);

			if (code>=10 && code<=39) codeDouble[code].push(&dvalue);
			if (code>=40 && code<=59) codeDouble[code].push(&dvalue);
			if (code>=60 && code<=99) codeInt[code].push(&ivalue);
			if (code>=210 && code<=239) codeDouble[code].push(&dvalue);


 

			if (entityDone) {
				switch (preEntityCode) {
					case ENT_LINE:
						{
							double x1[3],x2[3],Z[3];
							codeDouble[10].pop(&x1[0]); 
							codeDouble[20].pop(&x1[1]);
							codeDouble[30].pop(&x1[2]);
							codeDouble[11].pop(&x2[0]); 
							codeDouble[21].pop(&x2[1]);
							codeDouble[31].pop(&x2[2]);
							
							if (codeDouble[210].pop(&Z[0])) {
								codeDouble[220].pop(&Z[1]);
								codeDouble[230].pop(&Z[2]);
							} else {
								Z[0]=0; Z[1]=0; Z[2]=1;
							}

							extrudePoint(x1,Z);
							extrudePoint(x2,Z);

							
							int id1=geom->addGrid(x1[0],x1[1],x1[2]);
							int id2=geom->addGrid(x2[0],x2[1],x2[2]);
							geom->addLine(id1,id2);

						}
						break;
					case ENT_POINT:
						{
							double x[3];
							double Z[3];

							
							codeDouble[10].pop(&x[0]); 
							codeDouble[20].pop(&x[1]);
							codeDouble[30].pop(&x[2]);

							if (codeDouble[210].pop(&Z[0])) {
								codeDouble[220].pop(&Z[1]);
								codeDouble[230].pop(&Z[2]);
							} else {
								Z[0]=0; Z[1]=0; Z[2]=1;
							}
							extrudePoint(x,Z);
							int id=geom->addGrid(x[0],x[1],x[2]);
							geom->addPoint(id);
						}
						break;
					case ENT_LWPOLYLINE:
						{
							double x[3];
							double Z[3];
							int k,n;
							codeInt[90].pop(&n);
							if (codeDouble[210].pop(&Z[0])) {
								codeDouble[220].pop(&Z[1]);
								codeDouble[230].pop(&Z[2]);
							} else {
								Z[0]=0; Z[1]=0; Z[2]=1;
							}

							if (n>0) {
								int *id=new int[n];
								for (k=0; k<n; k++) {
									codeDouble[10].pop(&x[0]); 
									codeDouble[20].pop(&x[1]);
									x[2]=0;
									extrudePoint(x,Z);

									id[k]=geom->addGrid(x[0],x[1],x[2]);
								}
								for (k=n-1; k>=1; k--) {
									geom->addLine(id[k],id[k-1]);
								}
								if (n>1) {
									codeInt[70].pop(&k);
									if (k==1) geom->addLine(id[0],id[n-1]);
								}

								delete []id;
							}
						}
						break;
					case ENT_CIRCLE:
						{
							double x0[3];
							double x[3];

							double r;
							double Z[3];
							int k,n;
							codeInt[90].pop(&n);
							if (codeDouble[210].pop(&Z[0])) {
								codeDouble[220].pop(&Z[1]);
								codeDouble[230].pop(&Z[2]);
							} else {
								Z[0]=0; Z[1]=0; Z[2]=1;
							}
							
							codeDouble[10].pop(&x0[0]); 
							codeDouble[20].pop(&x0[1]);
							codeDouble[30].pop(&x0[2]);
							codeDouble[40].pop(&r);

							double f;
							n=360;
							int id0,id1,id2;
							id2=0;

							for (k=0; k<n; k++) {
								f=2*3.14159*k/((double)n);
								x[0]=x0[0]+r*cos(f);
								x[1]=x0[1]+r*sin(f);
								x[2]=x0[2];
								extrudePoint(x,Z);
								id1=id2;
								id2=geom->addGrid(x[0],x[1],x[2]);
								if (k>0) geom->addLine(id1,id2); else id0=id2;
							}
							geom->addLine(id2,id0);
						}
						break;

					case ENT_ARC:
						{
							double x0[3];
							double x[3];

							double r;
							double f1,f2;
							double Z[3];
							int k,n;
							codeInt[90].pop(&n);
							if (codeDouble[210].pop(&Z[0])) {
								codeDouble[220].pop(&Z[1]);
								codeDouble[230].pop(&Z[2]);
							} else {
								Z[0]=0; Z[1]=0; Z[2]=1;
							}
							codeDouble[10].pop(&x0[0]); 
							codeDouble[20].pop(&x0[1]);
							codeDouble[30].pop(&x0[2]);
							codeDouble[40].pop(&r);
							codeDouble[50].pop(&f1);
							codeDouble[51].pop(&f2);
							f1=f1*3.14159/180.;
							f2=f2*3.14159/180.;

							double f,df;
							
							n=360;
							int id0,id1,id2;
							df=(f2-f1)/((double)n);

							id2=0;
							
							for (k=0; k<n; k++) {
								f=f1+k*df;								
								
								x[0]=x0[0]+r*cos(f);
								x[1]=x0[1]+r*sin(f);
								x[2]=x0[2];
								extrudePoint(x,Z);
								id1=id2;
								id2=geom->addGrid(x[0],x[1],x[2]);
								if (k>0) geom->addLine(id1,id2); else id0=id2;
							}
						}
						break;
					case ENT_3DFACE:
						{
							double x[4][3];
							int edgeMask;
							codeDouble[10].pop(&x[0][0]);
							codeDouble[20].pop(&x[0][1]);
							codeDouble[30].pop(&x[0][2]);
							codeDouble[11].pop(&x[1][0]);
							codeDouble[21].pop(&x[1][1]);
							codeDouble[31].pop(&x[1][2]);
							codeDouble[12].pop(&x[2][0]);
							codeDouble[22].pop(&x[2][1]);
							codeDouble[32].pop(&x[2][2]);
							codeDouble[13].pop(&x[3][0]);
							codeDouble[23].pop(&x[3][1]);
							codeDouble[33].pop(&x[3][2]);
							codeInt[70].pop(&edgeMask);

							int id1,id2,id3,id4;
							id1=geom->addGrid(x[0][0],x[0][1],x[0][2]);
							id2=geom->addGrid(x[1][0],x[1][1],x[1][2]);
							id3=geom->addGrid(x[2][0],x[2][1],x[2][2]);
							if (x[3][0]==x[2][0] && x[3][1]==x[2][1] && x[3][2]==x[2][2]) {
								geom->addTriangle(id1,id2,id3,0);
								if (!(edgeMask & 1)) geom->addEdge(id1,id2);
								if (!(edgeMask & 2)) geom->addEdge(id2,id3);
								if (!(edgeMask & 4)) geom->addEdge(id3,id1);
							} else {
								id4=geom->addGrid(x[3][0],x[3][1],x[3][2]);
								geom->addTriangle(id1,id2,id3,0);
								geom->addTriangle(id3,id4,id1,0);
								if (!(edgeMask & 1)) geom->addEdge(id1,id2);
								if (!(edgeMask & 2)) geom->addEdge(id2,id3);
								if (!(edgeMask & 4)) geom->addEdge(id3,id4);
								if (!(edgeMask & 1)) geom->addEdge(id4,id1);
							}

						}
						break;

				}
				for (k=0; k<2000; k++) {
					codeDouble[k].clear();
					codeInt[k].clear();
				}
			}
		}
	}

		
	fclose(fp);

}