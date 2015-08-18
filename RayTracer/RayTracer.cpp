// RayTracer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

Raytracer::Raytracer() : _lightSource(NULL) {
	_root = new SceneDagNode();
}

Raytracer::~Raytracer() {
	delete _root;
}

void Raytracer::initOpenCL()
{
	// Get a platform
	cl_platform_id platform;
	cl_uint num_platforms;
	clGetPlatformIDs(1, &platform, &num_platforms);

	// Get a device
	cl_device_id deviceGPU;
	cl_uint num_devices;
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &deviceGPU, &num_devices);

	// Create a context and command queue on the GPU device
	cl_context context = clCreateContext(NULL, 1, &deviceGPU, NULL, NULL, NULL);
	cl_command_queue queue = clCreateCommandQueue(context, deviceGPU, 0, NULL);

	// Perform runtime source compilation, and obtain kernel entry point
	std::ifstream file("kernel.txt");
	std::string source;
	while (!file.eof())
	{
		char line[256];
		file.getline(line, 255);
		source += line;
	}
	const char* str = source.c_str();
	cl_program program = clCreateProgramWithSource(context, 1, &str, NULL, NULL);
	cl_int result = clBuildProgram(program, 1, &deviceGPU, NULL, NULL, NULL);
	if (result)
	{
		std::cout << "Error during compilation! (" << result << ")" << std::endl;
	}
	else
	{
		std::cout << "Compilation OK" << std::endl;
	}
	/*cl_kernel kernel = clCreateKernel(program, "main", NULL);
	std::cout << "Kernel created successfully" << std::endl;*/
}

SceneDagNode* Raytracer::addObject(SceneDagNode* parent,
	SceneObject* obj, Material* mat) {
	SceneDagNode* node = new SceneDagNode(obj, mat);
	node->parent = parent;
	node->next = NULL;
	node->child = NULL;

	// Add the object to the parent's child list, this means
	// whatever transformation applied to the parent will also
	// be applied to the child.
	if (parent->child == NULL) {
		parent->child = node;
	}
	else {
		parent = parent->child;
		while (parent->next != NULL) {
			parent = parent->next;
		}
		parent->next = node;
	}

	return node;;
}

LightListNode* Raytracer::addLightSource(LightSource* light)
{
	LightListNode* tmp = _lightSource;
	_lightSource = new LightListNode(light, tmp);
	return _lightSource;
}

void Raytracer::addAreaLightSource(int orient, double l, double w, double density, Point3D origin, Colour col)
{

	//	col[0] = col[0] / ((density)*(density));
	//	col[1] = col[1] / ((density)*(density));
	//	col[2] = col[2] / ((density)*(density));

	if (orient == 1)
	{// Area light in the yz plane
		for (double i = origin[1] - l / 2; i <= origin[1] + l / 2; i += l / density)
		{
			for (double j = origin[2] - w / 2; j <= origin[2] + w / 2; j += w / density)
			{
				addLightSource(new PointLight(Point3D(origin[0], i, j), col, true));
			}
		}
	}
	if (orient == 2)
	{// Area light in the xz plane
		for (double i = origin[0] - l / 2; i <= origin[0] + l / 2; i += l / density)
		{
			for (double j = origin[2] - w / 2; j <= origin[2] + w / 2; j += w / density)
			{
				addLightSource(new PointLight(Point3D(i, origin[1], j), col));
			}
		}
	}
	if (orient == 3)
	{// Area light in the xy plane
		for (double i = origin[0] - l / 2; i <= origin[0] + l / 2; i += l / density)
		{
			for (double j = origin[1] - w / 2; j <= origin[1] + w / 2; j += w / density)
			{
				addLightSource(new PointLight(Point3D(i, j, origin[2]), col));
			}
		}
	}
}

