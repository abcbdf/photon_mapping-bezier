#include"light.h"
#include<sstream>
#include<string>
#include<cmath>
#include<cstdlib>
#define ran() ( double( rand() % 32768 ) / 32768 )

Light::Light() {
	sample = rand();
	next = NULL;
}

void Light::Input( std::string var , std::stringstream& fin ) {
	if ( var == "color=" ) color.Input( fin );
}

void PointLight::Input( std::string var , std::stringstream& fin ) {
	if ( var == "O=" ) O.Input( fin );
	Light::Input( var , fin );
}

bool PointLight::Collide( Vector3 ray_O , Vector3 ray_V ) {
	return false;
}

double PointLight::CalnShade( Vector3 C , obj* obj_head , int shade_quality ) {
	Vector3 V = O - C;
	double dist = V.Module();

	for ( obj* now = obj_head ; now != NULL ; now = now->GetNext() )
		if ( now->Collide( C , V ) && ( now->crash.dist < dist ) ) return 0;

	return 1;
}

void AreaLight::Input( std::string var , std::stringstream& fin ) {
	if ( var == "O=" ) O.Input( fin );
	if ( var == "Dx=" ) Dx.Input( fin );
	if ( var == "Dy=" ) Dy.Input( fin );
	Light::Input( var , fin );
}

bool AreaLight::Collide( Vector3 ray_O , Vector3 ray_V ) {
	ray_V = ray_V.GetUnitVector();
	Vector3 N = ( Dx * Dy ).GetUnitVector();
	double d = N.Dot( ray_V );
	if ( fabs( d ) < EPS ) return false;
	double l = ( N * O.Dot( N ) - ray_O ).Dot( N ) / d;
	if ( l < EPS ) return false;

	Vector3 C = ( ray_O + ray_V * l ) - O;
	if ( fabs( Dx.Dot( C ) ) > Dx.Dot( Dx ) ) return false;
	if ( fabs( Dy.Dot( C ) ) > Dy.Dot( Dy ) ) return false;

	crash_dist = l;
	return true;
}

double AreaLight::CalnShade( Vector3 C , obj* obj_head , int shade_quality ) {
	int shade = 0;
	
	for ( int i = -2 ; i < 2 ; i++ )
		for ( int j = -2 ; j < 2 ; j++ )
			for ( int k = 0 ; k < shade_quality ; k++ ) {
				Vector3 V = O - C + Dx * ( ( ran() + i ) / 2 ) + Dy * ( ( ran() + j ) / 2 );
				double dist = V.Module();

				for ( obj* now = obj_head ; now != NULL ; now = now->GetNext() )
					if ( now->Collide( C , V ) && ( now->crash.dist < dist ) ) {
						shade++;
						break;
					}
			}
	
	return 1 - ( double ) shade / ( 16.0 * shade_quality );
}
