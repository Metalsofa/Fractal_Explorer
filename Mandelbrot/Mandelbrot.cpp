// Mandelbrot.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include "freeglut_win32_API/freeglut32.h"
#include "Shaders.h"
#include "Fractal.h"
#include "gradients.h"

//Globals
int windowHeight;
int windowWidth;

int mousedownx;
int mousedowny;

int mouseupx;
int mouseupy;

int mouseactivex;
int mouseactivey;

float zoomfactor = 1.2f;
float base_width = 4.0f;
float base_height = 1.0f;


bool mouseleftdown = false;
bool mouserightdown = false;
bool shiftdown = false;

bool consoleListen = false;

//Save a view you found particularly cool
class ViewingState {
public:
	//Camera
	float center_x = 0.0f;
	float center_y = 0.0f;
	float zoom = 1.0f;
	//Rendering
	float param_x = 0.0f;
	float param_y = 0.0f;
	int max_iterations = 16;
	//Which fractal is currently in use
	int currentFractal = 0;
	//Which type of fractal plotting is in use
	int plot_type = 0;
	//Which coloring method is in use
	int color_style = 0;
	//Colorscheme currently in use
	std::map<std::string,GLuint>::iterator colorscheme;

	//Temporary variables
	float temp_center_x = 0.0f;
	float temp_center_y = 0.0f;

	// MEDTHODS
	//Print to an output stream
	void print(std::ostream& dest) {
		dest << "REPORT: " << std::endl;
		dest << "center_x: " << center_x << std::endl;
		dest << "center_y: " << center_y << std::endl;
		dest << "zoom: " << zoom << std::endl;
		dest << "base_width: " << base_width << std::endl;
		dest << "base_height: " << base_height << std::endl;
		dest << "param_x: " << param_x << std::endl;
		dest << "param_y: " << param_y << std::endl;
		dest << "max_iterations: " << max_iterations << std::endl;
		dest << "source: " << FractalObject::all_FractalObjects_vec[currentFractal]->getName() << std::endl;
		dest << "plot_type: " << ((plot_type) ? ("Julia") : ("C-Map")) << std::endl;
		dest << "color_style: " << ((color_style) ? ("Final z") : ("Iteration")) << std::endl;
		dest << "colorscheme: " << colorscheme->first << std::endl;
	}
	//Returns updated center positions
	float updatedCenterX() const { return center_x + temp_center_x; }
	float updatedCenterY() const { return center_y + temp_center_y; }
	//Boundary calculations
	float leftBoundary() const { return updatedCenterX() - (base_width / zoom) / 2; }
	float rightBoundary() const { return updatedCenterX() + (base_width / zoom) / 2; }
	float topBoundary() const { return updatedCenterY() + (base_height / zoom) / 2; }
	float bottomBoundary() const { return updatedCenterY() - (base_height / zoom) / 2; }
	//Convert window coordinates to fractal ones
	float winToFracX(int x) const { return (float(x)/windowWidth * (base_width / zoom)+leftBoundary()); }
	float winToFracY(int y) const { return (float(y)/windowHeight * (base_height / zoom)+bottomBoundary()); }
	//Convert fractal coordinates to window ones
	int fracToWinX(float x) const { return (x - leftBoundary()) * (zoom / base_width) * windowWidth; }
	int fracToWinY(float y) const { return (y - bottomBoundary()) * (zoom / base_height) * windowHeight; }
	//Add temporary modifiers to actual values
	void gulpState() {
		center_x += temp_center_x;
		center_y += temp_center_y;
		temp_center_x = temp_center_y = 0;
		return;
	}
	//Draw a fullscreen quad
	void drawQuad() {
		float leftb = leftBoundary();
		float rightb = rightBoundary();
		float topb = topBoundary();
		float botb = bottomBoundary();
		glBegin(GL_QUADS);
			glTexCoord2f(leftb, botb); glVertex2f(-1, -1);
			glTexCoord2f(rightb, botb); glVertex2f(1, -1);
			glTexCoord2f(rightb, topb); glVertex2f(1, 1);
			glTexCoord2f(leftb, topb); glVertex2f(-1, 1);
		glEnd();
	}

};

//Curent viewing state
ViewingState myViewingState;

