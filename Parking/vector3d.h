#ifndef VECTOR3D_H
#define VECTOR3D_H

template <typename T> 
class vector3d {
public:
	T data[3];

	void diff(const T start[3],const T stop[3]) {data[0]=stop[0]-start[0]; data[1]=stop[1]-start[1]; data[2]=stop[2]-start[2];}

	void zero() {data[0]=0; data[1]=0; data[2]=0;}
	void scale(const T r) {data[0]*=r; data[1]*=r; data[2]*=r;}

	void add(const vector3d &a) {data[0]+=a.data[0]; data[1]+=a.data[1]; data[2]+=a.data[2];}
	void copy(const vector3d &a) {data[0]=a.data[0]; data[1]=a.data[1]; data[2]=a.data[2];}

	void cross_product(const vector3d &a,const vector3d &b) {
		data[0]=a.data[1]*b.data[2]-b.data[1]*a.data[2];
		data[1]=a.data[2]*b.data[0]-b.data[2]*a.data[0];
		data[2]=a.data[0]*b.data[1]-b.data[0]*a.data[1];
	}
	T dot_product(const vector3d &a) {
		return data[0]*a.data[0]+data[1]*a.data[1]+data[2]*a.data[2];
	}

};

template <typename T>
inline void vec_zero(T v[3]) {v[0]=v[1]=v[2]=0;}

template <typename T>
inline void vec_copy(T v[3],const T a[3]) {v[0]=a[0]; v[1]=a[1]; v[2]=a[2];}

template <typename T>
inline void vec_cross_product(T v[3],const T a[3],const T b[3]) {
	v[0]=a[1]*b[2]-b[1]*a[2];
	v[1]=a[2]*b[0]-b[2]*a[0];
	v[2]=a[0]*b[1]-b[0]*a[1];
}

template <typename T>
inline void vec_dot_product(T *v,const T a[3],const T b[3])
{
	(*v)=a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
}

template <typename T>
inline void vec_normalize(T v[3])
{
	T l;
	vec_dot_product(&l,v,v);
	if (l==0 || l==1) return;
	l=1./l;
	v[0]=v[0]*l;
	v[1]=v[1]*l;
	v[2]=v[2]*l;
}

#endif /* VECTOR3D_H */
