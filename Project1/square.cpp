#include "square.h"
#include <iostream>
square::square()
{
	;
}
bool square::Collide(Vector3 ray_O, Vector3 ray_V, double &_t, Vector3 &crash_N)
{
	ray_V = ray_V.GetUnitVector();
	Vector3 E1 = vertex[1] - vertex[0];
	Vector3 E2 = vertex[3] - vertex[0];
	Vector3 T1 = ray_O - vertex[0];
	Vector3 E3 = vertex[1] - vertex[2];
	Vector3 E4 = vertex[3] - vertex[2];
	Vector3 T2 = ray_O - vertex[2];
	Vector3 N = E1 * E2;
	N = N.GetUnitVector();
	double d = N.Dot(ray_V);
	if (fabs(d) < EPS)	return false;
	double coef1 = 1 / ((ray_V * E2).Dot(E1));
	double coef2 = 1 / ((ray_V * E4).Dot(E3));
	double t = coef1 * (T1 * E1).Dot(E2);
	double u1 = coef1 * (ray_V * E2).Dot(T1);
	double v1 = coef1 * (T1 * E1).Dot(ray_V);
	double u2 = coef2 * (ray_V * E4).Dot(T2);
	double v2 = coef2 * (T2 * E3).Dot(ray_V);

	if (t >= EPS && u1 >= EPS && v1 >= EPS && u1 + v1 <= 1 - EPS)
	{
		_t = t;
		crash_N = (d < 0) ? N : -N;
		/*std::cout << "_ray_V" << ray_V.z << std::endl;
		std::cout << "_t" << t << std::endl;
		std::cout << "0:" << vertex[0].z << std::endl;
		std::cout << "1:" << vertex[1].z << std::endl;
		std::cout << "p:" << (ray_O + ray_V * t).z << std::endl;*/
		
		return true;
	}
	else if (t >= EPS && u2 >= EPS && v2 >= EPS && u2 + v2 <= 1 - EPS)
	{
		_t = t;
		crash_N = (d < 0) ? N : -N;
		/*std::cout << "_t" << t << std::endl;
		std::cout << "0:" << vertex[0].z << std::endl;
		std::cout << "1:" << vertex[1].z << std::endl;
		std::cout << "p:" << (ray_O + ray_V * t).z << std::endl;*/
		return true;
	}
	else
	{
		return false;
	}
}