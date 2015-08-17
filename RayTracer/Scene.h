#pragma once
class Scene
{
public:
	Scene() {
		fov = 60;
	};
	//~Scene();

	void scene1(Raytracer raytracer);
	void scene2(Raytracer raytracer);

	double fov;

};

