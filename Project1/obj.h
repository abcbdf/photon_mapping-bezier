#ifndef obj_H
#define obj_H

#include"color.h"
#include"vector3.h"
#include"IO.h"
#include<vector>
#include<iostream>
#include<sstream>
#include<string>
#include"box.h"
#include<stdio.h>
#include<stdlib.h>
#include<ctime>

extern const double EPS;
extern const double PI;

class Material {
public:
	Color color , absor;
	double refl , refr;
	double diff , spec;
	double rindex;
	double drefl;
	Bmp* texture;

	Material();
	~Material() {}

	void Input( std::string , std::stringstream& );
};

struct Crash {
	Vector3 N , C;
	double dist;
	bool front;
};

class obj {
protected:
	int sample;
	Material* material;
	obj* next;

public:
	Crash crash;

	obj();
	obj( const obj& );
	~obj();
	
	int GetSample() { return sample; }
	Material* GetMaterial() { return material; }
	obj* GetNext() { return next; }
	void SetNext( obj* obj ) { next = obj; }

	virtual void Input( std::string , std::stringstream& );
	virtual bool Collide( Vector3 ray_O , Vector3 ray_V ) = 0;
	virtual Color GetTexture() = 0;
	virtual obj* objCopy() = 0;
};

class Sphere : public obj {
	Vector3 O , De , Dc;
	double R;

public:
	Sphere();
	~Sphere() {}

	void Input( std::string , std::stringstream& );
	bool Collide( Vector3 ray_O , Vector3 ray_V );
	Color GetTexture();
	obj* objCopy();
};

class Plane : public obj {
	Vector3 N , Dx , Dy;
	double R;

public:
	Plane() : obj() {}
	~Plane() {}

	void Input( std::string , std::stringstream& );
	bool Collide( Vector3 ray_O , Vector3 ray_V );
	Color GetTexture();
	obj* objCopy();
};

class Triangle : public obj {
	Vector3 vertex[3];
	Vector3 N;
public:
	Triangle() : obj() {}
	~Triangle(){}
	void setup(Vector3 A0, Vector3 A1, Vector3 A2, Vector3 _N) { vertex[0] = A0; vertex[1] = A1; vertex[2] = A2; N = _N; }
	void Input(std::string, std::stringstream&);
	bool Collide(Vector3 ray_O, Vector3 ray_V);
	Color GetTexture();
	obj* objCopy();
};

class Tetra : public obj {
	Vector3 vertex[4];
	
public:
	Triangle tris[4];
	Tetra() : obj() {};
	~Tetra() {};
	void Input(std::string, std::stringstream&);
	bool Collide(Vector3 ray_0, Vector3 ray_V);
	Color GetTexture();
	obj* objCopy();
};

class Bezier : public obj {
public:
	Vector3 center;
	box box_max;
	double crash_u, crash_v;
	std::vector<Vector3> set_point;
	std::vector<std::vector<Vector3>> points;
	const int bsize;
	const int lsize;
	Bezier() : obj(), bsize(20), lsize(20){
		std::vector<Vector3> p;
		points.push_back(p);		
	};
	~Bezier();
	void Input(std::string, std::stringstream&);
	bool Iter(double &t, double &u, double &v, Vector3 ray_O, Vector3 ray_V, Vector3 &N, double &_df);
	bool Collide(Vector3 ray_0, Vector3 ray_V);
	Vector3 line(double t);
	Vector3 dline(double t);
	void build_bezier();
	Color GetTexture();
	obj* objCopy();
};
#endif
