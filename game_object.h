#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#endif

#include "mathLib3D.h"
#include "mesh.h"

class GameObject {
public:
	GameObject();
	GameObject(Point3D position, Point3D rotation, float scale, bool random, bool physics, GLuint texture);
	GameObject(Mesh *mesh, Point3D position, Point3D rotation, float scale, bool random, GLuint texture);

	void render();
	virtual void logic(std::vector<GameObject> others, int idx);

	bool check_collision(GameObject g);

	Point3D position;
	Point3D rotation;
	float scale;

	GLuint texture;

	float bounds[6];

	float grav;

	bool random;
	bool norender;

	bool physics;

	Mesh *mesh;
};

#endif