//Map of saved states
std::map<unsigned char, ViewingState> savedStates;


 /* Initialize OpenGL Graphics */
void initGL() {
	// Set "clearing" or background color
	glClearColor(0.0, 0.0, 0.0, 0.0); // Black and opaque
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}


//Set the cenx and ceny in accordance with temp cenx and temp ceny
void gulpState() {
	mousedownx = mouseactivex;
	mousedowny = mouseactivey;
	myViewingState.gulpState();
}


//Callback for when the window changes size
void changeSize(int width, int height) {
	windowHeight = height;
	windowWidth = width;
	//To avoid divide by zero:
	if (height == 0)
		height = 1;
	float ratio =  float(width) / height;
	base_height = base_width / ratio;
	//Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);
	//Reset Matrix
	glLoadIdentity();
	//Set viewport to be the entire window
	//glViewport(0, 0, width, height);
	glViewport(0, 0, width, height);
	//Set the correct perspective
	//setPerspective();
	//Get back to modelview
	glMatrixMode(GL_MODELVIEW);
}

//Clears the buffers
void ClearScreen() { 
	glClearColor(0.0, 0.0, 0.0, 0.0); // Black and opaque
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
}

//Activate the fractal drawing mode for the current fractal mode selection
void activateFractalMode(const ViewingState& VS) {
	ShaderProgram* SP = FractalObject::all_FractalObjects_vec[VS.currentFractal]->getProgram(VS.plot_type, VS.color_style);
	SP->Use();
	SP->setInt("maxIterations", VS.max_iterations);
	SP->setInt("Gradient", 0);
	SP->setVec2("eyeCenter", VS.param_x, VS.param_y);
	if (VS.plot_type == FRACTYPE_JULIA) SP->setVec2("C", VS.param_x, VS.param_y);
	return;
}

//Activate fractal orbit tracing program
void activateOrbitTrace(const ViewingState& VS) {
	ShaderProgram* SP = FractalObject::all_FractalObjects_vec[VS.currentFractal]->getOrbitTraceProgram();
	SP->Use();
	SP->setVec2("C", VS.param_x, VS.param_y);
	SP->setInt("maxIterations", VS.max_iterations);
	SP->setVec2("bottomLeft", VS.leftBoundary(), VS.bottomBoundary());
	SP->setVec2("topRight", VS.rightBoundary(), VS.topBoundary());
	return;
}

//TODO: Implement a way to take screenshots natively

//Renderscene callback
void renderScene() {
	//Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_1D, myViewingState.colorscheme->second);
	//Use program
	glClearError();
	activateFractalMode(myViewingState);
	myViewingState.drawQuad();
	//Unuse program
	glUseProgram(0);
	glBindTexture(GL_TEXTURE_1D, 0);

	//Plot the orbit trace
	float tx = 0;//myViewingState.winToFracX(mousedownx);
	float ty = 0;//myViewingState.winToFracY(mousedowny);
	int X = myViewingState.fracToWinX(0.0f);
	int Y = myViewingState.fracToWinY(0.0f);
	float vx = (float(X) / windowWidth) * 2 - 1;
	float vy = (float(Y) / windowHeight) * 2 - 1;

	activateOrbitTrace(myViewingState);
	glBegin(GL_POINTS);
		glTexCoord2f(tx, ty); glVertex2f(vx, vy);
	glEnd();
	glUseProgram(0);

	//Implement what we've drawn
	glutSwapBuffers();
}

//Event handler for active mouse move
void activeMouseMove(int x, int y) {
	mouseactivex = x;
	mouseactivey = y;
	if (mouseleftdown) {
		myViewingState.temp_center_x = myViewingState.winToFracX(mousedownx) - myViewingState.winToFracX(x);
		myViewingState.temp_center_y = myViewingState.winToFracY(y) - myViewingState.winToFracY(mousedowny);;
		renderScene();
	}
	if (mouserightdown) {
		myViewingState.param_x = myViewingState.winToFracX(x);
		myViewingState.param_y = myViewingState.winToFracY(windowHeight-y);
		renderScene();
	}
}

