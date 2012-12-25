#include "bspline.h"

#include "myvector.h"
#include "vector3d.h"

#include <cstdio>
#include <cstdlib>
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



BSplineSurf::BSplineSurf()
{
	K1=K2=0;
	M1=M2=0;
	S=T=0;
	W=0;
	P=0;
	memset(U,0,sizeof(U));
	memset(V,0,sizeof(V));

	total_coords=0;
	coords=0;

}


BSplineSurf & BSplineSurf::operator = (const BSplineSurf &r)
{
        K1=r.K1;
	K2=r.K2;
        M1=r.M1;
	M2=r.M2;


	S=(float*)malloc((K1+M1+2)*sizeof(float));
	memcpy(S,r.S,(K1+M1+2)*sizeof(float));
        T=(float*)malloc((K2+M2+2)*sizeof(float));
        memcpy(T,r.T,(K2+M2+2)*sizeof(float));

        W=(float*)malloc((K1+1)*(K2+1)*sizeof(float));
        memcpy(W,r.W,(K1+1)*(K2+1)*sizeof(float));

        P=(float(*)[3])malloc((K1+1)*(K2+1)*sizeof(float[3]));
        memcpy(P,r.P,(K1+1)*(K2+1)*sizeof(float[3]));

	memcpy(U,r.U,sizeof(r.U));
        memcpy(V,r.V,sizeof(r.V));

	total_coords=0;
	coords=0;
    
        return *this;
    
}   
    
BSplineSurf::~BSplineSurf()
{
	free(S);
        free(T);
        free(W);
        free(P);
	free(coords);
}


int BSplineSurf::getParamPoint(float s,float t,float outp[3]) const
{
	if (s<U[0] || s>U[1]) return 0;
	if (t<V[0] || t>V[1]) return 0;

	float *b1,*b2;
	b1=new float[K1+1];
	b2=new float[K2+1];

	int i,j;

	float *btemp1,*btemp2;

	btemp1=new float[1+K1+M1];
	btemp2=new float[1+K2+M2];

	for (i=0; i<=K1+M1; i++) {
		if (s>=S[i] && s<=S[i+1]) btemp1[i]=1;
		else if (S[i]==S[i+1] && s==S[i]) btemp1[i]=1;
		else btemp1[i]=0;
	}

	for (j=1; j<=M1; j++) {
		for (i=0; i<=K1+M1-j; i++) {
			float c=0;
			if (S[i+j]!=S[i]) {
				c=btemp1[i]*(s-S[i])/(S[i+j]-S[i]);
			}
			if (S[i+j+1]!=S[i+1]) {
				c+= btemp1[i+1]*(S[i+j+1]-s)/(S[i+j+1]-S[i+1]);
			}
			btemp1[i]=c;
		}
	}

	for (i=0; i<=K1; i++) {
		b1[i]=btemp1[i];
	}


	for (i=0; i<=K2+M2; i++) {
		if (t>=T[i] && t<=T[i+1]) btemp2[i]=1;
		else if (T[i]==T[i+1] && t==T[i]) btemp2[i]=1;
		else btemp2[i]=0;
	}


	for (j=1; j<=M2; j++) {
		for (i=0; i<=K2+M2-j; i++) {
			float c=0;
			if (T[i+j]!=T[i]) {
				c=btemp2[i]*(t-T[i])/(T[i+j]-T[i]);
			}
			if (T[i+j+1]!=T[i+1]) {
				c+= btemp2[i+1]*(T[i+j+1]-t)/(T[i+j+1]-T[i+1]);
			}
			btemp2[i]=c;
		}
	}

	for (i=0; i<=K2; i++) {
		b2[i]=btemp2[i];
	}
	

	delete []btemp1;
	delete []btemp2;



	float denom=0;
	outp[0]=0; outp[1]=0; outp[2]=0;
	for (j=0; j<=K2; j++) {
		for (i=0; i<=K1; i++) {
			int ij=i+j*(K1+1);
			float r=W[ij]*b1[i]*b2[j];
			outp[0]+= P[ij][0]*r;
			outp[1]+= P[ij][1]*r;
			outp[2]+= P[ij][2]*r;
			denom+=r;
		}
	}
	outp[0]/=denom;
	outp[1]/=denom;
	outp[2]/=denom;
	delete []b1;
	delete []b2;

	return 1;
}



