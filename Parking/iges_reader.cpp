#include "iges_reader.h"


#include <stdlib.h>
#include <math.h>
#include "geometry.h"
#include "coord_system.h"

#include <stdio.h>
#include <qdebug.h>


static void stripTrailingSpaces(char *d)
{
	if (!d) return;
	int n=strlen(d);
	for (;n>0; n--) {
		if (d[n-1]!=' ') {
			d[n]=0;
			break;
		}
	}
}

class IGES_line {
	public:
	char data[73];
	char letterCode;
	int sequenceNumber;

	char ParameterDelimiterChar;
	char RecordDelimiterChar;

	IGES_line() { 
		ParameterDelimiterChar=',';
		RecordDelimiterChar=';';
	}

	int readLine(FILE *fp) {
		if (feof(fp)) return 0;
		char temp[100];
		fgets(temp,100,fp);
		memcpy(data,temp,72*sizeof(char));
		data[72]=0;

		letterCode=temp[72];
		sequenceNumber=atoi(&temp[73]);
		return 1;
	}

	int readDelimString(const char *data,char *target) {
		int k=0;
		const char *pdata=data;
		while (pdata[0]!=ParameterDelimiterChar && pdata[0]!=RecordDelimiterChar) {
			target[k]=pdata[0];
			k++;
			pdata++;
			if (pdata[0]==0) break;
		}
		target[k]=0;
		if (data[k]!=0) k++;	
		return k;
	}

};

static int readHollerithString(const char *data,char *target)
{
	int ret=0;
	int total=0;
	while (data[0]!='H') {
            if (data[0]=='\0') {target[0]=0; return ret;}
            if (data[0]<'0' || data[0]>'9') {target[0]=0; return ret;}
		total=total*10+ data[0]-'0';
		data++; ret++;
	}
	data++; ret++;
	int k;
	for (k=0; k<total; k++) {
		target[k]=data[0];
		data++; ret++;
	}
	target[total]=0;
	return ret;
}


class IGES_directory {
public:
	int entityType;
	int parameterData;
	int structure;
	int lineFontPattern;
	int level;
	int view;
	int transMatrix;
	int labelDisplayAssoc;
	int statusNumber;
	int lineWeightNumber;
	int colorNumber;
	int paramLineCount;
	int formNumber;
	char entityLabel[9];
	int entitySubscriptNumber;
        IGES_directory() { memset(this,0,sizeof(IGES_directory)); }
	void fill(const char Directory[18][9]);
};

void IGES_directory::fill(const char Directory[18][9])
{
	entityType=atoi(Directory[0]);
	parameterData=atoi(Directory[1]);
	structure=atoi(Directory[2]);
	lineFontPattern=atoi(Directory[3]);
	level=atoi(Directory[4]);
	view=atoi(Directory[5]);
	transMatrix=atoi(Directory[6]);
	labelDisplayAssoc=atoi(Directory[7]);
	statusNumber=atoi(Directory[8]);
	lineWeightNumber=atoi(Directory[10]);
	colorNumber=atoi(Directory[11]);
	paramLineCount=atoi(Directory[12]);
	formNumber=atoi(Directory[13]);
	memcpy(entityLabel,Directory[16],9);
	entitySubscriptNumber=atoi(Directory[17]);
}