//Event handler for mouseclicks
void mouseClick(int button, int state, int x, int y) {
	float fx = myViewingState.winToFracX(x);
	float fy = myViewingState.winToFracY(windowHeight - y);
	int mod = glutGetModifiers();
	switch (button) {
	case GLUT_LEFT_BUTTON:
		mouseleftdown = !state;
		if (state == GLUT_DOWN) {
			mousedownx = x;
			mousedowny = y;
		}
		else if (state == GLUT_UP) {
			mouseupx = x;
			mouseupy = y;
			myViewingState.gulpState();
			//ceny += tempceny;
			//tempcenx = 0.0f;
			//tempceny = 0.0f;
		}
		break;
	case GLUT_RIGHT_BUTTON:
		mouserightdown = !state;
		myViewingState.param_x = myViewingState.winToFracX(x);
		myViewingState.param_y = myViewingState.winToFracY(windowHeight-y);
		renderScene();
		break;
	case 3: //Upscroll
		if (state == GLUT_DOWN) {
			if (mod & GLUT_ACTIVE_SHIFT) {
				if (mod & GLUT_ACTIVE_CTRL)
					myViewingState.max_iterations *= 2;
				else
					++myViewingState.max_iterations;
			}
			else {
				myViewingState.zoom *= zoomfactor;
				fx = myViewingState.winToFracX(x);
				fy = myViewingState.winToFracY(windowHeight - y);
				gulpState();
				myViewingState.center_x += (fx - myViewingState.center_x) * (zoomfactor - 1.0f);
				myViewingState.center_y += (fy - myViewingState.center_y) * (zoomfactor - 1.0f);
			}
		}
		break;
	case 4: //Downscroll
		if (state == GLUT_DOWN) {
			if (mod & GLUT_ACTIVE_SHIFT) {
				if (mod & GLUT_ACTIVE_CTRL) {
					if (myViewingState.max_iterations > 1)
						myViewingState.max_iterations /= 2;
				}
				else {
					--myViewingState.max_iterations;
					if (myViewingState.max_iterations <= 0) {
						myViewingState.max_iterations = 0;
					}
				}
			}
			else {
				gulpState();
				myViewingState.center_x -= (fx - myViewingState.center_x) * (zoomfactor - 1.0f);
				myViewingState.center_y -= (fy - myViewingState.center_y) * (zoomfactor - 1.0f);
				myViewingState.zoom /= zoomfactor;
			}
		}
		break;
	}
	renderScene();
}

//Event handler for "special" keyboard input
void ProcessSpecialKeys(int key, int x, int y) {
	int mod = glutGetModifiers();
	switch (key) {
	case GLUT_KEY_LEFT:
		if (mod & GLUT_ACTIVE_CTRL)
			myViewingState.max_iterations /= 2;
		else {
			--myViewingState.max_iterations;
			if (myViewingState.max_iterations <= 0)
				myViewingState.max_iterations = 1;
		}
		break;
	case GLUT_KEY_RIGHT:
		if (mod & GLUT_ACTIVE_CTRL)
			myViewingState.max_iterations *= 2;
		else
			++myViewingState.max_iterations;
		break;
	}
	renderScene();
}

//Return true if they console should keep listening
bool listen() {
	std::string linetext;
	std::cerr << ">>>";
	std::cerr.flush();
	std::getline(std::cin, linetext);
	std::stringstream input(linetext);
	std::string command;
	input >> command;
	if (command == "back")
		return false;
	if (command == "save") {
		int which = 0;
		input >> which;
		if (which != 0) {
		}
		else {
			std::cerr << "Please specify which";
		}
		std::cerr << "Okay so the CLI isn't really implemented yet :)" << std::endl;
	}
	if (command == "ping") {
		//A changew
		std::cerr << "pong" << std::endl;
	}
	return true;
}

