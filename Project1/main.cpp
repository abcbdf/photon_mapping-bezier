#include"raytracer.h"
int main() {
	Raytracer* raytracer = new Raytracer;
	raytracer->SetInput("scene_1.txt");
	raytracer->SetOutput( "picture.bmp" );
	raytracer->Run();
	system("pause");
	return 0;
}
