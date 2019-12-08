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
#include "material.h"

#include <iostream>
#include <string>

GameObject::GameObject() {}

GameObject::GameObject(Point3D position, Point3D rotation, float scale, bool random, bool physics, GLuint texture, std::string name, std::string desc) {
	this->position = position;
	this->rotation = rotation;
	this->scale = scale;
	this->random = random;
	this->norender = true;
	this->physics = physics;
	this->grav = 0;
	this->texture = texture;
	this->intersects = false;
	float amb[4] = {0.0, 0.0, 1.0, 1.0};
	float diff[4] = {0.0, 0.0, 1.0, 1.0};
	float spec[4] = {0.0, 0.0, 1.0, 1.0};
	float emm[4] = {0.0, 0.0, 1.0, 1.0};
	float shin = 100.0;
	this->cubemat = Material(amb, diff, spec, emm, shin);
	this->name = name;
	this->desc = desc;
}

// need a way to find size of the object for checking what object is being interacted with
// we can check within a range of the object so it doesnt have to be super precise.
GameObject::GameObject(Mesh *mesh, Point3D position, Point3D rotation, float scale, bool random, GLuint texture, std::string name, std::string desc) {
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
	this->intersects = false;
	float amb[4] = {0.0, 0.0, 1.0, 1.0};
	float diff[4] = {0.0, 0.0, 1.0, 1.0};
	float spec[4] = {0.0, 0.0, 1.0, 1.0};
	float emm[4] = {0.0, 0.0, 1.0, 1.0};
	float shin = 100.0;
	this->cubemat = Material(amb, diff, spec, emm, shin);
	this->name = name;
	this->desc = desc;
}

void GameObject::render() {
	if(norender) return;

	if (intersects) {
		glBindTexture(GL_TEXTURE_2D, 0);
		this->cubemat.bind();
		glPushMatrix();
			glDisable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glBegin(GL_QUADS);
				// front face
				glVertex3f(this->position.mX + this->bounds[0], this->position.mY + this->bounds[1], this->position.mZ + this->bounds[2]);
				glVertex3f(this->position.mX + this->bounds[3], this->position.mY + this->bounds[1], this->position.mZ + this->bounds[2]);
				glVertex3f(this->position.mX + this->bounds[3], this->position.mY + this->bounds[4], this->position.mZ + this->bounds[2]);
				glVertex3f(this->position.mX + this->bounds[0], this->position.mY + this->bounds[4], this->position.mZ + this->bounds[2]);
			glEnd();

			glBegin(GL_QUADS);
				// back face
				glVertex3f(this->position.mX + this->bounds[0], this->position.mY + this->bounds[1], this->position.mZ + this->bounds[5]);
				glVertex3f(this->position.mX + this->bounds[3], this->position.mY + this->bounds[1], this->position.mZ + this->bounds[5]);
				glVertex3f(this->position.mX + this->bounds[3], this->position.mY + this->bounds[4], this->position.mZ + this->bounds[5]);
				glVertex3f(this->position.mX + this->bounds[0], this->position.mY + this->bounds[4], this->position.mZ + this->bounds[5]);
			glEnd();

			glBegin(GL_QUADS);
				// left face
				glVertex3f(this->position.mX + this->bounds[0], this->position.mY + this->bounds[1], this->position.mZ + this->bounds[2]);
				glVertex3f(this->position.mX + this->bounds[0], this->position.mY + this->bounds[4], this->position.mZ + this->bounds[2]);
				glVertex3f(this->position.mX + this->bounds[0], this->position.mY + this->bounds[4], this->position.mZ + this->bounds[5]);
				glVertex3f(this->position.mX + this->bounds[0], this->position.mY + this->bounds[1], this->position.mZ + this->bounds[5]);
			glEnd();

			glBegin(GL_QUADS);
				// right face
				glVertex3f(this->position.mX + this->bounds[3], this->position.mY + this->bounds[1], this->position.mZ + this->bounds[2]);
				glVertex3f(this->position.mX + this->bounds[3], this->position.mY + this->bounds[4], this->position.mZ + this->bounds[2]);
				glVertex3f(this->position.mX + this->bounds[3], this->position.mY + this->bounds[4], this->position.mZ + this->bounds[5]);
				glVertex3f(this->position.mX + this->bounds[3], this->position.mY + this->bounds[1], this->position.mZ + this->bounds[5]);
			glEnd();

			glBegin(GL_QUADS);
				// bottom face
				glVertex3f(this->position.mX + this->bounds[0], this->position.mY + this->bounds[1], this->position.mZ + this->bounds[2]);
				glVertex3f(this->position.mX + this->bounds[3], this->position.mY + this->bounds[1], this->position.mZ + this->bounds[2]);
				glVertex3f(this->position.mX + this->bounds[3], this->position.mY + this->bounds[1], this->position.mZ + this->bounds[5]);
				glVertex3f(this->position.mX + this->bounds[0], this->position.mY + this->bounds[1], this->position.mZ + this->bounds[5]);
			glEnd();

			glBegin(GL_QUADS);
				// top face
				glVertex3f(this->position.mX + this->bounds[0], this->position.mY + this->bounds[4], this->position.mZ + this->bounds[2]);
				glVertex3f(this->position.mX + this->bounds[3], this->position.mY + this->bounds[4], this->position.mZ + this->bounds[2]);
				glVertex3f(this->position.mX + this->bounds[3], this->position.mY + this->bounds[4], this->position.mZ + this->bounds[5]);
				glVertex3f(this->position.mX + this->bounds[0], this->position.mY + this->bounds[4], this->position.mZ + this->bounds[5]);
			glEnd();
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_CULL_FACE);
		glPopMatrix();
	}

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
	// this is a cheat, to stop things falling through the floor.
	// if something is below the floor, set y pos to 0.
	if (oldpos.mY < -4) {
		oldpos.mY = 0;
		this->position.mY = 0;
		this->grav = 0;
	}
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

bool GameObject::line_intersects(Vec3D l, Vec3D l0, float *t) {
	if(!(this->random)) return false;
	Vec3D dirfrac = Vec3D(1.0f/l.mX, 1.0f/l.mY, 1.0f/l.mZ);

	Point3D lb = Point3D(this->position.mX + this->bounds[0], this->position.mY + this->bounds[1], this->position.mZ + this->bounds[2]);
	Point3D rt = Point3D(this->position.mX + this->bounds[3], this->position.mY + this->bounds[4], this->position.mZ + this->bounds[5]);

	float t1 = (lb.mX - l0.mX) * dirfrac.mX;
	float t2 = (rt.mX - l0.mX) * dirfrac.mX;
	float t3 = (lb.mY - l0.mY) * dirfrac.mY;
	float t4 = (rt.mY - l0.mY) * dirfrac.mY;
	float t5 = (lb.mZ - l0.mZ) * dirfrac.mZ;
	float t6 = (rt.mZ - l0.mZ) * dirfrac.mZ;

	float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
	float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

	// if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
	if (tmax < 0)
	{
	    *t = tmax;
	    return false;
	}

	// if tmin > tmax, ray doesn't intersect AABB
	if (tmin > tmax)
	{
	    *t = tmax;
	    return false;
	}

	*t = tmin;
	return true;
}