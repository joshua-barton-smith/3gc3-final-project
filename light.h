#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include <GLUT/glut.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
  #include <GL/freeglut.h>
#endif

#ifndef LIGHT_H
#define LIGHT_H

class Light {
public:
	Light();
	Light(GLenum light, float position[], float ambient[], float diffuse[], float specular[], bool sphere);
	void render();
	void update(GLenum property, float data[]);
	GLenum boundLight;
	float position[4];
	float ambient[4];
	float diffuse[4];
	float specular[4];
	bool sphere;

	float constant;
	float linear;
	float quadratic;
};

#endif