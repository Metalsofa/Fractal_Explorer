// COMMON FRAGMENT SHADER HEADER
#version 410 compatibility
//Input: Interpolated 2D texture coordinates for this fragment 
in vec2 ftexCoord;

//Output: The fragment color
out vec4 FragColor;

//Uniform: the number of iterations
uniform int maxIterations;
//Uniform: sampler used as gradient
uniform sampler1D Gradient;

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