//Event handler for regular keyboard input
void ProccessKeys(unsigned char key, int x, int y) {
	int mod = glutGetModifiers();
	//If CTRL is held down, operate on savestates
	if (mod & GLUT_ACTIVE_CTRL) {
		//If shift is held down, load (if a valid state exists)
		if (mod & GLUT_ACTIVE_SHIFT) {
			if (savedStates.count(key)) {
				myViewingState = savedStates[key];
				std::cerr << "Loaded viewing state " << key << std::endl;
			}
			else
				std::cerr << char(7);
		}
		//Otherwise, save
		else {
			//Do not save if there exists one already
			if (savedStates.count(key))
				std::cerr << char(7);
			else {
				savedStates[key] = myViewingState;
				std::cerr << "Saved current viewing state as " << key << std::endl;
			}
		}
	}
	switch (key) {
	case '`':
		while(listen());
		break;
	case 'c':
		++myViewingState.colorscheme;
		if (myViewingState.colorscheme == Gradients::gradTextures.end())
			myViewingState.colorscheme = Gradients::gradTextures.begin();
		break;
	case 'C':
		if (myViewingState.colorscheme == Gradients::gradTextures.begin())
			myViewingState.colorscheme = Gradients::gradTextures.end();
		--myViewingState.colorscheme;
		break;
	case 'd':
		myViewingState.plot_type = !myViewingState.plot_type;
		break;
	case 's':
		myViewingState.color_style = !myViewingState.color_style;
		break;
	case 'f':
		++myViewingState.currentFractal;
		myViewingState.currentFractal %= FractalObject::all_FractalObjects_vec.size();
		break;
	case 'F':
		--myViewingState.currentFractal;
		if (myViewingState.currentFractal < 0)
			myViewingState.currentFractal = FractalObject::all_FractalObjects_vec.size() - 1;
		break;
	case 'h': //Return to home view
		myViewingState.center_x = 0.0f;
		myViewingState.center_y = 0.0f;
		myViewingState.zoom = 1.0f;
		break;
	case 'p': //Print summary of display state
		std::cerr << "Current viewing state: " << std::endl;
		myViewingState.print(std::cerr);
		std::cerr << std::endl;
		break;
	}
	renderScene();
}

//Initialize fractals to be used in this program
void initFractals() {
	//Set global fractal data directories
	FractalObject::my_global_source_directory = upLevelDirectory(glut32::getExecutableDirectory_s()) + "\\GlobalSource\\";
	FractalObject::my_fractals_directory = upLevelDirectory(glut32::getExecutableDirectory_s()) + "\\Fractals\\";
	//Open the config file
	std::ifstream input(FractalObject::my_fractals_directory + "\\config.txt");
	if (!input.good()) {
		std::cerr << "Fatal error - could not open config.txt" << std::endl;
		exit(1);
	}
	std::string token;
	while (std::getline(input, token)) {
		//Ignore comment lines; they start with //
		if (token.substr(0, 2) == "//") continue;
		new FractalObject(token);
	}
	//Close the config file
	input.close();
}

//Main function
int main(int argc, char** argv)
{
    std::cout << "Hello World!\n";

	//Initialize GLUT
	glutInit(&argc, argv);

	//Choose some settings for our Window
	glutInitWindowPosition(500, 100);
	glutInitWindowSize(900, 500);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);


	std::string windowName = "Mandelbrot Window";
	//Create the Window
	glutCreateWindow(windowName.c_str());
	//Minimize the window
	//glut32::minimizeWindow(windowName.c_str());
	glut32::maximizeWindow("Mandelbrot Window");


	//Initialize GLEW
	GLenum glewInitResult = glewInit();
	if (glewInitResult != GLEW_OK) {
		std::cerr << "Error while initializing GLEW. Exiting program..." << std::endl;
		exit(1);
	}

	////Some settings
	//glutIgnoreKeyRepeat(1);
	////glutSetCursor(GLUT_CURSOR_NONE); //Hide the cursor

	//// Display callbacks
	glutDisplayFunc(renderScene); //Callback for when we refresh
	//glutIdleFunc(idleFunc);
	glutReshapeFunc(changeSize); //Callback for when window is resized
	glutMouseFunc(mouseClick); //Callback for mouse clicks
	glutMotionFunc(activeMouseMove); //Callback for mouse clicks
	glutSpecialFunc(ProcessSpecialKeys); //Callback for a "special" key
	glutKeyboardFunc(ProccessKeys);


	//Initialize fractals
	initFractals();

	//Initialize shaders
	ShaderProgram::initializeAllShaderPrograms();
	//Initialize gradients
	Gradients::initGradients();
	myViewingState.colorscheme = Gradients::gradTextures.find("rainbow");

	initGL();                       // Our own OpenGL initialization
	glutMainLoop();                 // Enter the event-processing loop

	return 0;
}
