#pragma once
//Defines the shader class, to simplify loading/compiling/managing shaders
#ifndef __SHADER_H__
#define __SHADER_H__

#include <glew.h>
#include <GL/glut.h>
#include "freeglut_win32_API/freeglut32.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <cassert>

//Reads a text file into a single std::string
std::string fileToString(const std::string& filePath) {
	std::ifstream fin(filePath);
	//Make sure the open was successful
	if (!fin.good()) {
		std::cerr << "Error in fileToString - ERROR READING FILE " + filePath << std::endl;
		return "";
	}
	std::stringstream rets;
	//Read the full file buffer into the stringstream
	rets << fin.rdbuf();
	fin.close();
	return rets.str();
}

//Return just the file name, given a full file path
std::string nameFromPath(const std::string& fpath) {
	for (int i = fpath.size() - 1; i > 0; --i) {
		if (fpath[i] == '\\') {
			if (i == fpath.size() - 1) {
				std::cerr << "Error in nameFrompath - File path '" << fpath << "' ends in \\ character." << std::endl;
				return fpath;
			}
			return fpath.substr(i+1);
		}
	}
	std::cerr << "Error in nameFromPath - no \\ characters in file path '" << fpath << "'." << std::endl;
	return fpath;
}

//Edit a directory to go up a level
std::string upLevelDirectory(const std::string& fpath) {
	std::string rets = fpath;
	do {
		rets.pop_back();
	} while (rets.back() != '\\' && rets.size());
	if (rets.back() == '\\') rets.pop_back();
	if (!rets.size()) {
		std::cerr << "Error in upLevelDirectory - file path lacking \\ character";
		return fpath;
	}
	return rets;
}

//Return the file extention from a file name or path
std::string getFileExtention(const std::string& fpath) {
	//Seach for a '.' character from the end of the string
	for (int i = fpath.size() - 1; i >= 0; --i) {
		if (fpath[i] == '.') {
			return fpath.substr(i + 1);
		}
	}
	//Return a blank string if no extention is found
	return "";
}

//Const map that maps specific GLSL file extentions to the available shader types
const std::map<std::string, GLenum> shaderTypeMap = {
	{"vert", GL_VERTEX_SHADER},
	{"frag", GL_FRAGMENT_SHADER},
	{"geom", GL_GEOMETRY_SHADER},
	{"comp", GL_COMPUTE_SHADER},
	{"tesc", GL_TESS_CONTROL_SHADER},
	{"tese", GL_TESS_EVALUATION_SHADER}
};

//Returns the GLenum shader type for the file with extention if it can be found,
//returning 0 if the extention isn't recognized
GLenum guessShaderType(const std::string& fileName) {
	std::string ext = getFileExtention(fileName);
	//Return 0 if there is no extention
	if (!ext.size()) return 0;
	//Search the shaderTypeMap for the extention
	std::map<std::string, GLenum>::const_iterator itr = 
		shaderTypeMap.find(ext);
	//Return 0 if the extention is not mappable to a shader type
	if (itr == shaderTypeMap.cend()) return 0;
	//Otherwise, return the mapping
	return itr->second;
}

//Inspired by https://learnopengl.com/Getting-started/Shaders
//Class represents a 'program', consistening of a vertex shader and a fragment shader
class ShaderProgram {
private:
	// STATIC (OR STATIC-RELATED) MEMBERS
	//Global shaders directory
	static std::string my_shaders_directory;
	//Vector of pointers to all ShaderProgram objects ever created
	static std::vector<ShaderProgram*> all_ShaderPrograms_vec;
	//Index of this particular shader in the vector
	std::size_t myShaderProgramIndex;

	// REPRESENTATION
	//The ID of this program in OpenGL
	GLuint ID = 0;
	//Set to true if there were compilation/linking errors
	bool badErrors = false;
	//Remember every filepath passed to this shader, and its associated shader type
	std::vector<std::pair<std::string,GLenum>> sourcePaths;
	//The string used for ALL infolog output. Changes frequently.
	char infoLog[512];


