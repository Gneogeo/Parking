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
	strip=0;
}

void BSpline::resize()
{
	free(T);
	free(W);
	free(P);

	T=(float*)calloc((K+M+2),sizeof(float));
	W=(float*)calloc((K+1),sizeof(float));
	P=(float(*)[3])calloc((K+1),sizeof(float[3]));

}

BSpline & BSpline::operator = (const BSpline &r)
{
	K=r.K;
	M=r.M;

	resize();

	memcpy(T,r.T,(K+M+2)*sizeof(float));
	memcpy(W,r.W,(K+1)*sizeof(float));
	memcpy(P,r.P,(K+1)*sizeof(float[3]));
	memcpy(V,r.V,sizeof(r.V));

	total_coords=0;
	coords=0;
	strip=0;

	return *this;

}



BSpline::~BSpline()
{
	free(T);
	free(W);
	free(P);
	free(coords);
	free(strip);
}



int BSpline::getParamPoint(float t,float outp[3]) const
{
	if (t<V[0] || t>V[1]) {
		vec_zero(outp);
		return 0;
	}

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

	free(strip);
	strip=(int*)calloc(total_coords+2,sizeof(int));
	strip[0]=total_coords;
	
	for (j=0; j<total_coords; j++) {
		memcpy(coords[j],F.at(j).data,sizeof(float[3]));
		strip[j+1]=j;
	}
	strip[total_coords+1]=0;

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
	normals=0;
	strip=0;

}

void BSplineSurf::resize()
{
	free(S);
	free(T);
	free(W);
	free(P);
	S=(float*)calloc((K1+M1+2),sizeof(float));
	T=(float*)calloc((K2+M2+2),sizeof(float));
	W=(float*)calloc((K1+1)*(K2+1),sizeof(float));
    P=(float(*)[3])calloc((K1+1)*(K2+1),sizeof(float[3]));
            
}

BSplineSurf & BSplineSurf::operator = (const BSplineSurf &r)
{
        K1=r.K1;
	K2=r.K2;
        M1=r.M1;
	M2=r.M2;

	resize();

	memcpy(S,r.S,(K1+M1+2)*sizeof(float));
    memcpy(T,r.T,(K2+M2+2)*sizeof(float));
	memcpy(W,r.W,(K1+1)*(K2+1)*sizeof(float));
	memcpy(P,r.P,(K1+1)*(K2+1)*sizeof(float[3]));
	memcpy(U,r.U,sizeof(r.U));
	memcpy(V,r.V,sizeof(r.V));

	total_coords=0;
	coords=0;
	normals=0;
	strip=0;
    
        return *this;
    
}   
    
BSplineSurf::~BSplineSurf()
{
	free(S);
        free(T);
        free(W);
        free(P);
	free(coords);
	free(normals);
}


int BSplineSurf::getParamPoint(float s,float t,float outp[3]) const
{
	if (s<U[0] || s>U[1]) {
		vec_zero(outp);
		return 0;
	}
	if (t<V[0] || t>V[1]) {
		vec_zero(outp);
		return 0;
	}

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

void BSplineSurf::recalcCoords(float ds,float dt)
{
	float s,t;
	float out[3];
	int ns,nt;
	
	myVector<float> sv;
	myVector<float> tv;
	for (s=U[0]; s<U[1]; s+=ds) {
		sv.append(s);
	}
	sv.append(U[1]);

	for (t=V[0]; t<V[1]; t+=dt) {
		tv.append(t);
	}
	tv.append(V[1]);
	
	total_coords=sv.length()*tv.length();
	coords=(float(*)[3])malloc(total_coords*sizeof(float[3]));

	total_coords=0;
	for (ns=0; ns<sv.length(); ns++) {
		s=sv.at(ns);
		for (nt=0; nt<tv.length(); nt++) {
			t=tv.at(nt);	
			getParamPoint(s,t,coords[total_coords]);
			total_coords++;
		}
	}
	myVector<int> N;
	int striplen;
	for (ns=0; ns<sv.length()-1; ns++) {
		N.append(0);
		striplen=N.length()-1;
		for (nt=0; nt<tv.length(); nt++) {
			N.append(nt+tv.length()*ns);
			N.append(nt+ tv.length()*(ns+1));
		}
		N.at(striplen)=tv.length()*2;
	}
	N.append(0);
	
	free(strip);

	strip=(int*)malloc(N.length()*sizeof(int));
	memcpy(strip,N.getData(),N.length()*sizeof(int));


	free(normals);
	normals=(float(*)[3])calloc(total_coords,sizeof(float[3]));

	
	int *ar,totta,k;

	ar=strip;
	while (ar[0]) {
		totta=ar[0]; ar++;
		for (k=2; k<totta; k++) {
			float g1[3],g2[3],g3[3];
			vec_diff(g1,coords[ ar[k-2] ],coords[ ar[k-1] ]);
			vec_diff(g2,coords[ ar[k-1] ],coords[ ar[k] ]);
			vec_cross_product(g3,g1,g2);
			vec_normalize(g3);
			if (k&1) {
				vec_flip(g3,g3);
			}
			vec_sum(normals[ ar[k] ],normals[ ar[k] ],g3);
			if (k==2) {
				vec_sum(normals[ ar[0] ],normals[ ar[0] ],g3);
				vec_sum(normals[ ar[1] ],normals[ ar[1] ],g3);
			}
		}

		ar+=totta;
	}
	for (k=0; k<total_coords; k++) {
		vec_normalize(normals[k]);
	}

}



