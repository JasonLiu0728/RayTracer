/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements light_source.h

***********************************************************/

#include "stdafx.h"
//#include <cmath>
//#include "light_source.h"

void PointLight::shade( Ray3D& ray ) {
	if (sceneSignature)
	{
		// Normalized vectors needed for phong shading
		Vector3D N = ray.intersection.normal; // normal
		Vector3D L = _pos - ray.intersection.point; // light source direction

		N.normalize();
		L.normalize();

		Colour Id = (ray.intersection.mat->diffuse) * (max(0.0, N.dot(L))*_col_diffuse);

		Id.clamp();
		ray.col = ray.col + Id;
	}
	else
	{
		// Normalized vectors needed for phong shading
		Vector3D N = ray.intersection.normal; // normal
		Vector3D L = _pos - ray.intersection.point; // light source direction
		Vector3D V = -ray.dir; // reflection
		Vector3D R = 2.*(L.dot(N)*N) - L; // Perfect Mirror Specular Reflection

		N.normalize();
		L.normalize();
		V.normalize();
		R.normalize();

		//intensity due to ambient light
		Colour Ia = (ray.intersection.mat->ambient) * _col_ambient;

		//intensity due to diffuse light
		Colour Id = (ray.intersection.mat->diffuse) * (max(0.0, N.dot(L))*_col_diffuse);

		//intensity due to specular light
		Colour Is = (ray.intersection.mat->specular) * (max(0.0, pow(V.dot(R), (*ray.intersection.mat).specular_exp))*_col_specular);

		//Phong shading
		Ia.clamp();
		Id.clamp();
		Is.clamp();

		ray.col = ray.col + Ia + Id + Is;
	}
	
}

