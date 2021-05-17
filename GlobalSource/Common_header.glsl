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

//Divide two complex numbers
vec2 cdiv(vec2 z1, vec2 z2) {
	return vec2 (
		dot(z1,z2),
		dot(z1,vec2(-z2.y,z2.x))
	) / dot(z2,z2);
}

//Multiply two complex numbers
vec2 cmult(vec2 z1, vec2 z2) {
	return vec2(
		z1.x * z2.x - z1.y * z2.y,
		z1.x * z2.y + z1.y * z2.x
	);
}

//Raise a complex number to a real power
vec2 cpowf(vec2 z, float p) {
	float a = atan(z.y,z.x)*p;
	return pow(length(z),p) * vec2(cos(a),sin(a));
}

vec3 sineBowHue(float phi) {
	const float TAU = 6.2831853;
	vec3 offset = vec3(0, 1, -1) * TAU / 3;
	return (cos(phi - offset) + 1)/2;
}

//Required: Function for picking a color based on the final z state and iteration count
vec4 f_IterColorFunc(float i) {
	return texture1D(Gradient, i);
}

//Required: Function for picking a color based on the final z state and iteration count
vec4 f_FinalstateColorFunc(vec2 z) {
//	return texture1D(Gradient, 1.0 / (length(z) + 1));
	return vec4(sineBowHue(atan(z.y,z.x))/(length(z)+1),1);
}

// USER-SUPPLIED FUNCTIONS

