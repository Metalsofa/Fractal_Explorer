#pragma once
#ifndef __GRADIENTS_H__
#define __GRADIENTS_H__
#include <vector>
#include "fgrcolor.h"

typedef std::vector < std::pair<float, fgr::fcolor> > gradient;

//A gradient is assumed to have its frist t = 0
fgr::fcolor mapgradient(float t, const gradient& colorscheme) {
	//assert(t >= 0);
	unsigned int i = 0;
	while (i < colorscheme.size() && colorscheme[i].first <= t)
		++i;
	if (i >= colorscheme.size())
		return colorscheme.back().second;
	fgr::fcolor diffc(colorscheme[i].second - colorscheme[i - 1].second);
	float difft = (colorscheme[i].first) - (colorscheme[i - 1].first);
	t -= colorscheme[i - 1].first;
	float frac = t / difft;
	fgr::fcolor retfc = colorscheme[i - 1].second + diffc * frac;
	return retfc;
}

////The only gradient colorscheme we'll need for this application
//const gradient defaultGradient = {
//	{ 0.00, fgr::fcolor(1.0f, 1.0f, 1.0f, 0.00f) },
//	{ 0.25, fgr::fcolor(0.0f, 0.0f, 1.0f, 0.25f) },
//	{ 0.37, fgr::fcolor(0.0f, 1.0f, 1.0f, 0.37f) },
//	{ 0.50, fgr::fcolor(0.0f, 1.0f, 0.0f, 0.50f) },
//	{ 0.70, fgr::fcolor(1.0f, 1.0f, 0.0f, 0.70f) },
//	{ 0.89, fgr::fcolor(1.0f, 0.0f, 0.0f, 0.89f) },
//	{ 1.00, fgr::fcolor(0.9f, 0.0f, 0.0f, 1.00f) }
//};

//The only gradient colorscheme we'll need for this application
const gradient defaultGradient = {
	{ 0.00, fgr::fcolor(1.0f, 1.0f, 1.0f, 0.00f) }, // 0 - WHITE
	//{ 0.12, fgr::fcolor(0.5f, 0.5f, 1.0f, 0.12f) }, // D - LIGHT BLUE
	{ 0.25, fgr::fcolor(0.0f, 0.0f, 1.0f, 0.25f) }, // DRL - BLUE
	{ 0.37, fgr::fcolor(0.0f, 1.0f, 1.0f, 0.37f) }, // C - CYAN
	{ 0.50, fgr::fcolor(0.0f, 1.0f, 0.0f, 0.50f) }, // SSL - GREEN
	{ 0.70, fgr::fcolor(1.0f, 1.0f, 0.0f, 0.70f) }, // B - YELLOW
	{ 0.89, fgr::fcolor(1.0f, 0.5f, 0.0f, 0.89f) }, // ARL - ORANGE
	{ 1.00, fgr::fcolor(1.0f, 0.0f, 0.0f, 1.00f) }  // A - RED
};

//A simple red-only gradient
const gradient redGradient = {
	{0.0f, fgr::fcolor(1.0f, 0.0f, 0.0f, 0.00f) },
	{1.0f, fgr::fcolor(1.0f, 0.0f, 0.0f, 1.00f) },
};

//A Very Cool gradient
const gradient nornGradient = {
	//{ 0.00f, fgr::fcolor(1.0f, 1.0f, 1.0f, 0.00f) }, // 0
	{ 0.00f, fgr::fcolor(0.5f, 0.5f, 0.5f, 0.00f) }, // DRL
	{ 0.25f, fgr::fcolor(1.0f, 1.0f, 1.0f, 0.25f) }, // DRL
	{ 0.50f, fgr::fcolor(1.0f, 1.0f, 1.0f, 0.50f) }, // SSL
	{ 0.89f, fgr::fcolor(3.0f, 0.3f, 0.3f, 0.89f) }, // ARL
	{ 1.00f, fgr::fcolor(1.0f, 0.0f, 0.0f, 1.00f) }  // A - RED
};

//A simple red-only gradient
const gradient redBand = {
	{0.0f, fgr::fcolor(1.0f, 0.0f, 0.0f, 0.00f) },
	{0.20f, fgr::fcolor(1.0f, 0.0f, 0.0f, 0.00f) },
	{0.23f, fgr::fcolor(1.0f, 0.0f, 0.0f, 1.00f) },
	{0.25f, fgr::fcolor(1.0f, 0.0f, 0.0f, 1.00f) },
	{0.27f, fgr::fcolor(1.0f, 0.0f, 0.0f, 1.00f) },
	{0.30f, fgr::fcolor(1.0f, 0.0f, 0.0f, 0.00f) },
	{1.0f, fgr::fcolor(1.0f, 0.0f, 0.0f, 0.00f) },
};

//Resolution of the compiled gradient approximation
const unsigned int compiledGradientResolution = 256;
//Save on texture-creation time by using this cached approximation of the function
std::vector<fgr::fcolor> compiledGradient;

//Compile the gradient cache
void compileGradient() {
	compiledGradient.resize(compiledGradientResolution);
	for (unsigned int i = 0; i < compiledGradient.size(); ++i) {
		compiledGradient[i] = mapgradient(float(i) / compiledGradientResolution, defaultGradient);
	}
	return;
}

