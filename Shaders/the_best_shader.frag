// JULIA FRAGMENT SHADER
//uses a 1D texture to map that data point to an output color.
#version 410 compatibility
//Input: Interpolated 3D texture coordinates for this fragment 
in vec2 ftexCoord;

//Output: The fragment color
out vec4 FragColor;

//Uniform: the number of iterations
uniform int maxIterations;
//Uniform: sampler used as gradient
uniform sampler1D Gradient;
//Uniform: C
uniform float X;
uniform float Y;

vec2 cdiv(vec2 z1, vec2 z2) {
	return vec2 (
		dot(z1,z2) / dot(z2,z2),
		-dot(z1,vec2(z2.y,z2.x)) / dot(z2,z2)
	);
}

vec2 cmult(vec2 z1, vec2 z2) {
	return vec2(
		z1.x * z2.x - z1.y * z2.y,
		z1.x * z2.y + z1.y * z2.x
	);
}


//Main function
void main() {
	float Value;

	float x = ftexCoord.x;
	float y = ftexCoord.y;
	vec2 z = vec2(x,y);
	vec2 c = vec2(X,Y);
	int i;
	for (i = 0; i < maxIterations; ++i) {
		z = cdiv(cmult(z,z)+z, cmult(cmult(z,z),z)+c);
		if (length(z) > 2.0) {
			break;
		}
	}
	Value = float(maxIterations-i) / float(maxIterations+1);
//	Value = 1.0/(1.0+length(z));

	/* Set the frag color by sampling the Gradient 
	 * at a position determined by sampling the DataTexture */
//	FragColor = texture1D(Gradient, Value);
	/* Alpha is divided by multiplicity to keep the total opacity
	 * of this sheet invariant under multiplicity */
	 FragColor = texture1D(Gradient, Value);
//	 FragColor = vec4(Value,0,0,1);
};
