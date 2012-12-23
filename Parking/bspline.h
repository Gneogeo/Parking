#ifndef BSPLINE_H
#define BSPLINE_H

class BSpline {
public:
	BSpline();
	BSpline & operator = (const BSpline &r);
	~BSpline();

	int K;
	int M; /* N=1+K-M */
	float *T; /* -M:N+M */
	float *W; /* 0:K */
	float (*P)[3]; /* 0:K */
	float V[2];

	int total_coords;
	float (*coords)[3];
	
	int getParamPoint(float t,float outp[3]) const;

	void recalcCoords(float dt);
};




#endif
