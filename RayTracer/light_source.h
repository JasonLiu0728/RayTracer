/***********************************************************

Author: Mark Liu, Seungmoon Rieh
light source classes

***********************************************************/

#pragma once
#include "util.h"

// Base class for a light source.  You could define different types
// of lights here, but point light is sufficient for most scenes you
// might want to render.  Different light sources shade the ray 
// differently.
class LightSource {
public:
	LightSource() : sceneSignature(false) {};
	LightSource(bool sceneSig) : sceneSignature(sceneSig) {};
	~LightSource() {};
	virtual void shade( Ray3D& ) = 0;
	virtual Point3D get_position() const = 0;

	bool sceneSignature;
};

// A point light is defined by its position in world space and its
// colour.
class PointLight : public LightSource {
public:
	PointLight(Point3D pos, Colour col) : _pos(pos), _col_ambient(col),
		_col_diffuse(col), _col_specular(col), sceneSignature(false) {};
	PointLight(Point3D pos, Colour ambient, Colour diffuse, Colour specular)
		: _pos(pos), _col_ambient(ambient), _col_diffuse(diffuse),
		_col_specular(specular), sceneSignature(false) {};
	PointLight(Point3D pos, Colour col, bool sceneSig) : _pos(pos), _col_ambient(col),
		_col_diffuse(col), _col_specular(col), sceneSignature(sceneSig) {};
	PointLight(Point3D pos, Colour ambient, Colour diffuse, Colour specular, bool sceneSig)
		: _pos(pos), _col_ambient(ambient), _col_diffuse(diffuse),
		_col_specular(specular), sceneSignature(sceneSig) {};
	void shade( Ray3D& ray );
	Point3D get_position() const { return _pos; }
	
private:
	Point3D _pos;
	Colour _col_ambient;
	Colour _col_diffuse; 
	Colour _col_specular;
	bool sceneSignature;
};
