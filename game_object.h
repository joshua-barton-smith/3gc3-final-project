#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "mathLib3D.h"
#include "mesh.h"

class GameObject {
public:
	GameObject(Point3D position, Point3D rotation, float scale, bool random);
	GameObject(Mesh *mesh, Point3D position, Point3D rotation, float scale, bool random);

	void render();
	void logic(std::vector<GameObject> others, int idx);

	Point3D position;
	Point3D rotation;
	float scale;

	float bounds[6];

	float grav;

	bool random;
	bool norender;

	Mesh *mesh;
};

#endif