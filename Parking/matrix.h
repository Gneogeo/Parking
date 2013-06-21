#ifndef MATRIX_H
#define MATRIX_H

#include <cmath>


template <typename T>
inline void multiply_222(T A[2][2],const T B[2][2],const T C[2][2])
{
// A = B*C

	A[0][0] = B[0][0]*C[0][0]+B[0][1]*C[1][0]; 
	A[0][1] = B[0][0]*C[0][1]+B[0][1]*C[1][1];
	A[1][0] = B[1][0]*C[0][0]+B[1][1]*C[1][0];
	A[1][1] = B[1][0]*C[0][1]+B[1][1]*C[1][1];
}

template <typename T>
inline void multiply_221(T A[2],const T B[2][2],const T C[2])
{
// A = B*C
	A[0] = B[0][0]*C[0] + B[0][1]*C[1];
	A[1] = B[1][0]*C[0] + B[1][1]*C[1];
}

template <typename T>
inline void multiply_122(T A[2],const T B[2], const T C[2][2])
{
// A = B*C
	A[0] = B[0]*C[0][0]+B[1]*C[1][0];
	A[1] = B[0]*C[0][1]+B[1]*C[1][1];
}



/*For symmetrical 2x2 matrix*/
template <typename T>
inline void eigen_symmetrical(const T A[2][2],T P[2][2],T L[2][2])
{
	T a,b,c;
	a = A[0][0];
	b = 2.*A[0][1];
	c = A[1][1];

	T det2 , det;
	det2 = (a-c)*(a-c)+b*b;
	det = sqrt(det2);
	
	L[0][0]=(a+c+det)*0.5;
	L[0][1]=0;
	L[1][0]=0;
	L[1][1]=(a+c-det)*0.5;

	T f,sinf,cosf;
	
	f = atan2(a-c-det,b);
	sinf = sin(f);
	cosf = cos(f);

	P[0][0] =  cosf; P[0][1] = sinf;
	P[1][0] = -sinf; P[1][1] = cosf; 


}

#endif /* MATRIX_H */