void Raytracer::rotate(SceneDagNode* node, char axis, double angle) {
	Matrix4x4 rotation;
	double toRadian = 2 * M_PI / 360.0;
	int i;

	for (i = 0; i < 2; i++) {
		switch (axis) {
		case 'x':
			rotation[0][0] = 1;
			rotation[1][1] = cos(angle*toRadian);
			rotation[1][2] = -sin(angle*toRadian);
			rotation[2][1] = sin(angle*toRadian);
			rotation[2][2] = cos(angle*toRadian);
			rotation[3][3] = 1;
			break;
		case 'y':
			rotation[0][0] = cos(angle*toRadian);
			rotation[0][2] = sin(angle*toRadian);
			rotation[1][1] = 1;
			rotation[2][0] = -sin(angle*toRadian);
			rotation[2][2] = cos(angle*toRadian);
			rotation[3][3] = 1;
			break;
		case 'z':
			rotation[0][0] = cos(angle*toRadian);
			rotation[0][1] = -sin(angle*toRadian);
			rotation[1][0] = sin(angle*toRadian);
			rotation[1][1] = cos(angle*toRadian);
			rotation[2][2] = 1;
			break;
		}
		if (i == 0) {
			node->trans = node->trans*rotation;
			angle = -angle;
		}
		else {
			node->invtrans = rotation*node->invtrans;
		}
	}
}

void Raytracer::translate(SceneDagNode* node, Vector3D trans) {
	Matrix4x4 translation;

	translation[0][3] = trans[0];
	translation[1][3] = trans[1];
	translation[2][3] = trans[2];
	node->trans = node->trans*translation;
	translation[0][3] = -trans[0];
	translation[1][3] = -trans[1];
	translation[2][3] = -trans[2];
	node->invtrans = translation*node->invtrans;
}

void Raytracer::scale(SceneDagNode* node, Point3D origin, double factor[3]) {
	Matrix4x4 scale;

	scale[0][0] = factor[0];
	scale[0][3] = origin[0] - factor[0] * origin[0];
	scale[1][1] = factor[1];
	scale[1][3] = origin[1] - factor[1] * origin[1];
	scale[2][2] = factor[2];
	scale[2][3] = origin[2] - factor[2] * origin[2];
	node->trans = node->trans*scale;
	scale[0][0] = 1 / factor[0];
	scale[0][3] = origin[0] - 1 / factor[0] * origin[0];
	scale[1][1] = 1 / factor[1];
	scale[1][3] = origin[1] - 1 / factor[1] * origin[1];
	scale[2][2] = 1 / factor[2];
	scale[2][3] = origin[2] - 1 / factor[2] * origin[2];
	node->invtrans = scale*node->invtrans;
}

Matrix4x4 Raytracer::initInvViewMatrix(Point3D eye, Vector3D view,
	Vector3D up) {
	Matrix4x4 mat;
	Vector3D w;
	view.normalize();
	up = up - up.dot(view)*view;
	up.normalize();
	w = view.cross(up);

	mat[0][0] = w[0];
	mat[1][0] = w[1];
	mat[2][0] = w[2];
	mat[0][1] = up[0];
	mat[1][1] = up[1];
	mat[2][1] = up[2];
	mat[0][2] = -view[0];
	mat[1][2] = -view[1];
	mat[2][2] = -view[2];
	mat[0][3] = eye[0];
	mat[1][3] = eye[1];
	mat[2][3] = eye[2];

	return mat;
}

void Raytracer::traverseScene(SceneDagNode* node, Ray3D& ray) {
	SceneDagNode *childPtr;

	// Applies transformation of the current node to the global
	// transformation matrices.
	_modelToWorld = _modelToWorld*node->trans;
	_worldToModel = node->invtrans*_worldToModel;
	if (node->obj) {
		// Perform intersection.
		if (node->obj->intersect(ray, _worldToModel, _modelToWorld)) {
			ray.intersection.mat = node->mat;
		}
	}
	// Traverse the children.
	childPtr = node->child;
	while (childPtr != NULL) {
		traverseScene(childPtr, ray);
		childPtr = childPtr->next;
	}

	// Removes transformation of the current node from the global
	// transformation matrices.
	_worldToModel = node->trans*_worldToModel;
	_modelToWorld = _modelToWorld*node->invtrans;
}

