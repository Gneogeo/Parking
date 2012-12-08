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

};

static int readHollerithString(const char *data,char *target)
{
	int ret=0;
	int total=0;
	while (data[0]!='H') {
		if (data[0]=='\0') return ret;
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

static int readDelimiterString(const char *data,char delim,char delim1,char *target)
{
	int k=0;
	while (data[0]!=delim && data[0]!=delim1) {
		target[k]=data[0];
		k++;
		data++;
		if (data[0]==0) break;
	}
	target[k]=0;
	return k;	
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
					pnt+=readDelimiterString(pnt,ParameterDelimiterChar,RecordDelimiterChar,GlobalParam[globalParamCount]);
					if (pnt[0]!=0) {
						pnt++;
					}
					break;

			}
			qDebug("Parameter %d is '%s'",globalParamCount,GlobalParam[globalParamCount]);
			globalParamCount++;
		}


		ok=igs.readLine(fp);
	}

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



		
		ok=igs.readLine(fp);

	}


	/*TODO*/

	fclose(fp);
}