	// PRIVATE METHODS
	//Print the info log
	void printInfolog() const {
		std::cout << infoLog << std::endl;
	}
	//Add a shader of the specified type to the program
	bool addShaderSource(const char* source, GLenum type, bool log_errors = true) {
		GLint success;
		GLuint S = glCreateShader(type);
		//Compile the shader
		glShaderSource(S, 1, &source, NULL);
		glCompileShader(S);
		//Check for compile errors
		glGetShaderiv(S, GL_COMPILE_STATUS, &success);
		if (!success && log_errors) {
			glGetShaderInfoLog(S, 512, NULL, infoLog);
		}
		//Attach the shader
		glAttachShader(ID, S);
		//Flag the shader for deletion
		glDeleteShader(S);
		//Return the success indicator
		return success;
	}
	//Finally link the program (add no more shaders once this is done)
	bool linkProgram(bool log_errors = true) {
		GLint success;
		//Link the program
		glLinkProgram(ID);
		//Check for linking errors
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success && log_errors) {
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
		}
		//Return the success indicator
		return success;
	}
	//Initialize this shader object from the filepaths it has been passed
	bool initialize(bool verbose = true) {
		bool success;
		bool anyBad = false;
		ID = glCreateProgram();
		if (verbose) {
			std::cerr << "Initializing ShaderProgram " << ID << std::endl;
		}
		std::string source_code;
		//Iterate through all specified source files
		for (unsigned int i = 0; i < sourcePaths.size(); ++i) {
			//If the filepath contains to \\ characters, add the source path
			if (std::count(sourcePaths[i].first.begin(), sourcePaths[i].first.end(), '\\')) {
				source_code = fileToString(sourcePaths[i].first);
			}
			else {
				source_code = fileToString(my_shaders_directory + "\\" + sourcePaths[i].first);
			}
			//If it fails to open once, try again with the added source path
			//Check openability/emptiness
			if (!source_code.size()) {
				std::cerr << "ERROR - Un-openable or empty source file: " + sourcePaths[i].first << std::endl;
				continue;
			}
			//If the source code exists, attempt to add it
			success = addShaderSource(source_code.c_str(), sourcePaths[i].second);
			if (!success) {
				anyBad = true;
				if (verbose) {
					std::cerr << sourcePaths[i].first << " failed to compile: " << std::endl;
					printInfolog();
				}
			}
			else {
				if (verbose) {
					std::cerr << sourcePaths[i].first << " compiled successfully." << std::endl;
				}
			}
		}
		//Now attempt to link the program
		success = linkProgram();
		if (!success) {
			anyBad = true;
			if (verbose) {
				std::cerr << "ShaderProgram " << ID << " failed to link:" << std::endl;
				printInfolog();
			}
		}
		else {
			if (verbose) {
				std::cerr << "ShaderProgram " << ID << " linked successfully." << std::endl;
			}
		}
		//Return true if all efforts succeeded, false otherwise.
		return !anyBad;
	}
