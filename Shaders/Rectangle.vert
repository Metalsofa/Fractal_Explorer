// B-SCAN SHADER
//Meant to be used drawing a simple square
#version 410 compatibility
//This shader uses compatibility fixed-function inputs only

//Output: Texture Coordinates
out vec2 ftexCoord;

//Main function
void main() {
	gl_Position = gl_Vertex;
	ftexCoord = vec2(gl_MultiTexCoord0.x, gl_MultiTexCoord0.y);
};