void Raytracer::computeShading(Ray3D& ray)
{
	LightListNode* curLight = _lightSource;
	for (;;) {
		if (curLight == NULL) break;

		// Shadow
		if (enableShadow)
		{
			bool inShadow = false;
			Ray3D shadowRay;
			Point3D lightPos = (curLight->light)->get_position();
			shadowRay.origin = lightPos;
			shadowRay.dir = ray.intersection.point - lightPos;
			traverseScene(_root, shadowRay);
			if (shadowRay.intersection.t_value > 0.0001 && shadowRay.intersection.t_value < 0.999) {
				inShadow = true;
			}

			if (!inShadow) {
				curLight->light->shade(ray);
				curLight = curLight->next;
			}
			else {
				curLight = curLight->next;
			}
		}

		// No shadow
		else
		{
			curLight->light->shade(ray);
			curLight = curLight->next;
		}
	}
}

void Raytracer::initPixelBuffer() {
	int numbytes = _scrWidth * _scrHeight * sizeof(unsigned char);
	_rbuffer = new unsigned char[numbytes];
	_gbuffer = new unsigned char[numbytes];
	_bbuffer = new unsigned char[numbytes];
	for (int i = 0; i < _scrHeight; i++) {
		for (int j = 0; j < _scrWidth; j++) {
			_rbuffer[i*_scrWidth + j] = 0;
			_gbuffer[i*_scrWidth + j] = 0;
			_bbuffer[i*_scrWidth + j] = 0;
		}
	}
}

void Raytracer::flushPixelBuffer(char *file_name) {
	bmp_write(file_name, _scrWidth, _scrHeight, _rbuffer, _gbuffer, _bbuffer);
	delete _rbuffer;
	delete _gbuffer;
	delete _bbuffer;
}

// Calculate the direction of the reflected ray
Ray3D reflectRay(Ray3D& ray){
	ray.dir.normalize();
	ray.intersection.normal.normalize();
	Vector3D direction = ray.dir - 2 * (ray.dir.dot(ray.intersection.normal)) * ray.intersection.normal;
	return Ray3D(ray.intersection.point, direction);
}

Colour Raytracer::shadeRay(Ray3D& ray, int depth) {
	Colour col(0.0, 0.0, 0.0);
	traverseScene(_root, ray);

	// Don't bother shading if the ray didn't hit
	// anything.
	if (!ray.intersection.none) {
		computeShading(ray);

		// You'll want to call shadeRay recursively (with a different ray,
		// of course) here to implement reflection/refraction effects.
		if (depth < _maxDepth) {

			/****************** No glossy reflection or refraction ************************/
			if (!enableGlossyReflection && !enableRefraction)
			{
				col = ray.col;
				col.clamp();
			}

			/****************** Glossy reflection ************************/
			if (enableGlossyReflection)
			{
				double reflectionCoef = 0;

				// Calculate reflection ray
				Vector3D reflection_N = ray.intersection.normal;
				Vector3D reflection_D = ray.dir;
				Vector3D reflectionDir = reflection_D - 2 * reflection_D.dot(reflection_N) * reflection_N;
				reflectionDir.normalize();
				Point3D reflectionOrigin = ray.intersection.point + 0.01*reflectionDir;
				Ray3D reflectionRay = Ray3D(reflectionOrigin, reflectionDir);

				shadeRay(reflectionRay, depth + 1);

				if (reflectionRay.intersection.t_value > 10 || reflectionRay.intersection.t_value <= 0) {
					col = ray.col;
				}
				else {
					reflectionCoef = fabs(1.0 / reflectionRay.intersection.t_value);
					reflectionCoef = fmax(0, fmin(reflectionCoef, 0.8));
					col = ray.col + reflectionCoef * reflectionRay.col;
				}
				col.clamp();
			}

			/****************** Refraction ************************/
			if (enableRefraction)
			{
				double refractionCoef = 0;
				double index = 0.8;

				Vector3D refraction_N = ray.intersection.normal;
				Vector3D refraction_D = ray.dir;
				double k = 1 - index * index * (1 - refraction_D.dot(refraction_N) * refraction_D.dot(refraction_N));
				Vector3D refractionDir(0, 0, 0);
				if (k >= 0) {
					refractionDir = index * refraction_D - (index * refraction_D.dot(refraction_N) + sqrt(k)) * refraction_N;
				}
				refractionDir.normalize();
				Point3D refractionOrigin = ray.intersection.point + 0.01*refractionDir;
				Ray3D refractionRay = Ray3D(refractionOrigin, refractionDir);

				shadeRay(refractionRay, depth + 1);

				if (refractionRay.intersection.t_value > 10 || refractionRay.intersection.t_value <= 0) {
					col = ray.col;
				}
				else {
					refractionCoef = fabs(1 / refractionRay.intersection.t_value);
					refractionCoef = fmax(0, fmin(refractionCoef, 0.8));
					col = ray.col + refractionCoef * refractionRay.col;
				}
				col.clamp();
			}
		}
	}
	return col;
}

