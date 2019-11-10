#ifndef MESH_H
#define MESH_H

#include "include/tiny_obj_loader.h"
#include "material.h"
#include "mathLib3D.h"

class Mesh {
public:
	Mesh();
	Mesh(tinyobj::attrib_t attribs, std::vector<tinyobj::shape_t> shapes, std::vector<tinyobj::material_t> materials);

	void render();
	void computeBounds(float *bounds, float *center);

	tinyobj::attrib_t attribs;
	std::vector<tinyobj::shape_t> shapes;
	//std::vector<tinyobj::material_t> materials;
	std::vector<Material> materials;
};

#endif