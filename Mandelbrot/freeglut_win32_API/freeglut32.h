#pragma once
/* This header file defines functions that are meant to look like
 * they belong to the freeglut API. In reality, they are simply
 * helper functions for the win32 API, spelled out as if they
 * were for freeGLUT. This can be very helpful if don't want to
 * bother learning win32 syntax and just want to get working
 * with freeglut.
 * For obvious reasons, this is only supported on Windows. */
#ifndef __FREEGLUT32API_h__
#define __FREEGLUT32API_h__

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdlib.h>
#include <shellapi.h>

#define GLEW_STATIC
#include <glew.h>
#include <GL/glut.h>

#include <locale>
#include <codecvt>
#include <commdlg.h>
#include <string>
#include <vector>
#include <map>

//Map openGL error codes to their names
const std::map<GLenum, std::string> glErrorMap = {
	{GL_NO_ERROR, "GL_NO_ERROR"}, //0, No user error reported since the last call to glGetError.
	{GL_INVALID_ENUM, "GL_INVALID_ENUM"}, //1280, Set when an enumeration parameter is not legal.
	{GL_INVALID_VALUE, "GL_INVALID_VALUE"}, //1281, Set when a value parameter is not legal.
	{GL_INVALID_OPERATION, "GL_INVALID_OPERATION"}, //1282	Set when the state for a command is not legal for its given parameters.
	{GL_STACK_OVERFLOW, "GL_STACK_OVERFLOW"}, //1283, Set when a stack pushing operation causes a stack overflow.
	{GL_STACK_UNDERFLOW, "GL_STACK_UNDERFLOW"}, //1284, Set when a stack popping operation occurs while the stack is at its lowest point.
	{GL_OUT_OF_MEMORY, "GL_OUT_OF_MEMORY"}, //1285, Set when a memory allocation operation cannot allocate(enough) memory.
	{GL_INVALID_FRAMEBUFFER_OPERATION, "GL_INVALID_FRAMEBUFFER_OPERATION"}, //1286	Set when reading or writing to a framebuffer that is not complete.
};

bool rapidGLErrorOutput = true;

//Repeatedly queries glGetError until no error is returned, to clear it
void glClearError() {
	for (GLenum err; err = glGetError(); ) {
		if (rapidGLErrorOutput) {
			std::cerr << glErrorMap.find(err)->second
				<< " | DETECTED IN glClearError()";
		}
	}
}


//Queries glGetError and broadcasts errors raised
void errorCheck(const std::string& fileName, const std::string& functionName, int line, const std::string& otherNotes = "") {
	GLenum error = glGetError();
	if (error == GL_NO_ERROR) {
		return;
	}
	//It is assumed that a valid error is raised by glGetError
	std::string errorFound = glErrorMap.find(error)->second;
	std::string output = errorFound + " | " + fileName + " " + functionName
		+ " (" + std::to_string(line) + ")";
	if (otherNotes.size())
		output = output + " - " + otherNotes;
	if (rapidGLErrorOutput)
		std::cerr << output << std::endl;
	return;
}
	
	//A helpful errorchecking macro
#define glCheckError(_notes) errorCheck(__FILE__,__FUNCTION__,__LINE__,_notes)
#define glCheckError2() errorCheck(__FILE__,__FUNCTION__,__LINE__)


namespace glut32 {
	//Converts a wstring into a string
	//This code based on a response in the following stackoverflow page:
	//https://stackoverflow.com/questions/4804298/how-to-convert-wstring-into-string
	std::string ws_to_s(const std::wstring& ws) {
		//setup converter
		using convert_type = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_type, wchar_t> converter;

		//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
		return converter.to_bytes(ws);
	}

	//Returns the directory of the executable file currently running (wstring)
	std::wstring getExecutableDirectory_ws() {
		//This code is taken from an answer in the following stackoverflow page
		//https://stackoverflow.com/questions/875249/how-to-get-current-directory
		TCHAR buffer[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, buffer, MAX_PATH);
		std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
		return std::wstring(buffer).substr(0, pos);
	}

	//Returns the directory of the executable file currently running (string)
	std::string getExecutableDirectory_s() {
		//This code is taken from an answer in the following stackoverflow page
		//https://stackoverflow.com/questions/875249/how-to-get-current-directory
		TCHAR buffer[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, buffer, MAX_PATH);
		std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
		return ws_to_s(std::wstring(buffer).substr(0, pos));
	}
	
