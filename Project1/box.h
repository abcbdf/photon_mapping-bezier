#ifndef BOX_H
#define BOX_H
#include "vector3.h"
#include <algorithm>
extern const double EPS;
class box {
public:
	double upper[3], lower[3];
	bool Collide(Vector3 ray_O, Vector3 ray_V, double &tmin, double &tmax);
	box(){
		for (int i = 0; i < 3; i++)
		{
			upper[i] = -(1e10);
			lower[i] = 1e10;
		}
	}
	~box() {}
};

#endif