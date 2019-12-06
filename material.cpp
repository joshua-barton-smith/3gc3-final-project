#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#endif

#include "material.h"

Material::Material() {}

// material just stores the 4 properties passed as arguments
Material::Material(float *amb, float *diff, float *spec, float *emm, float shin) {
	for (int i = 0; i < 4; i++) {
		this->amb[i] = amb[i];
		this->diff[i] = diff[i];
		this->spec[i] = spec[i];
		this->emm[i] = emm[i];
	}
	this->shin = shin;
}

// opengl commands to bind the material properties
void Material::bind() {
	glMaterialfv(GL_FRONT, GL_AMBIENT, this->amb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, this->diff);
	glMaterialfv(GL_FRONT, GL_SPECULAR, this->spec);
	glMaterialfv(GL_FRONT, GL_EMISSION, this->emm);
	glMaterialf(GL_FRONT, GL_SHININESS, this->shin);
}