	//Converts a c-style string to a long-pointer-character-wide-string object, making a 
	//NEW one on the heap. Be sure to delete it when finished to avoid memory leaks.
	wchar_t* makeLPCWSTR(const char*& text) {
		// Based on the code from this stack overflow response
		//https://stackoverflow.com/questions/29847036/convert-char-to-wchar-t-using-mbstowcs-s
		std::size_t size = strlen(text) + 1;
		wchar_t* wideText = new wchar_t[size];
		//This argument isn't presently returned, but it's still here.
		std::size_t outSizeArg;
		mbstowcs_s(&outSizeArg, wideText, size, text, size - 1);
		return wideText;
	}

	//Get a handle to the top-level GLUT window with a particular name
	HWND getWindowHandle(const char* windowTitle) {
		//Convert string type
		wchar_t* windt = makeLPCWSTR(windowTitle);
		//Get the window handle
		HWND reth = FindWindow(NULL, windt);
		//Delete the new string
		delete windt;
		//Return the handle
		return reth;
	}

	//Provide the program name as the second argument (this should be argc[0])
	void setWindowIcon(const char* windowname, const char* execname, int iconID) {
		/* Thanks a ton to the following askers/answerers:
		 *	https://stackoverflow.com/questions/12748103/how-to-change-freeglut-main-window-icon-in-c
		 *	https://stackoverflow.com/questions/18064943/console-window-has-icon-but-opengl-glut-window-dont-have-why
		 */
		 //Get the window handler
		wchar_t* windt = makeLPCWSTR(windowname);
		HWND hwnd = FindWindow(NULL, windt);
		delete windt;
		//Get the height and width of an icon for this system
		SIZE smallIconSize = { GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON) };
		wchar_t* exect = makeLPCWSTR(execname);
		//Create the icon handle
		HANDLE icon = LoadImage(GetModuleHandle(exect), MAKEINTRESOURCE(iconID), IMAGE_ICON, 64, 64, LR_COLOR);
		delete exect;
		SendMessage(hwnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(icon));
	}

	//The following code is taken and slightly modified from here
	//https://www.daniweb.com/programming/software-development/code/217307/a-simple-getopenfilename-example
	//Global OpenFileName structure for win32 file selection dialogue
	OPENFILENAME ofn;
	// a another memory buffer to contain the file name, must be >= 256 chars long
	wchar_t szFile[256];
	//Opens a file-select dialogue box and returns the selected file as a string
	//Returns "" if the action is cancelled
	std::string singleFileSelectDialog() {
		// open a file name
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = szFile;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = L"All\0*.*\0OmniPC Files\0*.OPD\0";
		ofn.nFilterIndex = 2;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		bool canceled = !GetOpenFileName(&ofn);

		//Now return the selected string
		if (canceled)
			return "";
		else
			return ws_to_s(ofn.lpstrFile);
	}

	//Opens a file-select dialog box that allows the selection of multiple files,
	//Returning them all as strings in a vector
	//Returns an empty vector if the action is cancelled
	std::vector<std::string> multiFileSelectDialog() {
		//Vector to return
		std::vector<std::string> retv;
		// open a file name
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = szFile;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = L"All\0*.*\0OmniPC Files\0*.OPD\0";
		ofn.nFilterIndex = 2;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_ALLOWMULTISELECT;

		bool canceled = !GetOpenFileName(&ofn);

		//Return an empty vector if the action is cancelled
		if (canceled)
			return retv;

		std::string path = ws_to_s(ofn.lpstrFile);
		//If only one file was selected, 'path' will contain a file name erroneously
		if (path.size() > ofn.nFileOffset) {
			retv.push_back(path);
			return retv;
		}
		wchar_t* itr = ofn.lpstrFile + ofn.nFileOffset;

		std::string addition;
		while (*itr != NULL) {
			addition = ws_to_s(itr);
			retv.push_back(path + "\\" + addition);
			itr += addition.size() + 1;
		}

		return retv;
	}

	//Create a loading bar
	void makeProgressBar() {
		//Not yet implemented
	}

	//Maximizes a window by being passed its name
	void maximizeWindow(const char* windowName) {
		ShowWindow(getWindowHandle(windowName), SW_SHOWMAXIMIZED);
	}

	//Minimizes a window by being passed its name
	void minimizeWindow(const char* windowName) {
		ShowWindow(getWindowHandle(windowName), SW_SHOWMINIMIZED);
	}

	//Open a file in the system's preferred way
	void sysOpenFile(const std::string& fullpath) {
		ShellExecuteA(NULL, "open", fullpath.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}
}


#endif
