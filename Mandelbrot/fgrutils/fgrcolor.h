#pragma once

#ifndef __fcolor_h__
#define __fcolor_h__

#include <cmath>

namespace fgr {
	class fcolor { //In this class, all levels are a float from 0 to 1.
	public:
		float R;
		float G;
		float B;
		float A;
		//Correct any levels accidentally being above 1
		void caplevels();
	public:
		fcolor();
		fcolor(float red, float green, float blue, float alpha);
		fcolor(float red, float green, float blue);
		//Accepted components are 'r', 'g', 'b', and 'a'.
		float getLevel(char component) const;
		//Accepted components are 'r', 'g', 'b', and 'a'.
		void setLevel(char component, float level);
		//Accepted components are 'r', 'g', 'b', and 'a'.
		void alterLevel(char component, float level);
		void invert();
		//OPERATORS
		fcolor operator- (const fcolor& other) const;
		fcolor operator+ (const fcolor& other) const;
		fcolor operator* (float other) const;
	};

	//The subtraction operator
	fcolor fcolor::operator- (const fcolor& other) const {
		return fcolor(R - other.R, G - other.G, B - other.B, A - other.A);
	}

	//The addition operator
	fcolor fcolor::operator+ (const fcolor& other) const {
		return fcolor(R + other.R, G + other.G, B + other.B, A + other.A);
	}

	//The multiplication operator
	fcolor fcolor::operator* (float other) const {
		return fcolor(R * other, G * other, B * other, A * other);
	}

	fcolor fcolorInverse(fcolor& col);

	//This function is presently incomplete
	fcolor fcolorFromHSV(float hue, float saturation, float value);

	void fcolor::caplevels() {
		R = fminf(fabsf(R), 1.0);
		G = fminf(fabsf(G), 1.0);
		B = fminf(fabsf(B), 1.0);
		A = fminf(fabsf(A), 1.0);
		//Thanks DP~
	}

	fcolor::fcolor() {
		R = G = B = A = 1.0f;
	}
	fcolor::fcolor(float red, float green, float blue, float alpha) {
		R = red;
		G = green;
		B = blue;
		A = alpha;
		//caplevels();
	}
	fcolor::fcolor(float red, float green, float blue) {
		R = red;
		G = green;
		B = blue;
		A = 1.0f;
		//caplevels();
	}
	//Accepted components are 'r', 'g', 'b', and 'a'.
	float fcolor::getLevel(char component) const {
		if (component == 'r')
			return R;
		if (component == 'g')
			return G;
		if (component == 'b')
			return B;
		if (component == 'a')
			return A;
		return A;
	}


	//Accepted components are 'r', 'g', 'b', and 'a'.
	void fcolor::setLevel(char component, float level) {
		if (component == 'r')
			R = level;
		if (component == 'g')
			G = level;
		if (component == 'b')
			B = level;
		if (component == 'a')
			A = level;
		caplevels();
	}
	//Accepted components are 'r', 'g', 'b', and 'a'.
	void fcolor::alterLevel(char component, float level) {
		if (component == 'r')
			R += level;
		if (component == 'g')
			G += level;
		if (component == 'b')
			B += level;
		if (component == 'a')
			A += level;
		caplevels();
	}
	void fcolor::invert() {
		R = 1 - R;
		G = 1 - G;
		B = 1 - G;
	}

	fcolor fcolorInverse(fcolor& col) {
		col.invert();
		return col;
	}

	//This function is presently incomplete
	fcolor fcolorFromHSV(float hue, float saturation, float value) {
		fcolor col;
		//Incomplete
		return col;
	}


}

#endif