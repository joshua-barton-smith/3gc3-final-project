#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "mathLib3D.h"
#include "mesh.h"

class GameObject {
public:
	GameObject();
	GameObject(Point3D position, Point3D rotation, float scale, bool random, bool physics);
	GameObject(Mesh *mesh, Point3D position, Point3D rotation, float scale, bool random);

	void render();
	virtual void logic(std::vector<GameObject> others, int idx);

	bool check_collision(GameObject g);

	Point3D position;
	Point3D rotation;
	float scale;

	float bounds[6];

	float grav;

	bool random;
	bool norender;

	bool physics;

	Mesh *mesh;
};

#endif