void Raytracer::scene1()
{
	// Camera parameters
	Point3D eye(0, 0, 1);
	Vector3D view(0, 0, -1);
	Vector3D up(0, 1, 0);
	double fov = 60;

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
	//initOpenCL();

	/****************** An area light source. ************************/
	addAreaLightSource(3, 2, 2, 8, Point3D(0, 0, 5), Colour(0.01, 0.01, 0.01));

	// Add a unit square into the scene with material mat.
	//	SceneDagNode* sphere = addObject( new UnitSphere(), &gold );
	SceneDagNode* plane = addObject(new UnitSquare(), &jade);
	SceneDagNode* cylinder = addObject(new UnitCylinder(), &gold);
	SceneDagNode* sphere1 = addObject(new UnitSphere(), &gold);
	SceneDagNode* sphere2 = addObject(new UnitSphere(), &copper);
	SceneDagNode* sphere3 = addObject(new UnitSphere(), &glass);
	SceneDagNode* sphere4 = addObject(new UnitSphere(), &jade);

	// Apply some transformations to the unit square.
	/*double factor1[3] = { 1.0, 2.0, 1.0 };
	double factor2[3] = { 6.0, 6.0, 6.0 };
	double factor3[3] = { 1.5, 1.5, 3.0 };*/
	double scaleCylinder[3] = { 1.0, 1.0, 4.0 };
	double scalePlane[3] = { 10.0, 10.0, 10.0 };
	double scaleSphere1[3] = { 0.5, 0.5, 0.5 };
	double scaleSphere2[3] = { 0.5, 0.5, 0.5 };
	double scaleSphere3[3] = { 0.5, 0.5, 0.5 };
	double scaleSphere4[3] = { 0.5, 0.5, 0.5 };

	translate(sphere4, Vector3D(-2, 0, -6));
	scale(sphere4, Point3D(0, 0, 0), scaleSphere4);

	translate(sphere3, Vector3D(0, 2, -6));
	scale(sphere3, Point3D(0, 0, 0), scaleSphere3);

	translate(sphere2, Vector3D(2, 0, -6));
	scale(sphere2, Point3D(0, 0, 0), scaleSphere2);

	translate(sphere1, Vector3D(0, -2, -6));
	scale(sphere1, Point3D(0, 0, 0), scaleSphere1);

	translate(plane, Vector3D(0, 0, -10));
	scale(plane, Point3D(0, 0, 0), scalePlane);

	translate(cylinder, Vector3D(0, 0, -6));
	scale(cylinder, Point3D(0, 0, 0), scaleCylinder);

	// Render the scene, feel free to make the image smaller for
	// testing purposes.
	render(imgWidth, imgHeight, eye, view, up, fov, "scene1_view1.bmp");

	// Render it from a different point of view.
	Point3D eye2(4, 2, 1);
	Vector3D view2(-4, -2, -6);
	render(imgWidth, imgHeight, eye2, view2, up, fov, "scene1_view2.bmp");
}
void Raytracer::scene2()
{
	// Camera parameters.
	Point3D eye(0, 0, 1);
	Vector3D view(0, 0, -1);
	Vector3D up(0, 1, 0);
	double fov = 60;

	// Defines a material for shading.
	Material gold(Colour(0.3, 0.3, 0.3), Colour(0.75164, 0.60648, 0.22648),
		Colour(0.628281, 0.555802, 0.366065),
		51.2);
	Material jade(Colour(0, 0, 0), Colour(0.54, 0.89, 0.63),
		Colour(0.316228, 0.316228, 0.316228),
		12.8);

	// Defines a point light source.
	addLightSource(new PointLight(Point3D(0, 0, 5),
		Colour(0.9, 0.9, 0.9), sceneSignature));

	// Add a unit square into the scene with material mat.
	SceneDagNode* sphere = addObject(new UnitSphere(), &gold);
	SceneDagNode* plane = addObject(new UnitSquare(), &jade);

	// Apply some transformations to the unit square.
	double factor1[3] = { 1.0, 2.0, 1.0 };
	double factor2[3] = { 6.0, 6.0, 6.0 };
	translate(sphere, Vector3D(0, 0, -5));
	rotate(sphere, 'x', -45);
	rotate(sphere, 'z', 45);
	scale(sphere, Point3D(0, 0, 0), factor1);

	translate(plane, Vector3D(0, 0, -7));
	rotate(plane, 'z', 45);
	scale(plane, Point3D(0, 0, 0), factor2);

	// Render the scene, feel free to make the image smaller for
	// testing purposes.	
	render(imgWidth, imgHeight, eye, view, up, fov, "scene2_view1.bmp");

	// Render it from a different point of view.
	Point3D eye2(4, 2, 1);
	Vector3D view2(-4, -2, -6);
	render(imgWidth, imgHeight, eye2, view2, up, fov, "scene2_view2.bmp");
}

