#ifndef MATERIAL_H
#define MATERIAL_H

class Material {
public:
	Material();
	Material(float *amb, float *diff, float *spec, float shin);

	void bind();

	float amb[4];
	float diff[4];
	float spec[4];
	float shin;
};

#endif