void readIGES(Geometry *geom,const char *name)
{

	FILE *fp=fopen(name,"rb");
	if (!fp) return;

	IGES_line igs;

	int ok=igs.readLine(fp);

	while (ok) {
		if (igs.letterCode!='S') break;
		ok=igs.readLine(fp);
	}

	int globalParamCount=0;
	char ParameterDelimiterChar=',';
	char RecordDelimiterChar=';';
	char GlobalParam[26][100];


	while (ok) {
		if (igs.letterCode!='G') break;
		
		stripTrailingSpaces(igs.data);
		char *pnt=igs.data;
		if (globalParamCount==0) {
			if (pnt[0]==',' && pnt[1]==',') {
				globalParamCount+=2;
				pnt+=2;
			} else if (pnt[0]=='1' && pnt[1]=='H' && pnt[2]==pnt[3] && pnt[3] !=pnt[4] && pnt[4]=='1' && pnt[5]=='H') {
				ParameterDelimiterChar=pnt[2];
				RecordDelimiterChar=pnt[6];
				pnt=&pnt[8];
				globalParamCount+=2;
			} else if (pnt[0]=='1' && pnt[1]=='H' && pnt[2]==pnt[3] && pnt[3]==pnt[4]) {
				ParameterDelimiterChar=pnt[2];
				pnt=&pnt[5];
				globalParamCount+=2;
			} else if (pnt[0]==',' && pnt[1]=='1' && pnt[2]=='H' && pnt[4]==',') {
				RecordDelimiterChar=pnt[3];
				pnt=&pnt[5];
				globalParamCount+=2;
			}
		}
		while (pnt[0]!=0) {
			switch (globalParamCount) {
				case 2: case 3: case 4: case 5:
				case 11:
				case 14:
				case 17:
				case 20: case 21:
				case 24: case 25:
					pnt+=readHollerithString(pnt,GlobalParam[globalParamCount]);
					if (pnt[0]!=0) {
						pnt++;
					}
					break;
				default:
					pnt+=igs.readDelimString(pnt,GlobalParam[globalParamCount]);
					break;

			}
			qDebug("Parameter %d is '%s'",globalParamCount,GlobalParam[globalParamCount]);
			globalParamCount++;
		}


		ok=igs.readLine(fp);
	}

	myVector<IGES_directory> dirlist;
	while (ok) {
		if (igs.letterCode!='D') break;
		char Directory[18][9];
		char *pnt=igs.data;
		int k;
		for (k=0; k<9; k++) {
			memcpy(Directory[k],pnt,8);
			Directory[k][8]=0;
			pnt+=8;
		}

		ok=igs.readLine(fp);
		if (!ok) break;
		if (igs.letterCode!='D') break;

                pnt=igs.data;

                for (k=9; k<18; k++) {
			memcpy(Directory[k],pnt,8);
			Directory[k][8]=0;
			pnt+=8;
		}
		IGES_directory IGESD;
		IGESD.fill(Directory);

		dirlist.append(IGESD);



		
		ok=igs.readLine(fp);

	}

	int igesCount=0;
	while (ok) {
		if (igs.letterCode!='P') break;

		QString ParameterLine=QString::fromUtf8("");
		while (ok) {
			igs.data[64]=0;
			stripTrailingSpaces(igs.data);
			int len=strlen(igs.data);
			ParameterLine.append(QString::fromLocal8Bit(igs.data));
/*TODO : What can I do if Record delimiter is the same as parameter delimiter? */
			if (len && igs.data[len-1]==RecordDelimiterChar) break;
			ok=igs.readLine(fp);
		}
//		qDebug("Parameter Line : '%s'",ParameterLine.toLocal8Bit().data());
		
		IGES_directory *igesd;
		igesd=&dirlist.at(igesCount);

		char param[100];
		char *cpnt,*pnt;

		cpnt=strdup(ParameterLine.toUtf8().data());
		pnt=cpnt;

		pnt+=igs.readDelimString(pnt,param);

		switch (igesd->entityType) {
			case 0: /*Null*/
				break;
			case 100: /*Circular arc*/
				{
					float x1,y1,x2,y2,x3,y3;
					float z;
					pnt+=igs.readDelimString(pnt,param);
					z=atof(param);
					pnt+=igs.readDelimString(pnt,param);
					x1=atof(param);
					pnt+=igs.readDelimString(pnt,param);
					y1=atof(param);
					pnt+=igs.readDelimString(pnt,param);
					x2=atof(param);
					pnt+=igs.readDelimString(pnt,param);
					y2=atof(param);
					pnt+=igs.readDelimString(pnt,param);
					x3=atof(param);
					pnt+=igs.readDelimString(pnt,param);
					y3=atof(param);
					CoordinateSystem<float> C;
					float xC[3];
					xC[0]=x1; xC[1]=y1; xC[2]=z;
					C.setCenter(xC);
					float fmin=atan2(y2-y1,x2-x1);
					float fmax=atan2(y3-y1,x3-x1);
					if (fmax<fmin) fmax+=2*3.14159;
					float rad=sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
					geom->addArc(C,rad,fmin,fmax);
				}
				break;
			case 104: /*Conic section*/
#if 0
				{
					float A,B,C,D,E,F;
					float Z;
					float x1,y1,x2,y2;

					if (igesd->formNumber==1) {
						/*Ellipse*/
					} else if (igesd->formNumber==2) {
						/*Hyperbola*/
					} else if (igesd->formNumber==3) {
						/*Parabola*/
						B^2=4AC

						Cy^2+(Bx+E)y+Ax^2+Dx+F=0
						disc=(Bx+E)^2-4C(Ax^2+Dx+F)

					}
					t=x0, y0=t^2
					x=ax0+by0+c
					y=dx0+ey0+f
					


					Ax^2+Bxy+Cy^2+Dx+Ey+F=0



				}
#endif
				break;
			case 106: /*Linear Path*/
				{
					pnt+=igs.readDelimString(pnt,param);
					int IP=atoi(param);
					pnt+=igs.readDelimString(pnt,param);
					int N=atoi(param);
					if (IP==1) {
						pnt+=igs.readDelimString(pnt,param);
						float Z=atof(param);
						float X,Y;
						int j;
						for (j=0; j<N; j++) {
							pnt+=igs.readDelimString(pnt,param);
							X=atof(param);
							pnt+=igs.readDelimString(pnt,param);
							Y=atof(param);
							
							int gid=geom->addGrid(X,Y,Z);
							if (j>0) {
								geom->addLine(gid-1,gid);
							}
						}
					} else if (IP==2) {
						float X,Y,Z;
						int j;
						for (j=0; j<N; j++) {
							pnt+=igs.readDelimString(pnt,param);
							X=atof(param);
							pnt+=igs.readDelimString(pnt,param);
							Y=atof(param);
							pnt+=igs.readDelimString(pnt,param);
							Z=atof(param);

							int gid=geom->addGrid(X,Y,Z);
							if (j>0) {
								geom->addLine(gid-1,gid);
							}
						}
					}
				}
				break;

			case 110: /*Line Entity*/
				{
					float x,y,z;
					int g1,g2;
					pnt+=igs.readDelimString(pnt,param);
					x=atof(param);
					pnt+=igs.readDelimString(pnt,param);
					y=atof(param);
					pnt+=igs.readDelimString(pnt,param);
					z=atof(param);
					g1=geom->addGrid(x,y,z);
					pnt+=igs.readDelimString(pnt,param);
					x=atof(param);
					pnt+=igs.readDelimString(pnt,param);
					y=atof(param);
					pnt+=igs.readDelimString(pnt,param);
					z=atof(param);
					g2=geom->addGrid(x,y,z);
					geom->addLine(g1,g2);
				}
				break;
			default:
				qDebug("Parameter %d not implemented yet",igesd->entityType);
		}

		igesCount++;


		free(pnt);

		ok=igs.readLine(fp);
	}


	/*TODO*/

	fclose(fp);
}
