#include "bspline.h"

#include <cstdio>
#include <stdlib.h>
#include <cstring>

BSpline::BSpline()
{
	K=0;
	M=0;
	T=0;
	W=0;
	P=0;
	memset(V,0,sizeof(V));
	total_coords=0;
	coords=0;
}

BSpline & BSpline::operator = (const BSpline &r)
{
	K=r.K;
	M=r.M;

	T=(float*)malloc((K+M+2)*sizeof(float));
	memcpy(T,r.T,(K+M+2)*sizeof(float));
	W=(float*)malloc((K+1)*sizeof(float));
	memcpy(W,r.W,(K+1)*sizeof(float));
	P=(float(*)[3])malloc((K+1)*sizeof(float[3]));
	memcpy(P,r.P,(K+1)*sizeof(float[3]));
	memcpy(V,r.V,sizeof(r.V));

	total_coords=0;
	coords=0;

	return *this;

}

BSpline::~BSpline()
{
	free(T);
	free(W);
	free(P);
	free(coords);
}

int BSpline::getParamPoint(float t,float outp[3]) const
{
	if (t<V[0] || t>V[1]) return 0;

	float *b;
	b=new float[K+1];
	int i,j;

	float *btemp;

	btemp=new float[1+K+M];
	for (i=0; i<=K+M; i++) {
		if (t>=T[i] && t<=T[i+1]) btemp[i]=1;
		else if (T[i]==T[i+1] && t==T[i]) btemp[i]=1;
		else btemp[i]=0;
	}

	for (j=1; j<=M; j++) {
		for (i=0; i<=K+M-j; i++) {
			float c=0;
			if (T[i+j]!=T[i]) {
				c=btemp[i]*(t-T[i])/(T[i+j]-T[i]);
			}
			if (T[i+j+1]!=T[i+1]) {
				c+= btemp[i+1]*(T[i+j+1]-t)/(T[i+j+1]-T[i+1]);
			}
			btemp[i]=c;
		}
	}

	for (i=0; i<=K; i++) {
		b[i]=btemp[i];
	}
	

	delete []btemp;



	float denom=0;
	outp[0]=0; outp[1]=0; outp[2]=0;
	for (i=0; i<=K; i++) {
		float r=W[i]*b[i];
		outp[0]+= P[i][0]*r;
		outp[1]+= P[i][1]*r;
		outp[2]+= P[i][2]*r;
		denom+=r;
	}
	outp[0]/=denom;
	outp[1]/=denom;
	outp[2]/=denom;
	delete []b;

	return 1;
}

#include "myvector.h"
#include "vector3d.h"

void BSpline::recalcCoords(float dt)
{
	int j;
	float t;
	vector3d<float> X;

	myVector< vector3d<float> > F;

	for (t=V[0]; t<V[1]; t+=dt) {
		if (getParamPoint(t,X.data)) {
			F.append(X);
		}
	}
	t=V[1];
	if (getParamPoint(t,X.data)) {
		F.append(X);
	}
	free(coords);
	coords=(float(*)[3])calloc(F.length(),sizeof(float[3]));
	total_coords=F.length();
	for (j=0; j<total_coords; j++) {
		memcpy(coords[j],F.at(j).data,sizeof(float[3]));
	}

}
