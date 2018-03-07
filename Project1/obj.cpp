#include"obj.h"
#include<sstream>
#include<cstdio>
#include<string>
#include<cmath>
#include<iostream>
#include<cstdlib>
#include<algorithm>
#include<math.h>


Material::Material() {
	color = absor = Color();
	refl = refr = 0;
	diff = spec = 0;
	rindex = 0;
	drefl = 0;
	texture = NULL;
}

void Material::Input( std::string var , std::stringstream& fin ) {
	if ( var == "color=" ) color.Input( fin );
	if ( var == "absor=" ) absor.Input( fin );
	if ( var == "refl=" ) fin >> refl;
	if ( var == "refr=" ) fin >> refr;
	if ( var == "diff=" ) fin >> diff;
	if ( var == "spec=" ) fin >> spec;
	if ( var == "drefl=" ) fin >> drefl;
	if ( var == "rindex=" ) fin >> rindex;
	if ( var == "texture=" ) {
		std::string file; fin >> file;
		texture = new Bmp;
		texture->Input( file );
	}
}

obj::obj() {
	sample = rand();
	material = new Material;
	next = NULL;
}

obj::obj( const obj& obj ) {
	*this = obj;
	material = new Material;
	*material = *obj.material;
}

obj::~obj() {
	delete material;
}

void obj::Input( std::string var , std::stringstream& fin ) {
	material->Input( var , fin );
}

Sphere::Sphere() : obj() {
	De = Vector3( 0 , 0 , 1 );
	Dc = Vector3( 0 , 1 , 0 );
}

void Sphere::Input( std::string var , std::stringstream& fin ) {
	if ( var == "O=" ) O.Input( fin );
	if ( var == "R=" ) fin >> R;
	if ( var == "De=" ) De.Input( fin );
	if ( var == "Dc=" ) Dc.Input( fin );
	obj::Input( var , fin );
}

bool Sphere::Collide( Vector3 ray_O , Vector3 ray_V ) {
	ray_V = ray_V.GetUnitVector();
	Vector3 P = ray_O - O;
	double b = -P.Dot( ray_V );
	double det = b * b - P.Module2() + R * R;

	if ( det > EPS ) {
		det = sqrt( det );
		double x1 = b - det  , x2 = b + det;

		if ( x2 < EPS ) return false;
		if ( x1 > EPS ) {
			crash.dist = x1;
			crash.front = true;
		} else {
			crash.dist = x2;
			crash.front = false;
		} 
	} else 
		return false;

	crash.C = ray_O + ray_V * crash.dist;
	crash.N = ( crash.C - O ).GetUnitVector();
	if ( crash.front == false ) crash.N = -crash.N;
	return true;
}

Color Sphere::GetTexture() {
	Vector3 I = ( crash.C - O ).GetUnitVector();
	double a = acos( -I.Dot( De ) );
	double b = acos( std::min( std::max( I.Dot( Dc ) / sin( a ) , -1.0 ) , 1.0 ) );
	double u = a / PI , v = b / 2 / PI;
	if ( I.Dot( Dc * De ) < 0 ) v = 1 - v;
	return material->texture->GetSmoothColor( u , v );
}

obj* Sphere::objCopy() {
	Sphere* ret = new Sphere( *this );
	return ret;
}

void Plane::Input( std::string var , std::stringstream& fin ) {
	std::cout << "Plane" << std::endl;
	if ( var == "N=" ) N.Input( fin );
	if ( var == "R=" ) fin >> R;
	if ( var == "Dx=" ) Dx.Input( fin );
	if ( var == "Dy=" ) Dy.Input( fin );
	obj::Input( var , fin );
}

bool Plane::Collide( Vector3 ray_O , Vector3 ray_V ) {
	ray_V = ray_V.GetUnitVector();
	N = N.GetUnitVector();
	double d = N.Dot( ray_V );
	if ( fabs( d ) < EPS ) return false;
	double l = ( N * R - ray_O ).Dot( N ) / d;
	if ( l < EPS ) return false;

	crash.dist = l;
	crash.front = ( d < 0 );
	crash.C = ray_O + ray_V * crash.dist;
	crash.N = ( crash.front ) ? N : -N;
	return true;
}

Color Plane::GetTexture() {
	double u = crash.C.Dot( Dx ) / Dx.Module2();
	double v = crash.C.Dot( Dy ) / Dy.Module2();
	return material->texture->GetSmoothColor( u , v );
}

obj* Plane::objCopy() {
	Plane* ret = new Plane( *this );
	return ret;
}

void Triangle::Input(std::string var, std::stringstream& fin) {
	
	if (var == "A0=") vertex[0].Input(fin);
	if (var == "A1=") vertex[1].Input(fin);
	if (var == "A2=") vertex[2].Input(fin);
	if (var == "N=") N.Input(fin);
	obj::Input(var, fin);
	std::cout << "Triangle" << std::endl;
}

