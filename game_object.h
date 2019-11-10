#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "mathLib3D.h"
#include "mesh.h"

class GameObject {
public:
	GameObject(Mesh *mesh, Point3D position, Point3D rotation, float scale, bool random);

	void render();

	Point3D position;
	Point3D rotation;
	float scale;

	float bounds[3];
	float center[3];

	bool random;

	Mesh *mesh;
};

#endif