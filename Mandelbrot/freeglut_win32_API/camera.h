#pragma once
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <cmath>
#include "gl/glut.h"
#include "coordinates.h"


//Places a glVertex3f at the location of a 3D point
void glVertexPoint(const point3D& pt) {
	glVertex3f(pt.x, pt.y, pt.z);
}

//Preforms a glTranslatef to a 3D point
void glTranslatePoint(const point3D& pt) {
	glTranslatef(pt.x, pt.y, pt.z);
}


//Class for storing perspective by way of the point one is looking at, and the orbital position relative to it
class camera {
public:
	//Members
	//Point the camera is looking at
	point3D look;
	//Horizontal angle to camera
	float theta = 0;
	//Azimuthal angle to camera
	float phi = 0;
	//Distance from perspective to point
	float rho = 1;

	//Default constructor
	camera() {
		look = point3D(0, 0, 0);
		theta = phi = 0;
		rho = 1;
	}

	//Thorough constructor
	camera(float xval, float yval, float zval, float phival, float thetaval, float rhoval) {
		look = point3D(xval, yval, zval);
		theta = thetaval;
		phi = phival;
		rho = rhoval;
	}

	//Point-angle constructor
	camera(point3D lookval, float phival, float thetaval, float rhoval) {
		look = lookval;
		theta = thetaval;
		phi = phival;
		rho = rhoval;
	}

	//Methods
	//Return the relative position of the camera to its focus as a point
	point3D relpos() const {
		return point3D(std::cosf(phi)*std::cosf(theta), std::sinf(phi)*std::cosf(theta), std::sinf(theta)) * rho;
	}
	//Return the position of the camera itself
	point3D abspos() const {
		return look + relpos();
	}
	//Return rho-unit-vector, which points outwards
	point3D rhoHat() const {
		return relpos().unit();
	}
	//Return the phi-unit-vector, which points along a line of latitude
	point3D phiHat() const {
		return point3D(-std::sinf(phi), std::cosf(phi), 0);
	}
	//Return the theta-unit-vector, which points upwards along a line of longitude
	point3D thetaHat() const {
		return rhoHat() * phiHat();
	}

	//Calls gluLookAt in such a way as to use the current camera as the new drawing perspective
	void gluLook() const {
		point3D eye = abspos();
		//point3D up = thetaHat();
		gluLookAt(eye.x, eye.y, eye.z, look.x, look.y, look.z, 0, 0, 1);
		return;
	}

	// Some user-friendly camera motion
	//Pans the camera in the plane normal to the direction of viewing
	void panMe(float X, float Y) {
		look += phiHat() * X + thetaHat() * Y;
	}
	//Orbits the camera about the looking point by a certain number of radians in each direction
	void orbitMe(float Phi, float Theta) {
		phi += Phi;
		theta += Theta;
		//Check if horizontal orbit is higher than north pole or lower than south pole
		if (theta > fgr::PI / 2 - 0.001f) {
			theta = fgr::PI / 2 - 0.001f;
		}
		if (theta < -fgr::PI / 2 + 0.001f) {
			theta = -fgr::PI / 2 + 0.001f;
		}
		//Clock arithmatic to ensure phi is between 0 and 2pi
		if (phi < 0 || phi > 2 * fgr::PI) {
			phi = fmodf(phi, 2 * fgr::PI);
		}
	}
	//Return a panned version of this camera without panning this one directly
	camera pan(float X, float Y) const {
		camera retcam = *this;
		retcam.panMe(X, Y);
		return retcam;
	}
	//Return an orbited version of this camera without orbiting this one directly
	camera orbit(float Phi, float Theta) const {
		camera retcam = *this;
		retcam.orbitMe(Phi, Theta);
		return retcam;
	}
	//Zoom in the camera by a certain percentage
	void zoomMe(float ratio) {
		rho /= ratio;
		return;
	}
	//Returns the angle, in radians, of the eye from the x axis about the y.
	float eyeAngle() const {
		return std::asinf(abspos().z/std::sqrtf(abspos().x*abspos().x+abspos().z*abspos().z));
	}
};

#endif