bool Triangle::Collide(Vector3 ray_O, Vector3 ray_V) {
	ray_V = ray_V.GetUnitVector();
	N = N.GetUnitVector();
	double d = N.Dot(ray_V);
	if (fabs(d) < EPS)	return false;
	Vector3 E1 = vertex[1] - vertex[0];
	Vector3 E2 = vertex[2] - vertex[0];
	Vector3 T = ray_O - vertex[0];
	double coef = 1 / ((ray_V * E2).Dot(E1));
	double t = coef * (T * E1).Dot(E2);
	double u = coef * (ray_V * E2).Dot(T);
	double v = coef * (T * E1).Dot(ray_V);
	if (t < 0 || u < 0 || v < 0 || u + v > 1)	return false;
	crash.dist = t;
	crash.front = (d < 0);
	crash.C = ray_O + ray_V * t;
	crash.N = (crash.front) ? N : -N;
	//std::cout << "collide";
	return true;
}

Color Triangle::GetTexture() {
	Vector3 Dx = vertex[1] - vertex[0];
	Vector3 Dy = vertex[2] - vertex[0];
	double u = crash.C.Dot(Dx) / Dx.Module2();
	double v = crash.C.Dot(Dy) / Dy.Module2();
	return material->texture->GetSmoothColor(u, v);
}

obj* Triangle::objCopy() {
	Triangle* ret = new Triangle(*this);
	return ret;
}

void Tetra::Input(std::string var, std::stringstream& fin) {
	std::cout << "tetra" << std::endl;
	if (var == "V0=") vertex[0].Input(fin);
	if (var == "V1=") vertex[1].Input(fin);
	if (var == "V2=") vertex[2].Input(fin);
	if (var == "V3=") vertex[3].Input(fin);
	obj::Input(var, fin);
	Vector3 N0 = (vertex[1] - vertex[0]) * (vertex[2] - vertex[0]);
	Vector3 N1 = (vertex[2] - vertex[1]) * (vertex[3] - vertex[1]);
	Vector3 N2 = (vertex[3] - vertex[2]) * (vertex[0] - vertex[2]);
	Vector3 N3 = (vertex[0] - vertex[3]) * (vertex[1] - vertex[3]);
	tris[0].setup(vertex[0], vertex[1], vertex[2], (N0.Dot(vertex[3] - vertex[0])) ? -N0 : N0);
	tris[1].setup(vertex[1], vertex[2], vertex[3], (N1.Dot(vertex[0] - vertex[1])) ? -N1 : N1);
	tris[2].setup(vertex[2], vertex[3], vertex[0], (N2.Dot(vertex[1] - vertex[2])) ? -N2 : N2);
	tris[3].setup(vertex[3], vertex[0], vertex[1], (N3.Dot(vertex[2] - vertex[3])) ? -N3 : N3);
}

bool Tetra::Collide(Vector3 ray_O, Vector3 ray_V) {
	//for (int i = 0; i < 4; i++)
	//{
	//	if (tris[i]->Collide(ray_O, ray_V))
	//	{
	//		crash.dist = tris[i]->crash.dist;
	//		crash.front = tris[i]->crash.front;
	//		crash.C = tris[i]->crash.C;
	//		crash.N = tris[i]->crash.N;
	//		return true;
	//	}
	//}
	return false;
}
Color Tetra::GetTexture() {
	return material->texture->GetSmoothColor(0.5, 0.5);
}
obj* Tetra::objCopy() {
	Tetra* ret = new Tetra(*this);
	return ret;
}

void Bezier::Input(std::string var, std::stringstream& fin)
{
	if (var == "center=") center.Input(fin);
	if (var == "V0=")
	{
		Vector3 p;
		p.Input(fin);
		set_point.push_back(p);
	}
	if (var == "V1=") 
	{
		Vector3 p;
		p.Input(fin);
		set_point.push_back(p);
	}
	if (var == "V2=")
	{
		Vector3 p;
		p.Input(fin);
		set_point.push_back(p);
	}
	if (var == "V3=")
	{
		Vector3 p;
		p.Input(fin);
		set_point.push_back(p);
		build_bezier();
	}
	obj::Input(var, fin);
}

