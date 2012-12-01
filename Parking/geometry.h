#ifndef GEOMETRY_H
#define GEOMETRY_H

#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include "vector3d.h"


class Grid {
public:
	int pos;
	float coords[3];
};


class Line {
public:
	int node[2];
};


class Triangle {
public:
	int node[3];
	vector3d<float> normal;
	vector3d<float> cnormal[3];
};

class Circle {
public:
	vector3d<float> center;
};


template <typename T> class myVector {
	myVector(myVector &x); //deactivated copy-constructor
	unsigned int mem;
	

public:
	T *data;
	unsigned int len;
	

	myVector() {
		data=0; len=0; mem=0;
	}
	~myVector() {
		free(data);
	}

	void append(const T &elem) {
		if (mem==len) {
			if (mem==0) mem=1;
			else mem*=2;
			data=(T*)realloc(data,mem*sizeof(T));
		}
		memcpy(&data[len],&elem,sizeof(T));
		len++;
	}

	void clear() {
		len=0;
	}

	void truncate() {
		free(data); data=0; len=0; mem=0;
	}

	unsigned int length() {return len;}

	const T *getData() {return data;}

	T &at(unsigned int i) {return data[i];}
	
};


template <typename T> class myStack {
	T *data;
	int datalen,datamem;
	myStack(myStack &x);
public:
	myStack() {
		data=(T *)malloc(sizeof(double));
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

class Geometry 
{
public:

	myVector<Grid> grids;
	myVector<int> points;
	myVector<Line> lines;
	myVector<Triangle> triangles;
	myVector<Line> edges;

	int pickedGrid;

	float minn[3],maxx[3];

	int hasSmoothNormals;

	Geometry();
	~Geometry();

	int addGrid(float x,float y,float z);
	int addPoint(int n);
	int addLine(int n1,int n2); 
	int addEdge(int n1,int n2);
	int addTriangle(int n1,int n2,int n3,float normal[3]);
	
	void shrinkGeometry();
	void compressGrids();
	void calcTrianglesNormals();
	void calcTrianglesSmoothNormals();

	void recalcEdge(float angle);


	void loadSTL(char *name);
	void loadDXF(char *name);
	void load3DS(char *name);

	void translateGeometry(float mat[4][4]);

	int *edgeStrip;
	int *lineStrip;
	int *triaStripVertex;
	int *triaStripElement;

	void makeEdgeStrip();
	void makeLineStrip();
	void makeTriaStrip();
};



#endif