#ifndef BSPLINE_H
#define BSPLINE_H

class BSpline {
public:
	BSpline();
	BSpline & operator = (const BSpline &r);
	~BSpline();

	void resize();

	int K;
	int M; /* N=1+K-M */
	float *T; /* -M:1+K */
	float *W; /* 0:K */

	float (*P)[3]; /* 0:K */
	float V[2];

	int total_coords;
	float (*coords)[3];

	int *strip;
	
	int getParamPoint(float t,float outp[3]) const;

	void recalcCoords(float dt);
};


class BSplineSurf {
public:
	BSplineSurf();
	BSplineSurf & operator = (const BSplineSurf &r);
	~BSplineSurf();

	void resize();

	int K1;
	int K2;
	int M1;
	int M2;
	/*N1=1+K1-M1*/
	/*N2=1+K2-M2*/
	float *S; /* -M1:1+K1 */
	float *T; /* -M2:1+K2 */
	float *W; /* (0:K1),(0:K2) */
	float (*P)[3]; /* (0:K1),(0:K2) */
	float U[2];
	float V[2];

	int total_coords;
	float (*coords)[3];
	int *strip;

	float (*normals)[3];

	int getParamPoint(float s,float t,float outp[3]) const;

	void recalcCoords(float ds,float dt);


};



#endif