void Bezier::build_bezier()
{
	for (int i = 0; i <= lsize; i++)
	{		
		double t = (double)i / lsize;
		points[0].push_back(line(t));
	}
	for (int i = 1; i < bsize; i++)
	{
		std::vector<Vector3> pV;
		points.push_back(pV);
		for (int j = 0; j < points[0].size(); j++)
		{
			const double r = std::fabs(points[0][j].x - center.x);
			double dx = r * cos(i * 2 * PI / bsize);
			double dy = r * sin(i * 2 * PI / bsize);
			Vector3 p(center.x + dx, center.y + dy, points[0][j].z);
			points[i].push_back(p);
		}
	}
	for (int i = 0; i < bsize; i++)
	{
		for (int j = 0; j < lsize; j++)
		{
			if (points[i][j].x < box_max.lower[0])	box_max.lower[0] = points[i][j].x;
			if (points[i][j].x > box_max.upper[0])	box_max.upper[0] = points[i][j].x;
			if (points[i][j].y < box_max.lower[1])	box_max.lower[1] = points[i][j].y;
			if (points[i][j].y > box_max.upper[1])	box_max.upper[1] = points[i][j].y;
			if (points[i][j].z < box_max.lower[2])	box_max.lower[2] = points[i][j].z;
			if (points[i][j].z > box_max.upper[2])	box_max.upper[2] = points[i][j].z;
		}
	}
}
bool Bezier::Collide(Vector3 ray_O, Vector3 ray_V)
{
	//std::cout << "collide" << std::endl;
	//if (GetMaterial()->texture != NULL) std::cout << "yes" << std::endl;
	ray_V = ray_V.GetUnitVector();
	const int group = 20;
	double tmin, tmax, t_array[group], u[group], v[group];
	Vector3 crash_N[group];
	bool is_true[group];
	int number = 0;
	if (box_max.Collide(ray_O, ray_V, tmin, tmax))
	{
		srand(time(NULL));
		for (int i = 0; i < group; i++)
		{
			
			u[i] = 2 * PI * i / group;
			v[i] = (double)(rand() % 10000) / 10000;
			//std::cout << "u:" << u << std::endl;
			//std::cout << "v:" << v << std::endl;
			t_array[i] = (tmax - tmin) * (double)(rand() % 10000) / 10000 + tmin;
			double df;
			for (int iter = 0; iter < 10; iter++)
			{
				Iter(t_array[i], u[i], v[i], ray_O, ray_V, crash_N[i], df);
				if (df < 1e-5 || df > 100)
				{
					//std::cout << "yes" << std::endl;
					break;
				}
			}
			if (df < 1e-3 && v[i] > 0 && v[i] < 1)
			{
				is_true[i] = true;
				number++;
			}
			else
			{
				is_true[i] = false;
			}
		}
		//std::cout << number << std::endl;
		double t_min = 1e10;
		for (int i = 0; i < group; i++)
		{
			if (is_true[i] && t_array[i] < t_min && t_array[i] > EPS)
			{
				t_min = t_array[i];
				//std::cout << t_min << std::endl;
				crash_u = u[i];
				crash_v = v[i];
				crash.C = ray_O + t_min * ray_V;
				crash.N = (ray_V.Dot(crash_N[i]) > 0) ? -crash_N[i] : crash_N[i];
				crash.dist = t_min;
				crash.front = true;
			}
		}
		if (t_min < 1e10)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	
}
bool Bezier::Iter(double &t, double &u, double &v, Vector3 ray_O, Vector3 ray_V, Vector3 &N, double &_df)
{
	double dt, du, dv;
	Vector3 temp_v = line(v);
	double dx = (temp_v.x - center.x) * cos(u);
	double dy = (temp_v.x - center.x) * sin(u);
	Vector3 s = Vector3(center.x + dx, center.y + dy, temp_v.z);
	Vector3 df = (s - (ray_O + ray_V * t));
	Vector3 dsdu = Vector3(-dy, dx, 0);
	Vector3 dcdt = ray_V;
	Vector3 temp_v_2 = dline(v);
	double dx_2 = temp_v_2.x * cos(u);
	double dy_2 = temp_v_2.x * sin(u);
	Vector3 dsdv = Vector3(dx_2, dy_2, temp_v_2.z);
	double D = dcdt.Dot(dsdu * dsdv);
	dt = dsdu.Dot(dsdv * df) / D;
	du = dcdt.Dot(dsdv * df) / D;
	dv = dcdt.Dot(dsdu * df) / D;
	t += dt;
	u += du;
	v -= dv;
	//std::cout << "t" << dt << std::endl;
	//std::cout << "u" << du << std::endl;
	//std::cout << "v" << dv << std::endl;
	//std::cout << "df" << df.Module() << std::endl;
	N = dsdu * dsdv;
	_df = df.Module();
	return true;
}
Vector3 Bezier::line(double t)
{
	double coef[4];
	coef[0] = std::pow(1 - t, 3);
	coef[1] = 3 * t * std::pow(1 - t, 2);
	coef[2] = 3 * std::pow(t, 2) * (1 - t);
	coef[3] = std::pow(t, 3);
	return (set_point[0] * coef[0]) + (set_point[1] * coef[1]) + (set_point[2] * coef[2]) + (set_point[3] * coef[3]);
}
Vector3 Bezier::dline(double t)
{
	double coef[4];
	coef[0] = - 3 * std::pow(1 - t, 2);
	coef[1] = 3 * std::pow(1 - t, 2) - 6 * t * (1 - t);
	coef[2] = 6 * t * (1 - t) - 3 * t * t;
	coef[3] = 3 * t * t;
	return (set_point[0] * coef[0]) + (set_point[1] * coef[1]) + (set_point[2] * coef[2]) + (set_point[3] * coef[3]);
}
Color Bezier::GetTexture()
{
	//std::cout << (material->texture->GetSmoothColor(0, 0)).r << std::endl;
	//std::cout << (material->texture->GetSmoothColor(0, 0)).g << std::endl;
	//std::cout << (material->texture->GetSmoothColor(0, 0)).b << std::endl;

	return material->texture->GetSmoothColor(-crash_v , crash_u / (2 * PI) + 0.75);
}
obj* Bezier::objCopy() {
	Bezier* ret = new Bezier(*this);
	return ret;
}

Bezier::~Bezier()
{
	;
}