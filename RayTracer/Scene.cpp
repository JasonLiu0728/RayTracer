#include "stdafx.h"
#include "Scene.h"

void Scene::scene1(Raytracer raytracer)
{
	// Camera parameters
	Point3D eye(0, 0, 1);
	Vector3D view(0, 0, -1);
	Vector3D up(0, 1, 0);

	// Defines a material for shading.
	Material gold(Colour(0.3, 0.3, 0.3), Colour(0.75164, 0.60648, 0.22648),
		Colour(0.628281, 0.555802, 0.366065),
		51.2);
	Material jade(Colour(0, 0, 0), Colour(0.54, 0.89, 0.63),
		Colour(0.316228, 0.316228, 0.316228),
		12.8);
	Material copper(Colour(0.3, 0.3, 0.3), Colour(0.75164, 0.30648, 0.12648),
		Colour(0.628281, 0.555802, 0.366065),
		51.2);
	Material glass(Colour(0.3, 0.3, 0.3), Colour(0.8, 0.8, 0.8),
		Colour(0.628281, 0.555802, 0.366065),
		51.2);

	// Initialize OpenCL
	//raytracer.initOpenCL();

	/****************** An area light source. ************************/
	raytracer.addAreaLightSource(3, 2, 2, 8, Point3D(0, 0, 5), Colour(0.01, 0.01, 0.01));

	// Add a unit square into the scene with material mat.
	//	SceneDagNode* sphere = raytracer.addObject( new UnitSphere(), &gold );
	SceneDagNode* plane = raytracer.addObject(new UnitSquare(), &jade);
	SceneDagNode* cylinder = raytracer.addObject(new UnitCylinder(), &gold);

	// Apply some transformations to the unit square.
	double factor1[3] = { 1.0, 2.0, 1.0 };
	double factor2[3] = { 6.0, 6.0, 6.0 };
	double factor3[3] = { 1.5, 1.5, 3.0 };

	SceneDagNode* sphere1 = raytracer.addObject(new UnitSphere(), &gold);
	SceneDagNode* sphere2 = raytracer.addObject(new UnitSphere(), &copper);
	SceneDagNode* sphere3 = raytracer.addObject(new UnitSphere(), &glass);
	SceneDagNode* sphere4 = raytracer.addObject(new UnitSphere(), &jade);
	double factor4[3] = { 0.5, 0.5, 0.5 };
	double factor5[3] = { 0.5, 0.5, 0.5 };
	double factor6[3] = { 1.0, 1.0, 4.0 };
	double factor7[3] = { 10.0, 10.0, 10.0 };

	raytracer.translate(sphere4, Vector3D(-2, 0, -6));
	raytracer.scale(sphere4, Point3D(0, 0, 0), factor5);

	raytracer.translate(sphere3, Vector3D(0, 2, -6));
	raytracer.scale(sphere3, Point3D(0, 0, 0), factor6);

	raytracer.translate(sphere2, Vector3D(2, 0, -6));
	raytracer.scale(sphere2, Point3D(0, 0, 0), factor5);

	raytracer.translate(sphere1, Vector3D(0, -2, -6));
	raytracer.scale(sphere1, Point3D(0, 0, 0), factor4);

	raytracer.translate(plane, Vector3D(0, 0, -8));
	raytracer.rotate(plane, 'z', 45);
	raytracer.scale(plane, Point3D(0, 0, 0), factor7);

	raytracer.translate(cylinder, Vector3D(0, 0, -6));
	raytracer.scale(cylinder, Point3D(0, 0, 0), factor6);

	// Render the scene, feel free to make the image smaller for
	// testing purposes.
	raytracer.render(raytracer.imgWidth, raytracer.imgHeight, eye, view, up, fov, "scene1_view1.bmp");

	// Render it from a different point of view.
	Point3D eye2(4, 2, 1);
	Vector3D view2(-4, -2, -6);
	raytracer.render(raytracer.imgWidth, raytracer.imgHeight, eye2, view2, up, fov, "scene1_view2.bmp");
}
void Scene::scene2(Raytracer raytracer)
{
	// Camera parameters
	Point3D eye(0, 0, 1);
	Vector3D view(0, 0, -1);
	Vector3D up(0, 1, 0);

	// Defines a material for shading.
	Material gold(Colour(0.3, 0.3, 0.3), Colour(0.75164, 0.60648, 0.22648),
		Colour(0.628281, 0.555802, 0.366065),
		51.2);
	Material jade(Colour(0, 0, 0), Colour(0.54, 0.89, 0.63),
		Colour(0.316228, 0.316228, 0.316228),
		12.8);

	// Initialize OpenCL
	//raytracer.initOpenCL();

	/****************** An area light source. ************************/
	raytracer.addAreaLightSource(3, 2, 2, 8, Point3D(0, 0, 5), Colour(0.01, 0.01, 0.01));

	// Add a unit square into the scene with material mat.
	SceneDagNode* plane = raytracer.addObject(new UnitSquare(), &jade);
	SceneDagNode* cylinder = raytracer.addObject(new UnitCylinder(), &gold);

	// Apply some transformations to the unit square.
	double factor6[3] = { 1.0, 1.0, 4.0 };
	double factor7[3] = { 10.0, 10.0, 10.0 };

	raytracer.translate(plane, Vector3D(0, 0, -8));
	raytracer.rotate(plane, 'z', 45);
	raytracer.scale(plane, Point3D(0, 0, 0), factor7);

	raytracer.translate(cylinder, Vector3D(2, 0, -6));
	raytracer.scale(cylinder, Point3D(0, 0, 0), factor6);

	// Render the scene, feel free to make the image smaller for
	// testing purposes.
	raytracer.render(raytracer.imgWidth, raytracer.imgHeight, eye, view, up, fov, "scene2_view1.bmp");
}