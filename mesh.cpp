#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#endif

#include <string>
#include <iostream>

#include "mesh.h"
#include "mathLib3D.h"

#include "include/tiny_obj_loader.h"

Mesh::Mesh() {}

// construct a mesh from loaded data from tinyobj,
// also constructs Material objects to use for all faces
Mesh::Mesh(tinyobj::attrib_t attribs, std::vector<tinyobj::shape_t> shapes, std::vector<tinyobj::material_t> materials) {
	this->attribs = attribs;
	this->shapes = shapes;
	for (size_t i = 0; i < materials.size(); i++) {
		tinyobj::material_t mat = materials[i];
		float amb[4] = {mat.ambient[0], mat.ambient[1], mat.ambient[2], 1.0};
		float diff[4] = {mat.diffuse[0], mat.diffuse[1], mat.diffuse[2], 1.0};
		float spec[4] = {mat.specular[0], mat.specular[1], mat.specular[2], 1.0};
		Material m = Material(amb, diff, spec, mat.shininess);
		this->materials.push_back(m);
	}
}

// this is used for hitboxes.
void Mesh::computeBounds(float *bounds) {
	float max_x = -999999999;
	float min_x = 999999999;
	float max_y = -999999999;
	float min_y = 999999999;
	float max_z = -999999999;
	float min_z = 999999999;

	// Loop over shapes in the model
	for (size_t s = 0; s < shapes.size(); s++) {
		// Loop over faces in this shape
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			// this is the count of vertices in the face
			// (but for now I am assuming all faces have 3 bc i triangulate in Blender)
			int fv = shapes[s].mesh.num_face_vertices[f];

			// Loop over vertices in the face.
			for (int v = 0; v < fv; v++) {
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				tinyobj::real_t vx = attribs.vertices[3*idx.vertex_index+0];
				tinyobj::real_t vy = attribs.vertices[3*idx.vertex_index+1];
				tinyobj::real_t vz = attribs.vertices[3*idx.vertex_index+2];

				// compare to existing max/min values
				if (vx > max_x) max_x = vx;
				if (vx < min_x) min_x = vx;

				if (vy > max_y) max_y = vy;
				if (vy < min_y) min_y = vy;

				if (vz > max_z) max_z = vz;
				if (vz < min_z) min_z = vz;
			}

			index_offset += fv;
		}
	}

	// return these
	bounds[0] = min_x;
	bounds[1] = min_y;
	bounds[2] = min_z;
	bounds[3] = max_x;
	bounds[4] = max_y;
	bounds[5] = max_z; 
}

// renders the mesh
void Mesh::render() {
	// Loop over shapes in the model
	for (size_t s = 0; s < shapes.size(); s++) {
		// Loop over faces in this shape
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			// this is the count of vertices in the face
			// (but for now I am assuming all faces have 3 bc i triangulate in Blender)
			int fv = shapes[s].mesh.num_face_vertices[f];

			// per-face material
			this->materials[shapes[s].mesh.material_ids[f]].bind();

			// gl render stuff
			glBegin(GL_TRIANGLES);

			// Loop over vertices in the face.
			for (int v = 0; v < fv; v++) {
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				tinyobj::real_t vx = attribs.vertices[3*idx.vertex_index+0];
				tinyobj::real_t vy = attribs.vertices[3*idx.vertex_index+1];
				tinyobj::real_t vz = attribs.vertices[3*idx.vertex_index+2];
				tinyobj::real_t nx = attribs.normals[3*idx.normal_index+0];
				tinyobj::real_t ny = attribs.normals[3*idx.normal_index+1];
				tinyobj::real_t nz = attribs.normals[3*idx.normal_index+2];
				// we will need these at some point for texturing.
				//tinyobj::real_t tx = attribs.texcoords[2*idx.texcoord_index+0];
				//tinyobj::real_t ty = attribs.texcoords[2*idx.texcoord_index+1];

				// draw the vertex with its predefined normal
				glNormal3f(nx, ny, nz);
				glVertex3f(vx, vy, vz);
			}

			glEnd();
			index_offset += fv;
		}
	}
}