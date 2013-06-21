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

	CoordinateSystem<T> & operator= (const CoordinateSystem<T> &r) {
		vec_copy(center,r.center);
		vec_copy(X,r.X);
		vec_copy(Y,r.Y);
		vec_copy(Z,r.Z);
		return *this;
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

	void fromLocalToGlobal(CoordinateSystem<T> *crd) const {
		T tmp[3];
		fromLocalToGlobal(tmp,crd->center);
		vec_copy(crd->center,tmp);
		tmp[0]=X[0]*crd->X[0]+Y[0]*crd->X[1]+Z[0]*crd->X[2];
		tmp[1]=X[1]*crd->X[0]+Y[1]*crd->X[1]+Z[1]*crd->X[2];
		tmp[2]=X[2]*crd->X[0]+Y[2]*crd->X[1]+Z[2]*crd->X[2];
		vec_copy(crd->X,tmp);
		tmp[0]=X[0]*crd->Y[0]+Y[0]*crd->Y[1]+Z[0]*crd->Y[2];
		tmp[1]=X[1]*crd->Y[0]+Y[1]*crd->Y[1]+Z[1]*crd->Y[2];
		tmp[2]=X[2]*crd->Y[0]+Y[2]*crd->Y[1]+Z[2]*crd->Y[2];
		vec_copy(crd->Y,tmp);
		tmp[0]=X[0]*crd->Z[0]+Y[0]*crd->Z[1]+Z[0]*crd->Z[2];
		tmp[1]=X[1]*crd->Z[0]+Y[1]*crd->Z[1]+Z[1]*crd->Z[2];
		tmp[2]=X[2]*crd->Z[0]+Y[2]*crd->Z[1]+Z[2]*crd->Z[2];
		vec_copy(crd->Z,tmp);
	}

	void fromGlobalToLocal(T out[3],const T inp[3]) const {
		out[0] = X[0]*(inp[0]-center[0]) + X[1]*(inp[1]-center[1]) + X[2]*(inp[2]-center[2]);
		out[1] = Y[0]*(inp[0]-center[0]) + Y[1]*(inp[1]-center[1]) + Y[2]*(inp[2]-center[2]);
		out[2] = Z[0]*(inp[0]-center[0]) + Z[1]*(inp[1]-center[1]) + Z[2]*(inp[2]-center[2]);
	}
	
};


#endif /* COORD_SYSTEM_H */
