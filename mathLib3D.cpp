#include <math.h>
#include <cmath>
#include "mathLib3D.h"

Point3D::Point3D() : Point3D(0.0, 0.0, 0.0) {}

Point3D::Point3D(float inX, float inY, float inZ) {
	mX = inX;
	mY = inY;
	mZ = inZ;
}

float Point3D::distanceTo(Point3D other) {
	return sqrt(pow(other.mX - mX, 2) + pow(other.mY - mY, 2) + pow(other.mZ - mZ, 2));
}

float Point3D::fastDistanceTo(Point3D other) {
	return pow(other.mX - mX, 2) + pow(other.mY - mY, 2) + pow(other.mZ - mZ, 2);
}

Vec3D::Vec3D() : Vec3D(0.0, 0.0, 0.0) {}

Vec3D::Vec3D(float inX, float inY, float inZ) {
	mX = inX;
	mY = inY;
	mZ = inZ;
}

float Vec3D::length() {
	return sqrt(pow(mX, 2) + pow(mY, 2) + pow(mZ, 2));
}

Vec3D Vec3D::normalize() {
	return Vec3D(mX/length(), mY/length(), mZ/length());
}

Vec3D Vec3D::multiply(float scalar) {
	return Vec3D(mX*scalar, mY*scalar, mZ*scalar);
}

Vec3D Vec3D::cross(Vec3D other) {
	return Vec3D((mY*other.mZ) - (mZ*other.mY), (mZ*other.mX) - (mX*other.mZ), (mX*other.mY) - (mY*other.mX));
}

Point3D Vec3D::movePoint(Point3D source) {
	return Point3D(source.mX + mX, source.mY + mY, source.mZ + mZ);
}

Vec3D Vec3D::createVector(Point3D p1, Point3D p2) {
	return Vec3D(p2.mX - p1.mX, p2.mY - p1.mY, p2.mZ - p1.mZ);
}

RotationMatrix::RotationMatrix() {}

// based on rotation matrix from wikipedia.
// takes any arbitrary axis and a rotation around that axis.
RotationMatrix::RotationMatrix(Vec3D axis, float angle) {
	this->axis = axis.normalize();
	this->angle = angle;
	this->update();
}

// set the axis for this matrix
void RotationMatrix::setAxis(Vec3D new_axis) {
	this->axis = new_axis.normalize();
	this->update();
}

// set the angle (rotation) for this axis
void RotationMatrix::setAngle(float new_angle) {
	this->angle = new_angle;
	this->update();
}

// compute the matrix which is used to rotate points/vectors in 3d space.
// this is all taken from wikipedia for rotation matrix,
// https://en.wikipedia.org/wiki/Rotation_matrix
void RotationMatrix::update() {
	float ang = (angle * M_PI) / 180;
	float c_ang = cos(ang);
	float s_ang = sin(ang);

	matrix[0][0] = c_ang + (pow(axis.mX, 2) * (1 - c_ang));
	matrix[0][1] = (axis.mX * axis.mY * (1 - c_ang)) - (axis.mZ * s_ang);
	matrix[0][2] = (axis.mX * axis.mZ * (1 - c_ang)) + (axis.mY * s_ang);

	matrix[1][0] = (axis.mY * axis.mX * (1 - c_ang)) + (axis.mZ * s_ang);
	matrix[1][1] = c_ang + (pow(axis.mY, 2) * (1 - c_ang));
	matrix[1][2] = (axis.mY * axis.mZ * (1 - c_ang)) - (axis.mX * s_ang);

	matrix[2][0] = (axis.mZ * axis.mX * (1 - c_ang)) - (axis.mY * s_ang);
	matrix[2][1] = (axis.mZ * axis.mY * (1 - c_ang)) + (axis.mX * s_ang);
	matrix[2][2] = c_ang + (pow(axis.mZ, 2) * (1 - c_ang));
}

// multiply a vector by this rotation matrix
// this is equivalent to rotating the vector by this->angle degrees about the axis this->axis.
Vec3D RotationMatrix::multiply(Vec3D tgt) {
	float newVec[3] = {matrix[0][0]*tgt.mX + matrix[0][1]*tgt.mY + matrix[0][2]*tgt.mZ,
                       matrix[1][0]*tgt.mX + matrix[1][1]*tgt.mY + matrix[1][2]*tgt.mZ,
                       matrix[2][0]*tgt.mX + matrix[2][1]*tgt.mY + matrix[2][2]*tgt.mZ
                      };
    return Vec3D(newVec[0], newVec[1], newVec[2]);
}