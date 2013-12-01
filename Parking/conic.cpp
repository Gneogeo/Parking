#include"conic.h"

#include "myvector.h"
#include "vector3d.h"

#include <cstdlib>

ArcEllipse::ArcEllipse()
{
	xradius = 0;
	yradius = 0;
	fmin = 0;
	fmax = 0;

	total_coords = 0;
	coords = 0;

	strip = 0;
}

ArcEllipse & ArcEllipse::operator = (const ArcEllipse &r)
{
        xradius=r.xradius;
        yradius=r.yradius;
	fmin=r.fmin;
	fmax=r.fmax;

        total_coords=0;
        coords=0;
        strip=0;

	XYZ = r.XYZ;

        return *this;

}

ArcEllipse::~ArcEllipse()
{
        free(coords);
        free(strip);
}

int ArcEllipse::getParamPoint(float t,float outp[3]) const
{
	float tmp[3];
	tmp[0] = xradius *cos(t);
	tmp[1] = yradius *sin(t);
	tmp[2] = 0;
	XYZ.fromLocalToGlobal(outp,tmp);
	return 1;
}


void ArcEllipse::recalcCoords(float dt)
{
	int j;
	float t;
	vector3d<float> X;

	myVector< vector3d<float> > F;

	for (t=fmin; t<fmax; t+=dt) {
		if (getParamPoint(t,X.data)) {
			F.append(X);
		}
	}
	t=fmax;
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



ArcHyperbola::ArcHyperbola()
{
	xradius = 0;
	yradius = 0;
	fmin = 0;
	fmax = 0;

	total_coords = 0;
	coords = 0;

	strip = 0;
}

ArcHyperbola & ArcHyperbola::operator = (const ArcHyperbola &r)
{
        xradius=r.xradius;
        yradius=r.yradius;
	fmin=r.fmin;
	fmax=r.fmax;

        total_coords=0;
        coords=0;
        strip=0;

	XYZ = r.XYZ;

        return *this;

}

ArcHyperbola::~ArcHyperbola()
{
        free(coords);
        free(strip);
}

int ArcHyperbola::getParamPoint(float t,float outp[3]) const
{
	float tmp[3];
	tmp[0] = xradius /cos(t);
	tmp[1] = yradius *tan(t);
	tmp[2] = 0;
	XYZ.fromLocalToGlobal(outp,tmp);
	return 1;
}


void ArcHyperbola::recalcCoords(float dt)
{
	int j;
	float t;
	vector3d<float> X;

	myVector< vector3d<float> > F;

	for (t=fmin; t<fmax; t+=dt) {
		if (getParamPoint(t,X.data)) {
			F.append(X);
		}
	}
	t=fmax;
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
