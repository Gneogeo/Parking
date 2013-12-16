#ifndef GEOMETRY_H
#define GEOMETRY_H

#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <malloc.h>
#include <cstdlib>
#include "vector3d.h"
#include "coord_system.h"
#include "bspline.h"
#include "conic.h"

#include "myvector.h"

#include <QString>

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
};


class Circle {
public:
	CoordinateSystem<float> XYZ;
	float radius;
};

class ArcCircle {
public:
	CoordinateSystem<float> XYZ;
	float radius;
	float fmin;
	float fmax;
};


class Spline {
public:
	float Px[4];
	float Py[4];
	float Pz[4];
};


class RevolveLine {
public:	
	int line_axis_pos;
	int line_gen_pos;
	float fmin;
	float fmax;
};


class Geometry 
{
public:

	myVector<Grid> grids;
	myVector<int> points;
	myVector<Line> lines;
	myVector<Triangle> triangles;
	myVector< vector3d<float> >smooth_normals;
	myVector<Line> edges;
	myVector<Circle> circles;
	myVector<ArcCircle> arcs;
	myVector<Spline> splines;
	myVector<BSpline> bsplines;
	myVector<BSplineSurf> bsplinesurfs;
	myVector<ArcEllipse> arcellipses;
	myVector<ArcHyperbola> archyperbolas;

	myVector<RevolveLine> revolvelines;

	int pickedGrid;

	float minn[3],maxx[3];

	int hasSmoothNormals;
	int visible;
	QString name;

	Geometry();
	~Geometry();

	int addGrid(float x,float y,float z);
	int addPoint(int n);
	int addLine(int n1,int n2); 
	int addEdge(int n1,int n2);
	int addTriangle(int n1,int n2,int n3,float normal[3]);
	int addCircle(const CoordinateSystem<float> XYZ,float radius);
	int addArc(const CoordinateSystem<float> XYZ,float radius,float fmin,float fmax);
	int addSpline(float Px[4],float Py[4],float Pz[4]);
	int addBSpline(const BSpline &BS);
	int addBSplineSurf(const BSplineSurf &BSS);
	int addArcEllipse(const ArcEllipse &AE);
	int addArcHyperbola(const ArcHyperbola &AH);
	
	void shrinkGeometry();
	void compressGrids();
	void calcTrianglesNormals();
	void calcTrianglesSmoothNormals();

	void recalcEdge(float angle);


	void loadSTL(char *name);
	void loadOBJ(char *name);
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
	void drawSplines();
	void drawBSplines();
	void drawArcEllipses();
	void drawArcHyperbolas();

	void drawBSplineSurfs();

	void drawRevolveLines();
};



#endif
