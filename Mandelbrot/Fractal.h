#pragma once

#ifndef __FRACTAL_H__
#define __FRACTAL_H__

#include "Shaders.h"
#include <fstream>

#define FRACTYPE_MANDELBROT 0
#define FRACTYPE_JULIA 1

#define FRACCOLOR_ITERATIONS 0
#define FRACCOLOR_FINALSTATE 1

//Create a file concatenating the contents of two other files
std::string concatFiles(const std::string& headerPath, const std::string& bodyPath, const std::string& headPath, const std::string& tailpath, const std::string& name) {
	std::string raw_text = fileToString(headerPath) + fileToString(bodyPath) + fileToString(headPath) + fileToString(tailpath);
	std::string name2 = glut32::getExecutableDirectory_s() + "\\" + name;
	std::ofstream fout(name2, std::ofstream::out);
	if (!fout.good()) {
		std::cerr << "Error in concatFiles - COULD NOT WRITE TO FILE " << name2 << std::endl;
	}
	fout << raw_text;
	fout.close();
	return name2;
}

//Delete a file... use with care!
void myDeleteFile(const std::string& removePath) {
	std::remove(removePath.c_str());
}

//A Fractal object consists of four shaders that use the same iteration function
class FractalObject {
public:
	// STATIC PROPERTIES
	//Individual fractal source directory
	static std::string my_fractals_directory;
	//Global fractal source directory
	static std::string my_global_source_directory;
	//Vector of pointers to all ShaderProgram objects ever created
	static std::vector<FractalObject*> all_FractalObjects_vec;
	//Index of this particular shader in the vector
	std::size_t myFractalObjectIndex;

	// STATIC SOURCE CODE
	//Common header for all fractal programs
	static const std::string CommonHeaderPath;
	//Head for Mandelbrot programs
	static const std::string MandelbrotHeadPath;
	//Head for Julia programs
	static const std::string JuliaHeadPath;
	//Tail for Iteration-colored programs
	static const std::string IterationTailPath;
	//Tail for Finalstate-colored programs
	static const std::string FinalstateTailPath;

	//Head for Orbit-Tracing program
	static const std::string OrbitTraceHeadPath;
	//Tail for Orbit-Tracing program
	static const std::string OrbitTraceTailPath;
private:
	// SHADER REPRESENTATION
	//Mandelbrot, Iteration-Colored
	ShaderProgram MandelbrotIterationProgram;
	//Julia, Iteration-Colored
	ShaderProgram JuliaIterationProgram;
	//Mandelbrot, Finalstate-Colored
	ShaderProgram MandelbrotFinalstateProgram;
	//Julia, Finalstate-Colored
	ShaderProgram JuliaFinalstateProgram;

	//Orbit-Tracing program
	ShaderProgram OrbitTraceProgram;

	//Path to user-supplied sourcecode
	std::string user_source_path;

	//Path to temporary files containing full source
	std::string temp_MandelbrotIterationPath;
	std::string temp_JuliaIterationPath;
	std::string temp_MandelbrotFinalstatePath;
	std::string temp_JuliaFinalstatePath;

	std::string temp_OrbitTracePath;

public:

	// CONSTRUCTOR
	FractalObject(const std::string& sourcePath) {
		//Set ID and put address in the global vector
		myFractalObjectIndex = all_FractalObjects_vec.size();
		all_FractalObjects_vec.push_back(this);
		user_source_path = sourcePath;
		//Concatenate all the source codes into temp files
		std::string gs = my_global_source_directory;
		temp_MandelbrotIterationPath = concatFiles(gs+CommonHeaderPath, my_fractals_directory + user_source_path, gs+MandelbrotHeadPath, gs+IterationTailPath,"fractal_temp\\Temp_MI_" + std::to_string(myFractalObjectIndex) + ".frag");
		temp_JuliaIterationPath = concatFiles(gs+CommonHeaderPath, my_fractals_directory + user_source_path, gs+JuliaHeadPath, gs+IterationTailPath,"fractal_temp\\Temp_JI_" + std::to_string(myFractalObjectIndex) + ".frag");
		temp_MandelbrotFinalstatePath = concatFiles(gs+CommonHeaderPath, my_fractals_directory + user_source_path, gs+MandelbrotHeadPath, gs+FinalstateTailPath,"fractal_temp\\Temp_MF_" + std::to_string(myFractalObjectIndex) + ".frag");
		temp_JuliaFinalstatePath = concatFiles(gs+CommonHeaderPath, my_fractals_directory + user_source_path, gs+JuliaHeadPath, gs+FinalstateTailPath,"fractal_temp\\Temp_JF_" + std::to_string(myFractalObjectIndex) + ".frag");
		temp_OrbitTracePath = concatFiles(gs+OrbitTraceHeadPath, my_fractals_directory + user_source_path, "", gs+OrbitTraceTailPath,"fractal_temp\\Temp_OT_" + std::to_string(myFractalObjectIndex) + ".geom");
		//Set up the shader code
		MandelbrotIterationProgram.addSourceFile("Rectangle.vert");
		MandelbrotIterationProgram.addSourceFile(temp_MandelbrotIterationPath);
		JuliaIterationProgram.addSourceFile("Rectangle.vert");
		JuliaIterationProgram.addSourceFile(temp_JuliaIterationPath);
		MandelbrotFinalstateProgram.addSourceFile("Rectangle.vert");
		MandelbrotFinalstateProgram.addSourceFile(temp_MandelbrotFinalstatePath);
		JuliaFinalstateProgram.addSourceFile("Rectangle.vert");
		JuliaFinalstateProgram.addSourceFile(temp_JuliaFinalstatePath);
		OrbitTraceProgram.addSourceFile("OrbitTrace.vert");
		OrbitTraceProgram.addSourceFile(temp_OrbitTracePath);
		OrbitTraceProgram.addSourceFile("OrbitTrace.frag");
	}

	// DESTRUCTOR
	~FractalObject() {
		//Delete the temp files associated with this object
		myDeleteFile(temp_MandelbrotIterationPath);
		myDeleteFile(temp_JuliaIterationPath);
		myDeleteFile(temp_MandelbrotFinalstatePath);
		myDeleteFile(temp_JuliaFinalstatePath);
		all_FractalObjects_vec.erase(all_FractalObjects_vec.begin() + myFractalObjectIndex);
	}


	//Get a pointer to the appropriate ShaderProgram
	ShaderProgram* getProgram(int fracType, int colorMethod) {
		switch (fracType) {
		case FRACTYPE_MANDELBROT:
			switch (colorMethod) {
			case FRACCOLOR_ITERATIONS:
				return &MandelbrotIterationProgram;
			case FRACCOLOR_FINALSTATE:
				return &MandelbrotFinalstateProgram;
			}
			break;
		case FRACTYPE_JULIA:
			switch (colorMethod) {
			case FRACCOLOR_ITERATIONS:
				return &JuliaIterationProgram;
			case FRACCOLOR_FINALSTATE:
				return &JuliaFinalstateProgram;
			}
			break;
		}
	}

	//Get a pointer to the orbit-tracing program
	ShaderProgram* getOrbitTraceProgram() {
		return &OrbitTraceProgram;
	}

	//Use this fractal
	void Use(int fracType, int colorMethod) {
		getProgram(fracType, colorMethod)->Use();
	}

	//Unuse this fractal
	void unUse() const { glUseProgram(0); }

	//Get the name of the user code for this fractal
	const std::string& getName() const { return user_source_path; }

};

std::vector<FractalObject*> FractalObject::all_FractalObjects_vec;
std::string FractalObject::my_fractals_directory;
std::string FractalObject::my_global_source_directory;

//Common header for all fractal programs
const std::string FractalObject::CommonHeaderPath = "Common_header.glsl";
//Head for Mandelbrot programs
const std::string FractalObject::MandelbrotHeadPath = "Mandelbrot_head.glsl";
//Head for Julia programs
const std::string FractalObject::JuliaHeadPath = "Julia_head.glsl";
//Tail for Iteration-colored programs
const std::string FractalObject::IterationTailPath = "Iteration_tail.glsl";
//Tail for Finalstate-colored programs
const std::string FractalObject::FinalstateTailPath = "Finalstate_tail.glsl";

//Head for Orbit-Tracing program
const std::string FractalObject::OrbitTraceHeadPath = "OrbitTrace_head.glsl";
//Tail for Orbit-Tracing program
const std::string FractalObject::OrbitTraceTailPath = "OrbitTrace_tail.glsl";

#endif 

