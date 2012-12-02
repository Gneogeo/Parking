#ifndef COORD_SYSTEM_H
#define COORD_SYSTEM_H


#include "vector3d.h"


template <typename T>
class CoordinateSystem {
	T center[3];
	T X[3];
	T Y[3];
	T Z[3];
	public:
	CoordinateSystem() {
		vec_zero(center);
		vec_zero(X); X[0]=1;
		vec_zero(Y); Y[1]=1;
		vec_zero(Z); Z[2]=1;
	}

	void setGlobal() {
		vec_zero(center);
		vec_zero(X); X[0]=1;
		vec_zero(Y); Y[1]=1;
		vec_zero(Z); Z[2]=1;
	}

	void setCenter(const T c[3]) {
		vec_copy(center,c);
	}

	void setAxis(const T x[3],const T y[3],const T z[3]) {
		vec_copy(X,x);
		vec_copy(Y,y);
		vec_copy(Z,z);
		vec_normalize(X);
		vec_normalize(Y);
		vec_normalize(Z);
	}
	void fromLocalToGlobal(T out[3],const T inp[3]) const {
		out[0]=X[0]*inp[0]+Y[0]*inp[1]+Z[0]*inp[2]+center[0];
		out[1]=X[1]*inp[0]+Y[1]*inp[1]+Z[1]*inp[2]+center[1];
		out[2]=X[2]*inp[0]+Y[2]*inp[1]+Z[2]*inp[2]+center[2];
	}
	
};


#endif /* COORD_SYSTEM_H */
