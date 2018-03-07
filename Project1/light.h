#ifndef LIGHT_H
#define LIGHT_H

#include"vector3.h"
#include"color.h"
#include"obj.h"
#include<sstream>
#include<string>
#include<cmath>

extern const double EPS;

class Light {
protected:
	int sample;
	Color color;
	Light* next;

public:
	double crash_dist;

	Light();
	~Light() {}
	
	virtual bool IsPointLight() = 0;
	int GetSample() { return sample; }
	Color GetColor() { return color; }
	Light* GetNext() { return next; }
	void SetNext( Light* light ) { next = light; }

	virtual void Input( std::string , std::stringstream& );
	virtual Vector3 GetO() = 0;
	virtual bool Collide( Vector3 ray_O , Vector3 ray_V ) = 0;
	virtual double CalnShade( Vector3 C , obj* obj_head , int shade_quality ) = 0;
};

class PointLight : public Light {
	Vector3 O;
public:
	PointLight() : Light() {}
	~PointLight() {}
	
	bool IsPointLight() { return true; }
	Vector3 GetO() { return O; }
	void Input( std::string , std::stringstream& );
	bool Collide( Vector3 ray_O , Vector3 ray_V );
	double CalnShade( Vector3 C , obj* obj_head , int shade_quality );
};

class AreaLight : public Light {
	Vector3 O , Dx , Dy;
public:
	AreaLight() : Light() {}
	~AreaLight() {}
	
	bool IsPointLight() { return false; }
	Vector3 GetO() { return O; }
	void Input( std::string , std::stringstream& );
	bool Collide( Vector3 ray_O , Vector3 ray_V );
	double CalnShade( Vector3 C , obj* obj_head , int shade_quality );
};

#endif
