#include "scene.h"
#include "game_object.h"
#include "light.h"

#include <vector>
#include <iostream>

Scene::Scene() {}

Scene::Scene(std::vector<GameObject> objs, std::vector<Light> lights) {
	this->objs = objs;
	this->lights = lights;
}

void Scene::render() {
	for (size_t i = 0; i < this->lights.size(); i++) {
		this->lights[i].render();
	}

	for (size_t i = 0; i < this->objs.size(); i++) {
		this->objs[i].render();
	}
}