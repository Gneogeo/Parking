#include "iges_reader.h"


#include <stdlib.h>
#include <math.h>
#include "geometry.h"
#include "coord_system.h"

#include <stdio.h>
#include <qdebug.h>
#include <QSet>

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
	char GlobalParam[26][100];


	QString globalData=QString::fromUtf8("");
	while (ok) {
		if (igs.letterCode!='G') break;
		stripTrailingSpaces(igs.data);
		globalData.append(QString::fromUtf8(igs.data));
		ok=igs.readLine(fp);
	}
	if (globalData.count()!=0) {
		char *cpnt,*pnt;
		cpnt=strdup(globalData.toUtf8().data());
		pnt=cpnt;
		if (globalParamCount==0) {
			if (pnt[0]==',' && pnt[1]==',') {
				globalParamCount+=2;
				pnt+=2;
			} else if (pnt[0]=='1' && pnt[1]=='H' && pnt[2]==pnt[3] && pnt[3] !=pnt[4] && pnt[4]=='1' && pnt[5]=='H') {
				igs.ParameterDelimiterChar=pnt[2];
				igs.RecordDelimiterChar=pnt[6];
				pnt=&pnt[8];
				globalParamCount+=2;
			} else if (pnt[0]=='1' && pnt[1]=='H' && pnt[2]==pnt[3] && pnt[3]==pnt[4]) {
				igs.ParameterDelimiterChar=pnt[2];
				pnt=&pnt[5];
				globalParamCount+=2;
			} else if (pnt[0]==',' && pnt[1]=='1' && pnt[2]=='H' && pnt[4]==',') {
				igs.RecordDelimiterChar=pnt[3];
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

		free(cpnt);
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
	QString ParameterLine;
	QString nextParameterLine;

	nextParameterLine=QString::fromLocal8Bit("");

	QSet<int> usedParamSet;

	QMap<int,CoordinateSystem<float> > crdMap;
	QMap<int,int> depcrdMap;
	QMap<int,int> arcCoordMap;
	QMap<int,int> gridCoordMap;
	QMap<int,int> bsplineCoordMap;
	QMap<int,int> bsplineSurfCoordMap;

	QMap<int,int> lineMap;


	
	


	while (ok) {
		if (igs.letterCode!='P') break;

		int DE=2*igesCount+1;
		int paramId;
		ParameterLine=nextParameterLine;
		nextParameterLine=QString::fromLocal8Bit("");
		while (ok) {
			if (igs.letterCode!='P') break;

			char paramIdstr[9]={0};
			memcpy(paramIdstr,&igs.data[64],8);
			paramIdstr[8]=0;
			paramId=atoi(paramIdstr);

			igs.data[64]=0;
			stripTrailingSpaces(igs.data);

			if (paramId>DE) {
				nextParameterLine.append(QString::fromLocal8Bit(igs.data));
				break;
			}
			ParameterLine.append(QString::fromLocal8Bit(igs.data));
			ok=igs.readLine(fp);
		}
		//qDebug("Parameter Line : '%s'",ParameterLine.toLocal8Bit().data());

		IGES_directory *igesd;
		igesd=&dirlist.at(igesCount);

		char param[100];
		char *cpnt,*pnt;

		cpnt=strdup(ParameterLine.toLocal8Bit().data());
		pnt=cpnt;

		pnt+=igs.readDelimString(pnt,param);

		if (!usedParamSet.contains(igesd->entityType)) {
			qDebug("Entity: %d",igesd->entityType);
		}
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
					int arcid=geom->addArc(C,rad,fmin,fmax);
					if (igesd->transMatrix!=0) {
						arcCoordMap.insert(arcid,igesd->transMatrix);
					}

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
						x=cx0+sy0+a
						y=-sx0+cy0+b



						Ax^2+Bxy+Cy^2+Dx+Ey+F=0

						A(c^2x0^2+s^2y0^2+a^2+2csx0y0+2say0+2acx0)
						+B(-scx0^2+(s^2+c^2)x0y0+(as+bc)x0+csy0^2+(ac+bs)y0+ab)
						+C(s^2x0^2+c^2y0^2+b^2-2scx0y0+2bcy0-2bsx0)
						+D(cx0+sy0+a)
						+E(-sx0+cy0+b)
						+F

						(Ac^2-Bsc+Cs^2)x0^2
						+(As^2+Bsc+Cc^2)y0^2
						+(2Asc+B(s^2+c^2)-2Csc)x0y0
						+(2Aac+B(as+bc)-2Cbs+Dc-Es)x0
						+(2As+B(ac+bs)+2Cbc+Ds+Ec)y0
						+(Aa^2+Bab+Cb^2+Da+Eb+F)

						1: s^2+c^2=1

						sc=B/(2(C-A))


						yparxei eutheia y=ax+b <->x=y/a-b/a

						A(y/a-b/a)^2+B(y/a-b/a)(ax+b)+C(ax+b)^2+D(y/a-b/a)+E(ax+b)+F= Ax^2+Bxy+Cy^2+Dx+Ey+F
						A(y^2/a^2+b^2/a^2-2yb/a^2)+B(xy+by/a-bx-b^2/a)+C(a^2x^2+b^2+2abx)+D(y/a-b/a)+E(ax+b)+F

						(Ca^2)x^2+(B)xy+(A/a^2)y^2+(-Bb+2Cab+Ea)x+(Bb/a-2Ab/a^2+D/a)y+Ab^2/a^2-Bb^2/a+Cb^2-Db/a+Eb+F

						a^2=A/C
						2Ab+EA/C=Da+Bab

						-Bb+2Cab+Ea=D 
						Bb/a-2Ab/a^2+D/a=E
						Ab^2/a^2-Bb^2/a+Cb^2-Db/a+Eb+F=F
						A(-b/a)^2+B(-b/a)b+Cb^2+D(-b/a)+Eb+F=F

						Ca^2=A -> a^2=A/C
						A/a^2=C -> Ca^2=A
						-Bb+2Cab+Ea=D -> b=(D-Ea)/(2Ca-B)
						Bb/a-2Ab/a^2+D/a=E -> b=(D-Ea)/(2A/a-B)
						A(-b/a)^2+B(-b/a)b+Cb^2+D(-b/a)+Eb=0 -> (-b^2)C+B(-b/a)b+Cb^2+D(-b/a)+Eb=0
						1: a=sqrt(A/C)
						

						

						1: a=sqrt(A/C)

						








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
							if (igesd->transMatrix!=0) {
								gridCoordMap.insert(gid,igesd->transMatrix);
							}
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
							if (igesd->transMatrix!=0) {
								gridCoordMap.insert(gid,igesd->transMatrix);
							}
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
					if (igesd->transMatrix!=0) {
						gridCoordMap.insert(g1,igesd->transMatrix);
					}
					pnt+=igs.readDelimString(pnt,param);
					x=atof(param);
					pnt+=igs.readDelimString(pnt,param);
					y=atof(param);
					pnt+=igs.readDelimString(pnt,param);
					z=atof(param);
					g2=geom->addGrid(x,y,z);
					if (igesd->transMatrix!=0) {
						gridCoordMap.insert(g2,igesd->transMatrix);
					}
					int lid=geom->addLine(g1,g2);
					lineMap.insert(DE,lid);
				}
				break;
			case 112: /*Spline*/
				{
					int N;
					float Px[4],Py[4],Pz[4];
					float *T;
					pnt+=igs.readDelimString(pnt,param);
					pnt+=igs.readDelimString(pnt,param);
					pnt+=igs.readDelimString(pnt,param);
					pnt+=igs.readDelimString(pnt,param);
					N=atoi(param);

					int j;

					T=new float[N+1];
					for (j=0; j<=N; j++) {
						pnt+=igs.readDelimString(pnt,param);
						T[j]=atof(param);
					}
					for (j=0; j<N; j++) {
						pnt+=igs.readDelimString(pnt,param); Px[0]=atof(param);
						pnt+=igs.readDelimString(pnt,param); Px[1]=atof(param);
						pnt+=igs.readDelimString(pnt,param); Px[2]=atof(param);
						pnt+=igs.readDelimString(pnt,param); Px[3]=atof(param);

						pnt+=igs.readDelimString(pnt,param); Py[0]=atof(param);
						pnt+=igs.readDelimString(pnt,param); Py[1]=atof(param);
						pnt+=igs.readDelimString(pnt,param); Py[2]=atof(param);
						pnt+=igs.readDelimString(pnt,param); Py[3]=atof(param);

						pnt+=igs.readDelimString(pnt,param); Pz[0]=atof(param);
						pnt+=igs.readDelimString(pnt,param); Pz[1]=atof(param);
						pnt+=igs.readDelimString(pnt,param); Pz[2]=atof(param);
						pnt+=igs.readDelimString(pnt,param); Pz[3]=atof(param);

						float s,s2,s3;
						s=T[j+1]-T[j];
						s2=s*s;
						s3=s*s*s;
						Px[1]/=s; Px[2]/=s2; Px[3]/=s3;
						Py[1]/=s; Py[2]/=s2; Py[3]/=s3;
						Pz[1]/=s; Pz[2]/=s2; Pz[3]/=s3;
					}
					delete []T;

				}
				break;
			case 120:
				/*Surface of Revolution*/
				{
					int axis_id;
					int gen_id;
					float f1;
					float f2;

					pnt+=igs.readDelimString(pnt,param); axis_id=atoi(param);
					pnt+=igs.readDelimString(pnt,param); gen_id=atoi(param);
					pnt+=igs.readDelimString(pnt,param); f1=atof(param);
                                        pnt+=igs.readDelimString(pnt,param); f2=atof(param);



					IGES_directory *gend;
                                        gend=&dirlist.at((gen_id-1)/2);
					if (gend->entityType==110) {
						RevolveLine RL;
						RL.line_axis_pos=axis_id;
						RL.line_gen_pos=gen_id;
						RL.fmin=f1;
						RL.fmax=f2;

						geom->revolvelines.append(RL);
					} else {
						qDebug("Revolving of %d not supported yet",gend->entityType);
					}
					

				}
				break;
			case 124:
				/*Coordinate system*/
				{
					CoordinateSystem<float> Crd;
					float x[3],y[3],z[3],c[3];
					pnt+=igs.readDelimString(pnt,param); x[0]=atof(param);
					pnt+=igs.readDelimString(pnt,param); y[0]=atof(param);
					pnt+=igs.readDelimString(pnt,param); z[0]=atof(param);
					pnt+=igs.readDelimString(pnt,param); c[0]=atof(param);
					pnt+=igs.readDelimString(pnt,param); x[1]=atof(param);
					pnt+=igs.readDelimString(pnt,param); y[1]=atof(param);
					pnt+=igs.readDelimString(pnt,param); z[1]=atof(param);
					pnt+=igs.readDelimString(pnt,param); c[1]=atof(param);
					pnt+=igs.readDelimString(pnt,param); x[2]=atof(param);
					pnt+=igs.readDelimString(pnt,param); y[2]=atof(param);
					pnt+=igs.readDelimString(pnt,param); z[2]=atof(param);
					pnt+=igs.readDelimString(pnt,param); c[2]=atof(param);
					Crd.setAxis(x,y,z);
					Crd.setCenter(c);
					crdMap.insert(DE,Crd);
					if (igesd->transMatrix!=0) {
						depcrdMap.insert(DE,igesd->transMatrix);
					}

				}
				break;
			case 126:
				{ 
					/*B-Spline curves*/
					BSpline BS;

					pnt+=igs.readDelimString(pnt,param);
					BS.K=atoi(param);
					pnt+=igs.readDelimString(pnt,param);
					BS.M=atoi(param);
					pnt+=igs.readDelimString(pnt,param);
					pnt+=igs.readDelimString(pnt,param);
					pnt+=igs.readDelimString(pnt,param);
					pnt+=igs.readDelimString(pnt,param);
					BS.T=(float*)calloc(BS.K+BS.M+2,sizeof(float));
					BS.W=(float*)calloc(BS.K+1,sizeof(float));
					BS.P=(float(*)[3])calloc(BS.K+1,sizeof(float[3]));

					int j;
					for (j=0; j<BS.K+BS.M+2; j++) {
						pnt+=igs.readDelimString(pnt,param);
						BS.T[j]=atof(param);
					}
					for (j=0; j<BS.K+1; j++) {
						pnt+=igs.readDelimString(pnt,param);
						BS.W[j]=atof(param);
					}
					for (j=0; j<BS.K+1; j++) {
						pnt+=igs.readDelimString(pnt,param);
						BS.P[j][0]=atof(param);
						pnt+=igs.readDelimString(pnt,param);
						BS.P[j][1]=atof(param);
						pnt+=igs.readDelimString(pnt,param);
						BS.P[j][2]=atof(param);
					}
					pnt+=igs.readDelimString(pnt,param);
					BS.V[0]=atof(param);
					pnt+=igs.readDelimString(pnt,param);
					BS.V[1]=atof(param);

					int bsid=geom->addBSpline(BS);
					if (igesd->transMatrix!=0) {
						bsplineCoordMap.insert(bsid,igesd->transMatrix);
					}

				}
				break;

			case 128:
				{
					/*B-Spline surfaces*/
					BSplineSurf BSS;

					pnt+=igs.readDelimString(pnt,param);
					BSS.K1=atoi(param);
					pnt+=igs.readDelimString(pnt,param);
					BSS.K2=atoi(param);
					pnt+=igs.readDelimString(pnt,param);
					BSS.M1=atoi(param);
					pnt+=igs.readDelimString(pnt,param);
					BSS.M2=atoi(param);

					pnt+=igs.readDelimString(pnt,param);
					pnt+=igs.readDelimString(pnt,param);
					pnt+=igs.readDelimString(pnt,param);
					pnt+=igs.readDelimString(pnt,param);
					pnt+=igs.readDelimString(pnt,param);

					BSS.S=(float*)calloc(BSS.K1+BSS.M1+2,sizeof(float));
					BSS.T=(float*)calloc(BSS.K2+BSS.M2+2,sizeof(float));
					BSS.W=(float*)calloc((BSS.K1+1)*(BSS.K2+1),sizeof(float));
					BSS.P=(float(*)[3])calloc((BSS.K1+1)*(BSS.K2+1),sizeof(float[3]));

					int i,j,ij;
					for (j=0; j<BSS.K1+BSS.M1+2; j++) {
						pnt+=igs.readDelimString(pnt,param); BSS.S[j]=atof(param);
					}
					for (j=0; j<BSS.K2+BSS.M2+2; j++) {
						pnt+=igs.readDelimString(pnt,param); BSS.T[j]=atof(param);
					}
					for (j=0; j<BSS.K2+1; j++) {
						for (i=0; i<BSS.K1+1; i++) {
							ij=i+j*(BSS.K1+1);
							pnt+=igs.readDelimString(pnt,param); BSS.W[ij]=atof(param);
						}
					}
					for (j=0; j<BSS.K2+1; j++) {
						for (i=0; i<BSS.K1+1; i++) {
							ij=i+j*(BSS.K1+1);
							pnt+=igs.readDelimString(pnt,param); 
							BSS.P[ij][0]=atof(param);
							pnt+=igs.readDelimString(pnt,param);
							BSS.P[ij][1]=atof(param);
							pnt+=igs.readDelimString(pnt,param);
							BSS.P[ij][2]=atof(param);
						}
					}
					pnt+=igs.readDelimString(pnt,param);
					BSS.U[0]=atof(param);
					pnt+=igs.readDelimString(pnt,param);
					BSS.U[1]=atof(param);
					pnt+=igs.readDelimString(pnt,param);
					BSS.V[0]=atof(param);
					pnt+=igs.readDelimString(pnt,param);
					BSS.V[1]=atof(param);

					int bssid=geom->addBSplineSurf(BSS);
					if (igesd->transMatrix!=0) {
						bsplineSurfCoordMap.insert(bssid,igesd->transMatrix);
					}
				}
				break;

			default:
				if (!usedParamSet.contains(igesd->entityType)) {
					qDebug("Parameter %d not implemented yet",igesd->entityType);
				}
				break;
		}

		usedParamSet.insert(igesd->entityType);

		igesCount++;


		free(cpnt);

		ok=igs.readLine(fp);
	}

        /*Coord fixing*/
	
	QMap<int,int>::iterator it;
	for (it=depcrdMap.begin(); it!=depcrdMap.end(); ++it) {
		int c1_id,c2_id;
		c1_id=it.key();
		c2_id=it.value();
		if (c1_id!=0) {
			if (crdMap.find(c1_id)!=crdMap.end()) {
				CoordinateSystem<float> c1=crdMap.find(c1_id).value();

				QSet<int> sett;
				sett.insert(c1_id);
				sett.insert(c2_id);
				for (;;) {
					if (crdMap.find(c2_id)==crdMap.end()) break;
					CoordinateSystem<float> c2=crdMap.find(c2_id).value();

					c2.fromLocalToGlobal(&c1);

					if (depcrdMap.find(c2_id)==depcrdMap.end()) break;
					c2_id=depcrdMap.find(c2_id).value();

					if (sett.contains(c2_id)) break;
					sett.insert(c2_id);
				}
			}
		}
	}
	
	for (it=arcCoordMap.begin(); it!=arcCoordMap.end(); ++it) {
		int c1_id,c2_id;
		c1_id=it.key();
		c2_id=it.value();

		ArcCircle *T=&geom->arcs.at( it.key() );

		if (crdMap.find(c2_id)!=crdMap.end()) {
			CoordinateSystem<float> c=crdMap.find(c2_id).value();

			c.fromLocalToGlobal(&T->XYZ);
		}
	}

	for (it=gridCoordMap.begin(); it!=gridCoordMap.end(); ++it) {
		int c1_id,c2_id;
		c1_id=it.key();
		c2_id=it.value();


		Grid *G=&geom->grids.at( c1_id );

		if (crdMap.find(c2_id)!=crdMap.end()) {
			CoordinateSystem<float> c=crdMap.find(c2_id).value();

			float tmp[3];
			c.fromLocalToGlobal(tmp,G->coords);
			vec_copy(G->coords,tmp);
		}
	}

	for (it=bsplineCoordMap.begin(); it!=bsplineCoordMap.end(); ++it) {
		int c1_id,c2_id;
		c1_id=it.key();
		c2_id=it.value();

		BSpline *BS=&geom->bsplines.at( c1_id );

		if (crdMap.find(c2_id)!=crdMap.end()) {
			CoordinateSystem<float> c=crdMap.find(c2_id).value();

			int j;
			float tmp[3];
			for (j=0; j<=BS->K; j++) {
				c.fromLocalToGlobal(tmp,BS->P[j]);
				vec_copy(BS->P[j],tmp);
			}
		}
	}

	for (it=bsplineSurfCoordMap.begin(); it!=bsplineSurfCoordMap.end(); ++it) {
		int c1_id,c2_id;
		c1_id=it.key();
		c2_id=it.value();

		BSplineSurf *BSS=&geom->bsplinesurfs.at( c1_id );

		if (crdMap.find(c2_id)!=crdMap.end()) {
			CoordinateSystem<float> c=crdMap.find(c2_id).value();

			int i,j,ij;
			float tmp[3];
			for (j=0; j<=BSS->K2; j++) {
				for (i=0; i<=BSS->K1; i++) {
					ij=i+j*(BSS->K1+1);
					c.fromLocalToGlobal(tmp,BSS->P[j]);
					vec_copy(BSS->P[j],tmp);
				}
			}
		}
	}

	for (int i=0; i<geom->bsplines.length(); i++) {
		BSpline & BS=geom->bsplines.at(i);
		BS.recalcCoords(0.05);
	}

	for (int i=0; i<geom->bsplinesurfs.length(); i++) {
		BSplineSurf &BSS=geom->bsplinesurfs.at(i);
		BSS.recalcCoords(0.2,0.2);
	}

	for (int i=0; i<geom->revolvelines.length(); i++) {
		RevolveLine &RL=geom->revolvelines.at(i);
		RL.line_axis_pos=lineMap.find(RL.line_axis_pos).value();
		RL.line_gen_pos=lineMap.find(RL.line_gen_pos).value();

	}

	fclose(fp);
}
