#include "bspline.h"

#include <cstdio>
#include <stdlib.h>
#include <cstring>

static float calc_B(float t,float *T,int n,float *btemp)
{
	if (n<2) return 0;

	int i,j;

	for (i=0; i<n-1; i++) {
        if (t>=T[i] && t<=T[i+1]) btemp[i]=1;
        else if (T[i]==T[i+1] && t==T[i]) btemp[i]=1;
        else btemp[i]=0;
	}

/*	
	b(0,1),b(1,2),...b(i,i+1,i+2),...,b(n-2,n-1)
	
	j=1
	i=0:n-3
	b(0,1,2)=b(0,1)(t-T0)/(T1-T0)+b(1,2)(T2-t)/(T2-T1)
	...
	b(i,i+1,i+2)=b(i,i+1)(t-T(i))/(T(i+1)-T(i))+b(i+1,i+2)*(T(i+2)-t)/(T(i+2)-T(i+1))
	...
	b(n-3,n-2,n-1)=b(n-3,n-2)(t-T(n-3))/(T(n-2)-T(n-3))+b(n-2,n-1)*(T(n-1)-t)/(T(n-1)-T(n-2))


	j=2
	i=0:n-4
	b(0,1,2,3)=b(0,1,2)(t-T0)/(T2-T0)+b(1,2,3)(T3-t)/(T3-T1)
	...
	b(i,i+1,i+2,i+3)=b(i,i+1,i+2)(t-T(i)/(T(i+2)-T(i))+b(i+1,i+2,i+3)*(T(i+3)-t)/(T(i+3)-T(i+1))
	...
	b(n-4,n-3,n-2,n-1)=b(n-4,n-3,n-2)(t-T(n-4)/(T(n-2)-T(n-4))+b(n-3,n-2,n-1)*(T(n-1)-t)/(T(n-1)-T(n-3))


	b(i,..,i+j+1)=b(i,..,i+j)(t-T(i)/(T(i+j)-T(i))+b(i+1,..,i+j+1)*(T(i+j+1)-t)/(T(i+j+1)-T(i+1))

	j=n-2
	i=0:0
	b(0,..,n-1)=b(0,..,n-2)(t-T(0)/(T(n-2)-T(0))+b(1,..,n-1)*(T(n-1)-t)/(T(n-1)-T(1))

	*/


	for (j=1; j<n-1; j++) {
		for (i=0; i<n-1-j; i++) {
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

    float ret=btemp[0];

	return ret;
	
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



	return *this;

}

BSpline::~BSpline()
{
	free(T);
	free(W);
	free(P);
}

int BSpline::getParamPoint(float t,float outp[3]) const
{
	if (t<V[0] || t>V[1]) return 0;
	
	float *b;
	b=new float[K+1];
	int i;
	float sbi=0;

	float *T1;
	int N=1+K-M;
	T1=(float*)calloc(N+2*M+1,sizeof(float));
	for (i=0; i<N+2*M+1; i++) {
		T1[i]=float(i)/float(N+2*M-3);
		if (T1[i]>1) T1[i]=1;
	}

    float *btemp;
    btemp=new float[M+1];
	for (i=0; i<=K; i++) {
        b[i]=calc_B(t,T+i,M+2,btemp);
		sbi+=b[i];
	}
    delete []btemp;

	free(T1);

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
