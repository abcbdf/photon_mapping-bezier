#include"raytracer.h"
#include<cstdlib>
#include<iostream>
#define ran() ( double( rand() % 32768 ) / 32768 )

const double SPEC_POWER = 20;
const int MAX_DREFL_DEP = 2;
const int MAX_RAYTRACING_DEP = 10;
const int HASH_FAC = 7;
const int HASH_MOD = 10000007;

Color Raytracer::CalnDiffusion( obj* pri , int* hash ) {
	Color color = pri->GetMaterial()->color;
	if ( pri->GetMaterial()->texture != NULL ) color = color * pri->GetTexture();
	Color ret = color * scene.GetBackgroundColor() * pri->GetMaterial()->diff;

	for ( Light* light = scene.GetLightHead() ; light != NULL ; light = light->GetNext() ) {
		double shade = light->CalnShade( pri->crash.C , scene.GetobjHead() , scene.GetCamera()->GetShadeQuality() );
		//std::cout << "shade:" << shade << std::endl;
		if ( shade < EPS ) continue;
		
		Vector3 R = ( light->GetO() - pri->crash.C ).GetUnitVector();
		double dot = R.Dot( pri->crash.N );
		if ( dot > EPS ) {
			if ( hash != NULL && light->IsPointLight() ) *hash = ( *hash + light->GetSample() ) % HASH_MOD;

			if ( pri->GetMaterial()->diff > EPS ) {
				//std::cout << "diff" << std::endl;
				double diff = pri->GetMaterial()->diff * dot * shade;
				ret += color * light->GetColor() * diff;
			}
			if ( pri->GetMaterial()->spec > EPS ) {
				double spec = pri->GetMaterial()->spec * pow( dot , SPEC_POWER ) * shade;
				ret += color * light->GetColor() * spec;
			}
		}
	}
	return ret;
}

Color Raytracer::CalnReflection( obj* pri , Vector3 ray_V , int dep , int* hash ) {
	ray_V = ray_V.Reflect( pri->crash.N );

	if ( pri->GetMaterial()->drefl < EPS || dep > MAX_DREFL_DEP )
		return RayTracing( pri->crash.C , ray_V , dep + 1 , hash ) * pri->GetMaterial()->color * pri->GetMaterial()->refl;

	Vector3 Dx = ray_V * Vector3( 1 , 0 , 0 );
	if ( Dx.IsZeroVector() ) Dx = Vector3( 1 , 0 , 0 );
	Vector3 Dy = ray_V * Dx;
	Dx = Dx.GetUnitVector() * pri->GetMaterial()->drefl;
	Dy = Dy.GetUnitVector() * pri->GetMaterial()->drefl;

	Color ret;
	for ( int k = 0 ; k < 16 * scene.GetCamera()->GetDreflQuality() ; k++ ) {
		double x , y;
		do {
			x = ran() * 2 - 1;
			y = ran() * 2 - 1;
		} while ( x * x + y * y > 1 );
		x *= pri->GetMaterial()->drefl;
		y *= pri->GetMaterial()->drefl;

		ret += RayTracing( pri->crash.C , ray_V + Dx * x + Dy * y , dep + MAX_DREFL_DEP , NULL );
	}

	ret = ret * pri->GetMaterial()->color * pri->GetMaterial()->refl / ( 16 * scene.GetCamera()->GetDreflQuality() );
	return ret;
}

Color Raytracer::CalnRefraction( obj* pri , Vector3 ray_V , int dep , int* hash ) {
	double n = pri->GetMaterial()->rindex;
	if ( pri->crash.front ) n = 1 / n;
	
	ray_V = ray_V.Refract( pri->crash.N , n );
	
	Color rcol = RayTracing( pri->crash.C , ray_V , dep + 1 , hash );
	if ( pri->crash.front ) return rcol * pri->GetMaterial()->refr;
	Color absor = pri->GetMaterial()->absor * -pri->crash.dist;
	Color trans = Color( exp( absor.r ) , exp( absor.g ) , exp( absor.b ) );
	return rcol * trans * pri->GetMaterial()->refr;
}

Color Raytracer::RayTracing( Vector3 ray_O , Vector3 ray_V , int dep , int* hash ) {
	if ( dep > MAX_RAYTRACING_DEP ) return Color();

	Color ret;
	obj* nearest_obj = scene.FindNearestobj( ray_O , ray_V );
	Light* nearest_light = scene.FindNearestLight( ray_O , ray_V );

	if ( nearest_light != NULL && ( nearest_obj == NULL || nearest_light->crash_dist < nearest_obj->crash.dist ) ) {
		if ( hash != NULL ) *hash = ( *hash + nearest_light->GetSample() ) % HASH_MOD;
		ret += nearest_light->GetColor();
	}
	
	else if ( nearest_obj != NULL ) {
		if ( hash != NULL ) *hash = ( *hash + nearest_obj->GetSample() ) % HASH_MOD;
		obj* obj = nearest_obj->objCopy();
		if ( obj->GetMaterial()->diff > EPS || obj->GetMaterial()->spec > EPS ) ret += CalnDiffusion( obj , hash );
		if ( obj->GetMaterial()->refl > EPS ) ret += CalnReflection( obj , ray_V , dep , hash );
		if ( obj->GetMaterial()->refr > EPS ) ret += CalnRefraction( obj , ray_V , dep , hash );
		delete obj;
	}

	if ( hash != NULL ) *hash = ( *hash * HASH_FAC ) % HASH_MOD;
	ret.Confine();
	return ret;
}

void Raytracer::Run() {
	Camera* camera = scene.GetCamera();
	scene.CreateScene( input );

	Vector3 ray_O = camera->GetO();
	int H = camera->GetH() , W = camera->GetW();
	int** sample = new int*[H];
	for ( int i = 0 ; i < H ; i++ ) {
		sample[i] = new int[W];
		for ( int j = 0 ; j < W ; j++ )
			sample[i][j] = 0;
	}

	for ( int i = 0 ; i < H ; std::cout << "origin:   " << ++i << "/" << H << std::endl )
		for ( int j = 0 ; j < W ; j++ ) {
			if (true)
			{
				Vector3 ray_V = camera->Emit(i, j);
				Color color = RayTracing(ray_O, ray_V, 1, &sample[i][j]);
				camera->SetColor(i, j, color);
			}
			
		}

	for ( int i = 0 ; i < H ; std::cout << "again: " << ++i << "/" << H << std::endl )
		for ( int j = 0 ; j < W ; j++ ) {
			if ( ( i == 0 || sample[i][j] == sample[i - 1][j] ) && ( i == H - 1 || sample[i][j] == sample[i + 1][j] ) &&
			     ( j == 0 || sample[i][j] == sample[i][j - 1] ) && ( j == W - 1 || sample[i][j] == sample[i][j + 1] ) ) continue;

			Color color;
			for ( int r = -1 ; r <= 1 ; r++ )
				for ( int c = -1 ; c <= 1 ; c++ ) {
					Vector3 ray_V = camera->Emit( i + ( double ) r / 3 , j + ( double ) c / 3 );
					color += RayTracing( ray_O , ray_V , 1 , NULL ) / 9;
				}
			camera->SetColor( i , j , color );
		}
	
	for ( int i = 0 ; i < H ; i++ )
		delete[] sample[i];
	delete[] sample;

	Bmp* bmp = new Bmp( H , W );
	camera->Output( bmp );
	bmp->Output( output );
	delete bmp;
}
