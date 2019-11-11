#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include <GLUT/glut.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
  #include <GL/freeglut.h>
#endif

#include "camera.h"
#include "mathLib3D.h"

// a significant amount of 3d camera code was converted from code in
// https://learnopengl.com/Getting-started/Camera
Camera::Camera(Vec3D camPos, Vec3D camTgt) {
	// position of camera
	this->camPos = camPos;
	// point camera is looking towards
	this->camTgt = camTgt;
	// direction camera looks in
	this->camDir = Vec3D(camPos.mX - camTgt.mX, camPos.mY - camTgt.mY, camPos.mZ - camTgt.mZ).normalize();

	// up direction (absolute)
	this->up = Vec3D(0.0, 1.0, 0.0);
	// right vector from the camera
	this->camRight = up.cross(camDir).normalize();
	// up vector from the camera
	this->camUp = camDir.cross(camRight);

	// front of the camera (where it is pointing to/looking)
	this->camFront = Vec3D(camDir.mX, camDir.mY, camDir.mZ).normalize();

	// angles of rotation
	this->pitch = 0.0;
	this->yaw = 0.0;

	// movement speed of the camera
	this->camSpeed = 0.1;
	// rotation speed of the camera
	this->rotSpeed = 0.7;

	// sensitivity default value
	this->sens = 0.06;
}

/**
* Sets up perspective view.
*/
void Camera::setupPerspective() {
	// load projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// set up perspective with 90 fov
	gluPerspective(90, 1.0, 0.1, 1000);
}

/**
* Points the camera towards the viewpoint
*/
void Camera::lookAt() {
	// load modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// look at the point
	gluLookAt (this->camPos.mX, this->camPos.mY, this->camPos.mZ,
		(this->camPos.mX + this->camFront.mX), (this->camPos.mY + this->camFront.mY), (this->camPos.mZ + this->camFront.mZ),
		this->up.mX, this->up.mY, this->up.mZ);
}

/**
* Updates rotation based on x/y mouse movement.
*/
void Camera::updateRotation(float xoff, float yoff) {
	// apply sensitivity to the motion
	xoff *= sens;
	yoff *= sens;

	// adjust rotations
	this->yaw += xoff;
	this->pitch -= yoff;
}

/**
* Applies the rotation to the camera.
*/
void Camera::applyRotation() {
	// pitch is constrained because pitch gets weird outside of (-90, 90)
	// (stuff flips upside down)
	if (pitch > 89.0) pitch = 89.0;
	if (pitch < -89.0) pitch = -89.0;

	// compute the new camFront based on the pitch/yaw angles.
	float mX = cos((M_PI*this->pitch)/180) * cos((M_PI*this->yaw)/180);
	float mY = sin((M_PI*this->pitch)/180);
	float mZ = cos((M_PI*this->pitch)/180) * sin((M_PI*this->yaw)/180);

	this->camFront = Vec3D(mX, mY, mZ).normalize();
}

/**
* Moves the camera some amount (speed) in the indicated direction.
* Movement should be one of CAMERA_MOVE_FORWARD, CAMERA_MOVE_BACKWARD,
* CAMERA_STRAFE_LEFT, CAMERA_STRAFE_RIGHT.
*/
void Camera::applyMovement(int movement, float speed) {
	// each of these corresponds to particular movement on camPos, the vector representing
	// the camera's position.
	// these are applied relative to camFront, because camFront stores the direction the camera is pointing in.

	// first two (forward/back) just move the camera in the direction of camFront,
	// or away from camFront. this is clear, we just move in the direction we're looking.
	// second ones are for strafe left/right, it produces a cross product between the direction
	// camera is pointing in, and direction pointing upwards.
	// this produces a vector perpendicular to both (i.e. pointing left/right of the camera)

	// NOTE: for final project i've commented out the y-components of the movement. this is bc the camera
	// should not be moving up/down freely if it's an object affected by gravity
	switch(movement) {
		case CAMERA_MOVE_FORWARD:
		{
			Vec3D tmp = this->camFront.multiply(speed);
			this->camPos.mX += tmp.mX;
			//this->camPos.mY += tmp.mY;
			this->camPos.mZ += tmp.mZ;
			break;
		}
		case CAMERA_MOVE_BACKWARD:
		{
			Vec3D tmp = this->camFront.multiply(speed);
			this->camPos.mX -= tmp.mX;
			//this->camPos.mY -= tmp.mY;
			this->camPos.mZ -= tmp.mZ;
			break;
		}
		case CAMERA_STRAFE_RIGHT:
		{
			Vec3D tmp = this->camFront.cross(this->camUp).normalize().multiply(speed);
			this->camPos.mX += tmp.mX;
			//this->camPos.mY += tmp.mY;
			this->camPos.mZ += tmp.mZ;
			break;
		}
		case CAMERA_STRAFE_LEFT:
		{
			Vec3D tmp = this->camFront.cross(this->camUp).normalize().multiply(speed);
			this->camPos.mX -= tmp.mX;
			//this->camPos.mY -= tmp.mY;
			this->camPos.mZ -= tmp.mZ;
			break;
		}
	}
}

// just a setter for sens
void Camera::setSensitivity(float sens) {
	this->sens = sens;
}