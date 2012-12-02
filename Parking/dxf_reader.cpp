#include "dxf_reader.h"

#include <stdlib.h>
#include <math.h>
#include "geometry.h"
#include "coord_system.h"

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

static void createObjectCoordSystem(CoordinateSystem<double> *pXYZ,const double Z[3])
{
	static const double cc=1./64.;
	double X[3],Y[3];

	if (Z[0]==0 && Z[1]==0 && Z[2]>=0) {
		X[0]=1; X[1]=0; X[2]=0;
		Y[0]=0; Y[1]=1; Y[2]=0;
	} else if (Z[0]>-cc && Z[0]<cc && Z[1]>-cc && Z[1]<cc) {
		/*X= [0 1 0] x Z*/
		X[0]=Z[2]; X[1]=0; X[2]=-Z[0];
	} else {
		/*X= [0 0 1] x Z*/
		X[0]=-Z[1]; X[1]=Z[0]; X[2]=0;
	}
	vec_cross_product(Y,Z,X);
	pXYZ->setGlobal();
	pXYZ->setAxis(X,Y,Z);

}

static void createObjectCoordSystem(CoordinateSystem<float> *pXYZ,const float Z[3])
{
	static const double cc=1./64.;
	float X[3],Y[3];

	if (Z[0]==0 && Z[1]==0 && Z[2]>=0) {
		X[0]=1; X[1]=0; X[2]=0;
		Y[0]=0; Y[1]=1; Y[2]=0;
	} else if (Z[0]>-cc && Z[0]<cc && Z[1]>-cc && Z[1]<cc) {
		/*X= [0 1 0] x Z*/
		X[0]=Z[2]; X[1]=0; X[2]=-Z[0];
	} else {
		/*X= [0 0 1] x Z*/
		X[0]=-Z[1]; X[1]=Z[0]; X[2]=0;
	}
	vec_cross_product(Y,Z,X);
	pXYZ->setGlobal();
	pXYZ->setAxis(X,Y,Z);

}


static void extrudePoint(double X[3],const double Z[3])
{
	if (Z[0]==0 && Z[1]==0 && Z[2]>=0) return; /*The coordinate system is WCS*/

	CoordinateSystem<double> XYZ;
	createObjectCoordSystem(&XYZ,Z);

	double nX[3];
	XYZ.fromLocalToGlobal(nX,X);

	vec_copy(X,nX);
}


template <typename T> class myStack {
	T *data;
	int datalen;
	int datamem;
	myStack(myStack &x);
	public:
	myStack() {
		data=(T *)malloc(1*sizeof(T));
		datalen=0; datamem=1;
	}
	~myStack() {
		free(data);
	}
	void push(const T *val) {
		if (datalen==datamem) {
			datamem*=2;
			data=(T *)realloc(data,datamem*sizeof(double));
		}
		memcpy(&data[datalen],val,sizeof(T));
		datalen++;
	}
	int pop(T *val) {
		if (datalen==0) return 0;
		datalen--;
		memcpy(val,&data[datalen],sizeof(T));
		return 1;
	}

	void clear() {
		datalen=0;
	}
};



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
							double x0_d[3];
							double x[3];

							double r_d;
							double Z_d[3];

							float Z[3];
							float x0[3];
							float r;

							int k,n;
							codeInt[90].pop(&n);
							if (codeDouble[210].pop(&Z_d[0])) {
								codeDouble[220].pop(&Z_d[1]);
								codeDouble[230].pop(&Z_d[2]);
							} else {
								Z_d[0]=0; Z_d[1]=0; Z_d[2]=1;
							}
							Z[0]=Z_d[0]; Z[1]=Z_d[1]; Z[2]=Z_d[2];
							
							codeDouble[10].pop(&x0_d[0]); 
							codeDouble[20].pop(&x0_d[1]);
							codeDouble[30].pop(&x0_d[2]);

							x0[0]=x0_d[0]; x0[1]=x0_d[1]; x0[2]=x0_d[2];

							codeDouble[40].pop(&r_d);
							r=r_d;

							CoordinateSystem<float> XYZ;

							createObjectCoordSystem(&XYZ,Z);

							float x_center[3];
							XYZ.fromLocalToGlobal(x_center,x0);

							XYZ.setCenter(x_center);
							geom->addCircle(XYZ,r);

						}
						break;

					case ENT_ARC:
						{
							double x0_d[3];
							double x[3];

							double r_d;
							double Z_d[3];
							double f_d;

							float Z[3];
							float x0[3];
							float r;
							float fmin,fmax;

							int k,n;
							codeInt[90].pop(&n);
							if (codeDouble[210].pop(&Z_d[0])) {
								codeDouble[220].pop(&Z_d[1]);
								codeDouble[230].pop(&Z_d[2]);
							} else {
								Z_d[0]=0; Z_d[1]=0; Z_d[2]=1;
							}
							Z[0]=Z_d[0]; Z[1]=Z_d[1]; Z[2]=Z_d[2];

							codeDouble[10].pop(&x0_d[0]); 
							codeDouble[20].pop(&x0_d[1]);
							codeDouble[30].pop(&x0_d[2]);

							x0[0]=x0_d[0]; x0[1]=x0_d[1]; x0[2]=x0_d[2];

							codeDouble[40].pop(&r_d);
							r=r_d;

							codeDouble[50].pop(&f_d);
							fmin=f_d*3.14159/180.;
							codeDouble[51].pop(&f_d);
							fmax=f_d*3.14159/180.;


							CoordinateSystem<float> XYZ;

							createObjectCoordSystem(&XYZ,Z);

							float x_center[3];
							XYZ.fromLocalToGlobal(x_center,x0);

							XYZ.setCenter(x_center);
							geom->addArc(XYZ,r,fmin,fmax);

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
