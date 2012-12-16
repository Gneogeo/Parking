#include "bspline.h"


static float calc_B(float t,float *T,int n)
{
	if (n<2) return 0;

	int i,j;
	float *b;
	b=new float[n-1];
	for (i=0; i<n-1; i++) {
		if (t>=T[i] && t<T[i+1]) b[i]=1;
		else b[i]=0;
	}

/*	
	b(0,1),b(1,2),...b(i,i+1,i+2),...,b(n-2,n-1)

	b(0,1,2)=b(0,1)(t-T0)/(T1-T0)+b(1,2)(T2-t)/(T2-T1)
	...
	b(i,i+1,i+2)=b(i,i+1)(t-T(i))/(T(i+1)-T(i))+b(i+1,i+2)*(T(i+2)-t)/(T(i+2)-T(i+1))


	b(i,i+1,i+2,i+3)=b(i,i+1,i+2)(t-T(i)/(T(i+2)-T(i))+b(i+1,i+2,i+3)*(T(i+3)-T(i+1))

	*/



	for (j=1; j<n-2; j++) {
		for (i=0; i<n-1-j; i++) {
			float c;
			if (T[i+j]!=T[i]) {
				c=b[i]*(t-T[i])/(T[i+j]-T[i]);
			}
			if (T[i+j+1]!=T[i+j]) {
				c+= b[i+1]*(T[i+j+1]-t)/(T[i+j+1]-T[i+j]);
			}
			b[i]=c;
		}
	}

	float ret=b[0];
	delete []b;

	return ret;
	
}


int BSpline::getParamPoint(float t,float outp[3]) const
{
	if (t<V[0] || t>V[1]) return 0;
	
	float *b;
	b=new float[K+1];
	int i;
	for (i=0; i<=K; i++) {
		b[i]=calc_B(t,T+i,M+2);
	}

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
