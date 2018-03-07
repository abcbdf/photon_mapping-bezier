#include "box.h"

bool box::Collide(Vector3 ray_O, Vector3 ray_V, double &tmin, double &tmax)
{
	double t0 = (upper[0] - ray_O.x) / ray_V.x;
	double t1 = (lower[0] - ray_O.x) / ray_V.x;
	double t2 = (upper[1] - ray_O.y) / ray_V.y;
	double t3 = (lower[1] - ray_O.y) / ray_V.y;
	double t4 = (upper[2] - ray_O.z) / ray_V.z;
	double t5 = (lower[2] - ray_O.z) / ray_V.z;
	tmin = std::max(std::min(t0, t1), std::max(std::min(t2, t3), std::min(t4, t5)));
	tmax = std::min(std::max(t0, t1), std::min(std::max(t2, t3), std::max(t4, t5)));
	if (tmin < tmax)
	{
		return true;
	}
	else
	{
		return false;
	}
}