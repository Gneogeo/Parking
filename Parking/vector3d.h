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
