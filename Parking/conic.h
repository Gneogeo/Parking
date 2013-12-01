#ifndef CONIC_H
#define CONIC_H

#include "coord_system.h"

class ArcEllipse {
        public:
		ArcEllipse();
		ArcEllipse &operator= (const ArcEllipse &);
		~ArcEllipse();

        CoordinateSystem<float> XYZ;
        float xradius;
        float yradius;
        float fmin;
        float fmax;

	int total_coords;
	float (*coords)[3];

	int *strip;

	int getParamPoint(float t,float outp[3]) const;

	void recalcCoords(float dt);


};

class ArcHyperbola {
	public:
		ArcHyperbola();
		ArcHyperbola &operator= (const ArcHyperbola &);
		~ArcHyperbola();

		CoordinateSystem<float> XYZ;
		float xradius;
		float yradius;
		float fmin;
		float fmax;

		int total_coords;
		float (*coords)[3];

		int *strip;

		int getParamPoint(float t,float outp[3]) const;

		void recalcCoords(float dt);

};

#endif /* CONIC_H */