void Raytracer::render(int width, int height, Point3D eye, Vector3D view,
	Vector3D up, double fov, char* fileName) {
	Matrix4x4 viewToWorld;
	_scrWidth = width;
	_scrHeight = height;
	double factor = (double(height) / 2) / tan(fov*M_PI / 360.0);

	_maxDepth = 20;

	initPixelBuffer();
	viewToWorld = initInvViewMatrix(eye, view, up);


	/****************** Construct 4 rays for each pixel. (Anti-alising) ************************/
	if (enableFSAA)
	{
		for (int i = 0; i < _scrHeight; i++) {
			for (int j = 0; j < _scrWidth; j++) {
				for (float k = i; k < i + 1.0f; k += 0.5f){
					for (float l = j; l < j + 1.0f; l += 0.5f){
						// Sets up ray origin and direction in view space,
						// image plane is at z = -1.
						Point3D origin(0, 0, 0);
						Point3D imagePlane;
						imagePlane[0] = (-double(width) / 2 + 0.5 + l) / factor;
						imagePlane[1] = (-double(height) / 2 + 0.5 + k) / factor;
						imagePlane[2] = -1;

						// TODO: Convert ray to world space and call
						// shadeRay(ray) to generate pixel colour.
						float incre = 0.25f;
						Ray3D ray;
						ray.origin = viewToWorld*origin;
						ray.dir = viewToWorld*(imagePlane - origin);

						Colour col = shadeRay(ray, 0);
						_rbuffer[i*width + j] += int(col[0] * 255 * incre);
						_gbuffer[i*width + j] += int(col[1] * 255 * incre);
						_bbuffer[i*width + j] += int(col[2] * 255 * incre);
					}
				}
			}
		}
	}

	/****************** Construct 1 ray for each pixel. (No anti-alising) ************************/
	if (!enableFSAA)
	{
		for (int i = 0; i < _scrHeight; i++) {
			for (int j = 0; j < _scrWidth; j++) {

				// Sets up ray origin and direction in view space,
				// image plane is at z = -1.
				Point3D origin(0, 0, 0);
				Point3D imagePlane;
				imagePlane[0] = (-double(width) / 2 + 0.5 + j) / factor;
				imagePlane[1] = (-double(height) / 2 + 0.5 + i) / factor;
				imagePlane[2] = -1;

				// TODO: Convert ray to world space and call
				// shadeRay(ray) to generate pixel colour.
				Ray3D ray;
				viewToWorld = initInvViewMatrix(eye, view, up);
				ray.origin = viewToWorld*origin;
				ray.dir = viewToWorld*(imagePlane - origin);

				Colour col = shadeRay(ray, 0);
				_rbuffer[i*width + j] += int(col[0] * 255);
				_gbuffer[i*width + j] += int(col[1] * 255);
				_bbuffer[i*width + j] += int(col[2] * 255);
			}
			std::cout << ".";
		}
		std::cout << "\n";
	}
	


	/****************** Depth of field ************************/
	/*
	for (int e=0; e<8; e++) {
	// Shake eyes to create blur effect
	eye[0] = eye[0] + 0.02*cos(0 + e * (2*M_PI/8.0));
	eye[1] = eye[1] + 0.02*sin(0 + e * (2*M_PI/8.0));

	for (int i = 0; i < _scrHeight; i++) {
	for (int j = 0; j < _scrWidth; j++) {
	// Sets up ray origin and direction in view space,
	// image plane is at z = -1.
	Point3D origin(0, 0, 0);
	Point3D imagePlane;
	imagePlane[0] = (-double(width)/2 + 0.5 + j)/factor;
	imagePlane[1] = (-double(height)/2 + 0.5 + i)/factor;
	imagePlane[2] = -1;

	// TODO: Convert ray to world space and call
	// shadeRay(ray) to generate pixel colour.
	Ray3D ray;

	ray.origin = viewToWorld*imagePlane;
	// ray.dir = viewToWorld*(imagePlane-origin);
	ray.dir = ray.origin - eye;
	ray.dir.normalize();

	Colour col = shadeRay(ray, 0);
	_rbuffer[i*width+j] += int(col[0]*255*0.125);
	_gbuffer[i*width+j] += int(col[1]*255*0.125);
	_bbuffer[i*width+j] += int(col[2]*255*0.125);
	}
	}
	}
	*/

	flushPixelBuffer(fileName);
}

