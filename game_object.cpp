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

#include <iostream>

GameObject::GameObject() {}

GameObject::GameObject(Point3D position, Point3D rotation, float scale, bool random, bool physics, GLuint texture) {
	this->position = position;
	this->rotation = rotation;
	this->scale = scale;
	this->random = random;
	this->norender = true;
	this->physics = physics;
	this->grav = 0;
	this->texture = texture;
}

// need a way to find size of the object for checking what object is being interacted with
// we can check within a range of the object so it doesnt have to be super precise.
GameObject::GameObject(Mesh *mesh, Point3D position, Point3D rotation, float scale, bool random, GLuint texture) {
	this->mesh = mesh;
	this->position = position;
	this->rotation = rotation;
	this->scale = scale;
	this->random = random;
	this->mesh->computeBounds(this->bounds);
	this->bounds[0] *= this->scale;
	this->bounds[1] *= this->scale;
	this->bounds[2] *= this->scale;
	this->bounds[3] *= this->scale;
	this->bounds[4] *= this->scale;
	this->bounds[5] *= this->scale;
	this->grav = 0;
	this->physics = true;
	this->texture = texture;
	this->norender = false;
}

void GameObject::render() {
	if(norender) return;

	glBindTexture(GL_TEXTURE_2D, this->texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glPushMatrix();
		glTranslatef(this->position.mX, this->position.mY, this->position.mZ);
		glRotatef(this->rotation.mX, 1.0, 0.0, 0.0);
		glRotatef(this->rotation.mY, 0.0, 1.0, 0.0);
		glRotatef(this->rotation.mZ, 0.0, 0.0, 1.0);
		glScalef(scale, scale, scale);

		this->mesh->render();
	glPopMatrix();
}

void GameObject::logic(std::vector<GameObject> others, int idx) {
	Point3D oldpos = Point3D(this->position.mX, this->position.mY, this->position.mZ);
	if (physics) {
		// apply gravity
		this->grav += 0.0098;
		this->position.mY -= grav;
		// check collision
		for (size_t i = 0; i < others.size(); i++) {
			if (i != idx) {
				GameObject g = others[i];
				if (this->check_collision(g)) {
		            	this->position = Point3D(oldpos.mX, oldpos.mY, oldpos.mZ);
		            	this->grav = 0;
		    	}
	    	}
		}
	}
}

bool GameObject::check_collision(GameObject g) {
	return ((this->position.mX + this->bounds[0]) < (g.position.mX + g.bounds[3])
		            && (this->position.mX + this->bounds[3]) > (g.position.mX + g.bounds[0]) &&
		            (this->position.mY + this->bounds[1]) < (g.position.mY + g.bounds[4])
		            && (this->position.mY + this->bounds[4]) > (g.position.mY + g.bounds[1]) &&
		            (this->position.mZ + this->bounds[2]) < (g.position.mZ + g.bounds[5])
		            && (this->position.mZ + this->bounds[5]) > (g.position.mZ + g.bounds[2]));
}