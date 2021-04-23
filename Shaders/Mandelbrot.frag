// MANDELBROT FRAGMENT SHADER
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

vec2 iterate(vec2 z, vec2 c) {
	return vec2(z.x*z.x -z.y*z.y + c.x, 2*z.x*z.y+c.y);
}

//Main function
void main() {
	float Value;

	vec2 z = vec2(0,0);
	int i;
	for (i = 0; i < maxIterations; ++i) {
		z = iterate(z,ftexCoord);
		if (length(z) > 2.0) {
			break;
		}
	}
	Value = float(maxIterations-i) / float(maxIterations+1);

	/* Set the frag color by sampling the Gradient 
	 * at a position determined by sampling the DataTexture */
//	FragColor = texture1D(Gradient, Value);
	/* Alpha is divided by multiplicity to keep the total opacity
	 * of this sheet invariant under multiplicity */
	 FragColor = texture1D(Gradient, Value);
};

