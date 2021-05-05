// BURNING SHIP JULIA FRAGMENT SHADER
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

//Main function
void main() {
	float Value;

	float x_;
	float y_;
	float x = ftexCoord.x;
	float y = ftexCoord.y;
	int i;
	for (i = 0; i < maxIterations; ++i) {
		x = abs(x);
		y = abs(y);
		x_ = x*x - y*y + X;
		y_ = 2.0*x*y - Y;
		x = x_;
		y = y_;
		if (x*x + y*y > 4.0) {
			break;
		}
	}
	Value = float(maxIterations-i) / float(maxIterations+1);
//	Value = 1.0 / (1 + length(vec2(x,y)));

	/* Set the frag color by sampling the Gradient 
	 * at a position determined by sampling the DataTexture */
//	FragColor = texture1D(Gradient, Value);
	/* Alpha is divided by multiplicity to keep the total opacity
	 * of this sheet invariant under multiplicity */
	 FragColor = texture1D(Gradient, Value);
//	 FragColor = vec4(Value,0,0,1);
};
