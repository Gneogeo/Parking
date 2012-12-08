#include "chunck3ds_reader.h"

#include "geometry.h"

#include <stdio.h>
#include <qdebug.h>

const char *chunckName(unsigned short int id)
{
	switch (id) {
		case 0x4D4D: return "Main chunck";
		case 0x0002: return "3DS-Version";
		case 0x3D3D: return "3D Editor chunk";
		case 0x3D3E: return "Mesh version";
		case 0x0100: return "One unit";
		case 0x4000: return "OBJECT BLOCK";
		case 0x4100: return "Triangular Mesh";
		case 0x4110: return "Vertices list";
		case 0x4160: return "Local coordinate system";
		case 0x4120: return "Faces description";
		case 0xB000: return "Keyframer";


		default: return "Unsupported chunck";
	}

	return "???";

}


int readChunck(FILE *fp,Geometry *geom)
{
	unsigned short int chunck_id=0;
	unsigned long int chunck_len=0;
	fread((void *)&chunck_id,sizeof(short int),1,fp);
	fread((void *)&chunck_len,sizeof(long int),1,fp);
	
        const char *chName=chunckName(chunck_id);


	//qDebug("Chunck %x: len :%d (%s)",chunck_id,chunck_len,chName);

	if (feof(fp)) {
		return 0;
	}
	unsigned long int version;
	float one_unit;
	int k;
	unsigned short vertNum;
	float vector[3];
	float coord[12];

	static int firstVertexId;

	unsigned short int faceNum;
	unsigned short int face[3];
	unsigned short int faceFlag;

	QString obj_name;
	char c;

	unsigned long int remain,rm;

	remain=6;
	switch (chunck_id) {
		case 0x4d4d:
		case 0x3d3d:
		case 0x4100:
		case 0xb000:
			if (chunck_id==0x4100) firstVertexId=geom->grids.length();
			while (remain<chunck_len) {
				rm=readChunck(fp,geom); 
				if (!rm) {
					break;
				}
				remain+=rm;
			}
			break;
		case 0x2:
		case 0x3d3e:
			fread((void *)&version,sizeof(unsigned long int),1,fp);
			//qDebug("Version : %d",version);
			remain+=sizeof(unsigned long int);
			while (remain<chunck_len) {
				rm=readChunck(fp,geom); 
				if (!rm) {
					break;
				}
				remain+=rm;
			}
			break;
		case 0x100:
			fread((void *)&one_unit,sizeof(float),1,fp);
			//qDebug("One unit : %f",one_unit);
			remain+=sizeof(float);
			while (remain<chunck_len) {
				rm=readChunck(fp,geom); 
				if (!rm) {
					break;
				}
				remain+=rm;
			}
			break;
		case 0x4000:
			do {
				fread((void *)&c,sizeof(char),1,fp);
				remain++;
				if (c) obj_name.append(QChar::fromAscii(c));
			} while (c);
			//qDebug("Object name: %s",obj_name.toLocal8Bit().data());
			while (remain<chunck_len) {
				rm=readChunck(fp,geom); 
				if (!rm) {
					break;
				}
				remain+=rm;
			}
			break;
		case 0x4110:
			fread((void *)&vertNum,sizeof(unsigned short int),1,fp);
			remain+=sizeof(unsigned short int);
			//qDebug("Total vertices: %d",vertNum);
			for (k=0; k<vertNum; k++) {
				fread((void *)vector,sizeof(float),3,fp);
				remain+=3*sizeof(float);
				//qDebug("Vertex %d: %f,%f,%f",k,vector[0],vector[1],vector[2]);
				int id=geom->addGrid(vector[0],vector[1],vector[2]);
				//geom->addPoint(id);
			}
			while (remain<chunck_len) {
				rm=readChunck(fp,geom); 
				if (!rm) {
					break; 
				}
				remain+=rm;
			}
			break;
		case 0x4160:
			fread((void *)coord,sizeof(float),12,fp);
			remain+=12*sizeof(float);
			while (remain<chunck_len) {
				rm=readChunck(fp,geom); 
				if (!rm) {
					break;
				}
				remain+=rm;
			}
			break;
		case 0x4120:
			fread((void *)&faceNum,sizeof(unsigned short),1,fp);
			remain+=sizeof(unsigned short);
			//qDebug("Total faces : %d",faceNum);
			for (k=0; k<faceNum; k++) {
				fread((void *)face,sizeof(unsigned short[3]),1,fp);
				fread((void *)&faceFlag,sizeof(unsigned short),1,fp);
				remain+=sizeof(unsigned short[4]);
				face[0]+=firstVertexId;
				face[1]+=firstVertexId;
				face[2]+=firstVertexId;
				//qDebug("Face %d: %d,%d,%d (%d)",k,face[0],face[1],face[2],faceFlag);
				geom->addTriangle(face[0],face[1],face[2],0);
				if (faceFlag&1) geom->addEdge(face[2],face[0]);
				if (faceFlag&2) geom->addEdge(face[1],face[2]);
				if (faceFlag&4) geom->addEdge(face[0],face[1]);
			}
			while (remain<chunck_len) {
				rm=readChunck(fp,geom); 
				if (!rm) {
					break;
				}
				remain+=rm;
			}
			break;
		default:
			//qDebug("Passing through chunck...");
			fseek(fp,chunck_len-remain,SEEK_CUR);
			break;
	}
	//qDebug("Done with chunck %x (%s)",chunck_id,chName);
	
	return chunck_len;
}


void readChunck3DS(Geometry *geom,const char *name)
{
	FILE *fp=fopen(name,"rb");
	if (!fp) return;

	readChunck(fp,geom);




	fclose(fp);
}



class chunck3ds {
	chunck3ds *parent;
	chunck3ds **kids;
	int kidLen,kidMem;
	void *data;
public:
	chunck3ds(chunck3ds *par);
	~chunck3ds();
	void attachData(void *d,size_t dlen);
};

chunck3ds::chunck3ds(chunck3ds *par)
{
	parent=par;
	data=0;
	kids=(chunck3ds **)malloc(sizeof(chunck3ds *));
	kidLen=0; kidMem=1;
	if (parent) {
		if (parent->kidLen==parent->kidMem) {
			parent->kidMem*=2;
			parent->kids=(chunck3ds **)realloc(parent->kids,parent->kidMem*sizeof(chunck3ds *));
		}
		parent->kids[parent->kidLen]=this;
		parent->kidLen++;
	}
}

chunck3ds::~chunck3ds()
{
	int k;
	for (k=0; k<kidLen; k++) {
		kids[k]->parent=NULL;
		delete kids[k];
	}
	free(kids);
	if (parent) {
		int fnd=0;
		for (k=0; k<parent->kidLen; k++) {
			if (parent->kids[k]==this) {
				fnd=1;
			}
			if (fnd) {
				if (k<parent->kidLen-1) {
					parent->kids[k]=parent->kids[k+1];
				}
			}
		}
		if (fnd) parent->kidLen--;
	}
	free(data);
}


void chunck3ds::attachData(void *d,size_t dlen)
{
	if (data) free(data);
	data=0;
	if (dlen) {
		data=malloc(dlen);
		memcpy(data,d,dlen);
	}
}
