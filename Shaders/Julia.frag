// JULIA FRAGMENT SHADER
//uses a 1D texture to map that data point to an output color.
#version 410 compatibility
//Input: Interpolated 2D texture coordinates for this fragment 
in vec2 ftexCoord;

//Output: The fragment color
out vec4 FragColor;

//Uniform: the number of iterations
uniform int maxIterations;
//Uniform: sampler used as gradient
uniform sampler1D Gradient;
//Uniform: C
uniform vec2 C;

// PROGRAM-SUPPLIED FUNCTIONS

vec2 cdiv(vec2 z1, vec2 z2) {
	return vec2 (
		dot(z1,z2),
		dot(z1,vec2(-z2.y,z2.x))
	) / dot(z2,z2);
}

vec2 cmult(vec2 z1, vec2 z2) {
	return vec2(
		z1.x * z2.x - z1.y * z2.y,
		z1.x * z2.y + z1.y * z2.x
	);
}

vec2 cpowf(vec2 z, float p) {
	float a = atan(z.y,z.x)*p;
	return pow(length(z),p) * vec2(cos(a),sin(a));
}

// USER-SUPPLIED FUNCTIONS

//Required: Function used to iterate z, using c as a parameter
vec2 f_iterFunc(vec2 z, vec2 c) {
	return cmult(z,z) + c;
}

//Required: Function for testing whether z has escaped
bool f_escapeTestFunc(vec2 z) {
	return (length(z) > 2.0);
}

//Required: Function for picking a color based on the final z state and iteration count
vec4 f_colorFunc(float i, vec2 z) {
	return texture1D(Gradient, i);
}

//Main function
void main() {
	float Value;
	vec2 z = ftexCoord;
	int i;
	for (i = 0; i < maxIterations; ++i) {
		z = f_iterFunc(z,C);
		if (f_escapeTestFunc(z)) break;
	}
	Value = float(maxIterations-i) / float(maxIterations+1);
	FragColor = f_colorFunc(Value, z);
};