void Raytracer::argParse(int argc, char* argv[])
{
	std::cout << "argc = " << argc << std::endl;
	for (int i = 0; i < argc; i++)
	{
		std::cout << argv[i] << std::endl;
	}

	imgWidth = atoi(argv[1]);
	imgHeight = atoi(argv[2]);
	std::istringstream(argv[3]) >> std::boolalpha >> sceneSignature;
	std::istringstream(argv[4]) >> std::boolalpha >> enableShadow;
	std::istringstream(argv[5]) >> std::boolalpha >> enableGlossyReflection;
	std::istringstream(argv[6]) >> std::boolalpha >> enableRefraction;

	printf("\nwidth = %d, height = %d\n", imgWidth, imgHeight);
	std::cout << "Debug (Scene Signature)\t" << sceneSignature << std::endl;
	std::cout << "Soft Shadow\t\t" << enableShadow << std::endl;
	std::cout << "Glossy Reflection\t" << enableGlossyReflection << std::endl;
	std::cout << "Refraction\t\t" << enableRefraction << std::endl;
}



int main(int argc, char* argv[])
{
	// Build your scene and setup your camera here, by calling
	// functions from Raytracer.  The code here sets up an example
	// scene and renders it from two different view points, DO NOT
	// change this if you're just implementing part one of the
	// assignment.

	Raytracer raytracer;
	raytracer.argParse(argc, argv);
	raytracer.scene1();

	return 0;
}