//Efficient approximation for mapgradient that uses the compiled gradient
void mapgradientEfficient(float t, fgr::fcolor* retc) {
	//if (t < 0.25) {
	//	retc->A = 0.0f;
	//	return;
	//}
	int i(t * compiledGradientResolution);
	if (i > compiledGradientResolution-1) i = compiledGradientResolution-1;
	*retc = compiledGradient[i];
	return;
}

//Create and return the name of a texture compiling a given gradient
GLuint makeGradientTexture(const gradient& grad, unsigned int resolution) {
	//Return this at the end
	GLuint reti;
	//Calculate the data
	std::vector<fgr::fcolor> texdat(resolution);
	for (unsigned int i = 0; i < texdat.size(); ++i) {
		texdat[i] = mapgradient(float(i+0.5f) / resolution, grad);
	}
	//Generate the texture
	glGenTextures(1, &reti);
	//Bind to the texture so we can operate on it
	glBindTexture(GL_TEXTURE_1D, reti);
	//Some texture settings
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//Read the data into the texture
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, resolution, 0, GL_RGBA, GL_FLOAT, texdat.data());
	//Done
	glBindTexture(GL_TEXTURE_1D, 0);
	//Return the texture name
	return reti;
}

namespace Gradients {
	//Cyclic
	const gradient rainbow = {
		{ 0.0 / 7.0, fgr::fcolor(0.0, 0.0, 0.0)},
		{ 1.0 / 7.0, fgr::fcolor(1.0, 0.0, 0.0)},
		{ 2.0 / 7.0, fgr::fcolor(1.0, 1.0, 0.0)},
		{ 3.0 / 7.0, fgr::fcolor(0.0, 1.0, 0.0)},
		{ 4.0 / 7.0, fgr::fcolor(0.0, 1.0, 1.0)},
		{ 5.0 / 7.0, fgr::fcolor(0.0, 0.0, 1.0)},
		{ 6.0 / 7.0, fgr::fcolor(1.0, 0.0, 1.0)},
		{ 7.0 / 7.0, fgr::fcolor(1.0, 0.0, 0.0)},
	};

	//Cyclic
	const gradient twilight = {
		{ 0.0 / 6.0, fgr::fcolor(0.1f, 0.0, 0.3f)}, // Dark violet
		{ 1.0 / 6.0, fgr::fcolor(0.5f, 0.0, 0.8f)}, // Lighet violet
		{ 2.0 / 6.0, fgr::fcolor(0.6f, 0.6f, 0.8f)}, // Lighet blue
		{ 3.0 / 6.0, fgr::fcolor(0.8f, 0.8f, 0.8f)}, // lighet gtrey
		{ 4.0 / 6.0, fgr::fcolor(0.8f, 0.4f, 0.1f)}, // light bronze
		{ 5.0 / 6.0, fgr::fcolor(1.0, 0.5f, 0.5f)}, //putrply bronze
		{ 6.0 / 6.0, fgr::fcolor(0.1f, 0.0, 0.3f)}  // Dark violet
	};

	//Cyclic
	const gradient cyanic = {
		{0.0 / 6.0, fgr::fcolor(0.0, 0.0, 0.0)},
		{1.0 / 6.0, fgr::fcolor(0.0, 0.0, 1.0)},
		{2.0 / 6.0, fgr::fcolor(0.0, 1.0, 1.0)},
		{3.0 / 6.0, fgr::fcolor(1.0, 1.0, 1.0)},
		{4.0 / 6.0, fgr::fcolor(0.0, 1.0, 1.0)},
		{5.0 / 6.0, fgr::fcolor(0.0, 1.0, 0.0)},
		{6.0 / 6.0, fgr::fcolor(0.0, 0.0, 0.0)}
	};

	//Cyclic
	const gradient blood = {
		{0.0 / 3.0, fgr::fcolor(0.0, 0.0, 0.0)},
		{1.0 / 3.0, fgr::fcolor(0.4, 0.2, 0.0)},
		{2.0 / 3.0, fgr::fcolor(1.0, 1.0, 1.0)},
		{3.0 / 3.0, fgr::fcolor(0.0, 0.0, 0.0)}
	};

	//Cyclic
	const gradient noir = {
		{0.0 / 5.0, fgr::fcolor(0.0, 0.0, 0.0)},
		{1.0 / 5.0, fgr::fcolor(0.4, 0.0, 0.0)},
		{2.0 / 5.0, fgr::fcolor(0.0, 0.0, 0.0)},
		{3.0 / 5.0, fgr::fcolor(1.0, 1.0, 1.0)},
		{4.0 / 5.0, fgr::fcolor(0.0, 0.0, 0.0)},
		{5.0 / 5.0, fgr::fcolor(1.0, 1.0, 1.0)}
	};

	std::map<std::string, GLuint> gradTextures;

	std::map<std::string, GLuint>::iterator gradTexItr;

#define initGrad(name) gradTextures[#name] = makeGradientTexture(name, compiledGradientResolution);

	void initGradients() {
		glClearError();
		initGrad(rainbow);
		initGrad(twilight);
		initGrad(cyanic);
		initGrad(blood);
		initGrad(noir);
		glCheckError("Finished generating texture gradients");
		gradTexItr = gradTextures.begin();
	}
}

#endif
