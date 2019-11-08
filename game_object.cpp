#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#endif

#include "game_object.h"
#include "mathLib3D.h"
#include "mesh.h"

// need a way to find size of the object for checking what object is being interacted with
// we can check within a range of the object so it doesnt have to be super precise.
GameObject::GameObject(Mesh *mesh, Point3D position, Point3D rotation, float scale, bool random) {
	this->mesh = mesh;
	this->position = position;
	this->rotation = rotation;
	this->scale = scale;
	this->random = random;
}

void GameObject::render() {
	glPushMatrix();
		glTranslatef(this->position.mX, this->position.mY, this->position.mZ);
		glRotatef(this->rotation.mX, 1.0, 0.0, 0.0);
		glRotatef(this->rotation.mY, 0.0, 1.0, 0.0);
		glRotatef(this->rotation.mZ, 0.0, 0.0, 1.0);
		glScalef(scale, scale, scale);

		this->mesh->render();
	glPopMatrix();
}