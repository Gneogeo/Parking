#include "stl_reader.h"

#include "geometry.h"

#include <stdio.h>
#include <qdebug.h>

void readSTL(Geometry *geom,const char *name)
{
	FILE *fp=fopen(name,"rb");
	if (!fp) return;
	char header[80];
	unsigned int size;

	fread((void *)header,sizeof(char),80,fp);
	fread((void *)&size,sizeof(unsigned int),1,fp);

	int k,k1;
	for (k=0; k<size; k++) {
		if ((k&0xff)==0) qDebug("Reading triangle %d of %d",k,size);
		
		float data[4][3];
		
		float crd[3][3];
		float norm[3];

		fread((void *)data,sizeof(float),12,fp);
		char count[2];
		fread((void *)count,sizeof(char),2,fp);

		memcpy(norm,data[0],3*sizeof(float));
		memcpy(crd,data[1],9*sizeof(float));


		int id[3];
		for (k1=0; k1<3; k1++) {
			id[k1]=geom->addGrid(crd[k1][0],crd[k1][1],crd[k1][2]);
		}

		geom->addTriangle(id[0],id[1],id[2],0);
	}

	fclose(fp);

	
}