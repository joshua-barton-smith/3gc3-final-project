#ifndef MATHLIB_3D_H
#define MATHLIB_3D_H

#include <math.h>
#include <cmath>

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

class Point3D {
public:
    Point3D();
    Point3D(float inX, float inY, float inZ);
    float mX;
    float mY;
    float mZ;

    float distanceTo(Point3D other);
    float fastDistanceTo(Point3D other);
};

class Vec3D {
public:
    Vec3D();
    Vec3D(float inX, float inY, float inZ);
    float mX;
    float mY;
    float mZ;
    float length();
    Vec3D normalize();
    Vec3D multiply(float scalar);
    Vec3D cross(Vec3D other);
    Point3D movePoint(Point3D source);

    static Vec3D createVector(Point3D p1, Point3D p2);
};

class RotationMatrix {
public:
    RotationMatrix();
    RotationMatrix(Vec3D axis, float angle);

    void setAxis(Vec3D new_axis);
    void setAngle(float new_angle);

    void update();
    Vec3D multiply(Vec3D tgt);

    Vec3D axis;
    float angle;

    float matrix[3][3];
};

#endif
