#ifndef SQUARE_H
#define SQUARE_H
#include "vector3.h"
extern const double EPS;
class square {
public:
	Vector3 vertex[4];
	bool Collide(Vector3 ray_O, Vector3 ray_V, double &_t, Vector3 &crash_N);
	square();
	~square(){}
};
#endif