public:
	// INITIALIZATION
	//STATIC INITIALIZATION: Initialize all ShaderPrograms
	static void initializeAllShaderPrograms(bool verbose = true) {
		my_shaders_directory = upLevelDirectory(glut32::getExecutableDirectory_s()) + "\\Shaders";
		//Iterate through entire vector
		for (unsigned int i = 0; i < all_ShaderPrograms_vec.size(); ++i) {
			all_ShaderPrograms_vec[i]->initialize(verbose);
		}
	}
	// USER SOURCE CONTROL
	//Add a source file to this shader
	void addSourceFile(const std::string& sourcePath, GLenum shaderType = 0) {
		//If no type is specified, try to guess it
		if (shaderType == 0) shaderType = guessShaderType(sourcePath);
		//If the type cannot be inferred and was not specified, abort
		if (shaderType == 0) {
			assert(0 && "Error - cannot infer shader type");
		}
		//If we've made it this far, add this source file to this object
		sourcePaths.push_back(std::make_pair(sourcePath,shaderType));
	}
	// CONSTRUCTORS
	//Default constructor
	ShaderProgram() {
		ID = 0;
		myShaderProgramIndex = all_ShaderPrograms_vec.size();
		all_ShaderPrograms_vec.push_back(this);
	}
	//Construct from two file paths
	ShaderProgram(const std::string& vSourcePath, const std::string& fSourcePath) : ShaderProgram() {
		addSourceFile(vSourcePath, GL_VERTEX_SHADER);
		addSourceFile(fSourcePath, GL_FRAGMENT_SHADER);
	}
	//Construct from an array of file paths
	ShaderProgram(const std::vector<std::string>& source_paths) : ShaderProgram() {
		for (unsigned int i = 0; i < source_paths.size(); ++i) {
			addSourceFile(source_paths[i]);
		}
	}

	// METHODS
	//Begin using this program
	void Use() const {
		glClearError();
		glUseProgram(ID);
		glCheckError2();
	}
	//Stop using this program (or whichever program is in use!)
	void unUse() const {
		glUseProgram(0);
	}
	//Print the program info log
	void printProgramInfoLog(std::ostream& dest = std::cerr) const {
		char infoLog[512];
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		dest << infoLog << std::endl;
	}
	//Check if this program is valid (computationally expensive!!! only call for debug)
	bool isValid(bool print_output = false) const {
		glValidateProgram(ID);
		int valid;
		glGetProgramiv(ID, GL_VALIDATE_STATUS, &valid);
		if (print_output) printProgramInfoLog();
		return valid;
	}
	//Get a property of this program using a glEnum
	int queryInt(GLenum prop) {
		int reti;
		glGetProgramiv(ID, prop, &reti);
		return reti;
	}
	//Check whether the program is flagged for deletion
	int getDeleteStatus() { return queryInt(GL_DELETE_STATUS); }
	//Check whether the program is linked
	int getLinkStatus() { return queryInt(GL_LINK_STATUS); }
	//Get the number of attached shader objects
	int getAttachedShaders() { return queryInt(GL_ATTACHED_SHADERS); }
	//Returns the number of active attributes
	int getActiveAttributes() { return queryInt(GL_ACTIVE_ATTRIBUTES); }
	//Returns the number of active uniforms
	int getActiveUniforms() { return queryInt(GL_ACTIVE_UNIFORMS); }
	//Returns the length (in bytes) of the program binary
	int getProgramBinaryLength() { return queryInt(GL_PROGRAM_BINARY_LENGTH); }
	//Returns the location of a particular uniform value as a GLint
	GLint getUniformLocationID(const char* uName) {
		return glGetUniformLocation(ID, uName);
	}

	// MODIFIERS
	//Set uniforms pertaining to this shader
	void setBool(const char* name, bool value) {
		glClearError();
		glUniform1i(getUniformLocationID(name), value);
		glCheckError(name);
	}
	void setInt(const char* name, int value) {
		glClearError();
		glUniform1i(getUniformLocationID(name), value);
		glCheckError(name);
	}
	void setFloat(const char* name, float value) {
		glClearError();
		glUniform1f(getUniformLocationID(name), value);
		glCheckError(name);
	}
	void setVec2(const char* name, float v0, float v1) {
		glClearError();
		glUniform2f(getUniformLocationID(name), v0, v1);
		glCheckError(name);
	}
	void setVec3(const char* name, float v0, float v1, float v2) {
		glClearError();
		glUniform3f(getUniformLocationID(name), v0, v1, v2);
		glCheckError(name);
	}
	//Version of setVec3 that can take a &point3D.x, for example
	void setVec3(const char* name, const float* v) {
		glClearError();
		glUniform3f(getUniformLocationID(name), v[0], v[1], v[2]);
		glCheckError2();
	}
	void setVec4(const char* name, float v0, float v1, float v2, float v3) {
		glClearError();
		glUniform4f(getUniformLocationID(name), v0, v1, v2, v3);
		glCheckError2();
	}

	// DELETED OPERATORS
	//Delete assignment operator
	ShaderProgram& operator=(const ShaderProgram& other) = delete;

	// DECONSTRUCTOR
	//Delete this program when it goes out of scope
	~ShaderProgram() {
		//Delete the program from the GPU (or anyway flag it for deletion)
		if (ID != 0)
			glDeleteProgram(ID);
	}

};

std::vector<ShaderProgram*> ShaderProgram::all_ShaderPrograms_vec;
std::string ShaderProgram::my_shaders_directory;

//Namespace for defining all our shaders
namespace ShaderPrograms {
	//Mandelbrot fractal shader
	ShaderProgram MandelbrotShader({ "Rectangle.vert","Mandelbrot.frag" });
	ShaderProgram JuliaShader({ "Rectangle.vert","Julia.frag" });
	ShaderProgram BurningShipShader({ "Rectangle.vert","BurningShip.frag" });
	ShaderProgram BurningJuliaShader({ "Rectangle.vert","BurningJulia.frag" });
	ShaderProgram MiscShader1Shader({ "Rectangle.vert","MiscShader1.frag" });
}

#endif
