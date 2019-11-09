#ifndef SCENE_H
#define SCENE_H

#include "game_object.h"
#include "light.h"

#include <vector>

class Scene {
public:
	Scene();
	Scene(std::vector<GameObject> objs, std::vector<Light> lights);

	void render();
	// a list of all GameObjects which can be drawn in the scene, moved, interacted with, etc
	std::vector<GameObject> objs;
	std::vector<Light> lights;
};

#endif