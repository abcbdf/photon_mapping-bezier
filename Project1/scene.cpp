#include"scene.h"
#include<string>
#include<fstream>
#include<sstream>
#include<cstdlib>
#include<ctime>

Scene::Scene() {
	obj_head = NULL;
	light_head = NULL;
	background_color = Color();
	camera = new Camera;
}

Scene::~Scene() {
	while ( obj_head != NULL ) {
		obj* next_head = obj_head->GetNext();
		if ( obj_head->GetMaterial()->texture != NULL )
			delete obj_head->GetMaterial()->texture;
		delete obj_head;
		obj_head = next_head;
	}

	while ( light_head != NULL ) {
		Light* next_head = light_head->GetNext();
		delete light_head;
		light_head = next_head;
	}

	delete camera;
}

void Scene::CreateScene( std::string file ) {
	srand( 1995 - 05 - 12 );
	std::ifstream fin( file.c_str() );

	std::string OBJ;
	while ( fin >> OBJ ) {
		obj* new_obj = NULL;
		Light* new_light = NULL;

		if ( OBJ == "obj" ) {
			std::string type; fin >> type;
			if ( type == "sphere" ) new_obj = new Sphere;
			if ( type == "plane" ) new_obj = new Plane;
			if (type == "triangle") new_obj = new Triangle;
			if (type == "bezier") new_obj = new Bezier;
			if (type == "tetra") {
				obj* temp = new Tetra;
				temp->SetNext(obj_head);
				obj_head = temp;
				for (int i = 0; i < 4; i++)
				{
					new_obj = &((Tetra*)temp)->tris[i];
					new_obj->SetNext(obj_head);
					obj_head = new_obj;
				}
			}
			if ( new_obj != NULL && type != "tetra") {
				new_obj->SetNext( obj_head );
				obj_head = new_obj;
			}
		} else
		if ( OBJ == "light" ) {
			std::string type; fin >> type;
			if ( type == "point" ) new_light = new PointLight;
			if ( type == "area" ) new_light = new AreaLight;
			if ( new_light != NULL ) {
				new_light->SetNext( light_head );
				light_head = new_light;
			}
		} else
		if ( OBJ != "background" && OBJ != "camera" ) continue;

		fin.ignore( 1024 , '\n' );
		
		std::string order;
		while ( getline( fin , order , '\n' ) ) {
			std::stringstream fin2( order );
			std::string var; fin2 >> var;
			if ( var == "end" ) break;

			if ( OBJ == "background" && var == "color=" ) background_color.Input( fin2 );
			if ( OBJ == "obj" && new_obj != NULL ) new_obj->Input( var , fin2 );
			if ( OBJ == "light" && new_light != NULL ) new_light->Input( var , fin2 );
			if ( OBJ == "camera" ) camera->Input( var , fin2 );
		}
	}
	camera->Initialize();
}

obj* Scene::FindNearestobj( Vector3 ray_O , Vector3 ray_V ) {
	obj* ret = NULL;

	for ( obj* now = obj_head ; now != NULL ; now = now->GetNext() )
		if ( now->Collide( ray_O , ray_V ) && ( ret == NULL || now->crash.dist < ret->crash.dist ) ) ret = now;

	return ret;
}

Light* Scene::FindNearestLight( Vector3 ray_O , Vector3 ray_V ) {
	Light* ret = NULL;

	for ( Light* now = light_head ; now != NULL ; now = now->GetNext() )
		if ( now->Collide( ray_O , ray_V ) && ( ret == NULL || now->crash_dist < ret->crash_dist ) ) ret = now;

	return ret;
}
