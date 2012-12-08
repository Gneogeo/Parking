#ifndef GEOMETRY_H
#define GEOMETRY_H

#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <malloc.h>
#include "vector3d.h"
#include "coord_system.h"


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
	CoordinateSystem<float> XYZ;
	float radius;
};

class Arc {
public:
	CoordinateSystem<float> XYZ;
	float radius;
	float fmin;
	float fmax;
};

template <typename T> class myVector {
	myVector(myVector &x); //deactivated copy-constructor
	unsigned int mem;
	unsigned int len;

public:
	T *data;
	

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
	void truncateInto(unsigned int ln) {
		if (ln<len) len=ln;
	}

	unsigned int length() {return len;}

	const T *getData() {return data;}

	T &at(unsigned int i) {return data[i];}
	
};



class Geometry 
{
public:

	myVector<Grid> grids;
	myVector<int> points;
	myVector<Line> lines;
	myVector<Triangle> triangles;
	myVector<Line> edges;
	myVector<Circle> circles;
	myVector<Arc> arcs;

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
	int addCircle(const CoordinateSystem<float> XYZ,float radius);
	int addArc(const CoordinateSystem<float> XYZ,float radius,float fmin,float fmax);
	
	void shrinkGeometry();
	void compressGrids();
	void calcTrianglesNormals();
	void calcTrianglesSmoothNormals();

	void recalcEdge(float angle);


	void loadSTL(char *name);
	void loadDXF(char *name);
	void load3DS(char *name);
	void loadIGES(char *name);

	void translateGeometry(float mat[4][4]);

	float edgeStripColor[4];
	float lineStripColor[4];

	int *edgeStrip;
	int *lineStrip;
	int *triaStripVertex;
	int *triaStripElement;

	void makeEdgeStrip();
	void makeLineStrip();
	void makeTriaStrip();

	void drawEdgeStrip();
	void drawLineStrip();

	void drawCircles();
	void drawArcs();
};



#endif
