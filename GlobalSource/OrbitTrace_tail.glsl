

uniform vec2 C;
uniform vec2 bottomLeft;
uniform vec2 topRight;
uniform int maxIterations;

vec2 mapCoords(vec2 z) {
	return (z - bottomLeft) / (topRight - bottomLeft) * 2 - 1;
}

void main() {
	vec2 z = gs_in[0].vtexCoord;
	gl_Position.xy = mapCoords(z);
	gl_Position.zw = vec2(0,1);
	EmitVertex();
	int I = min(maxIterations, 127);
	for (int i = 0; i < I; ++i) {
		z = f_iterFunc(z, C);
		gl_Position.xy = mapCoords(z);
		EmitVertex();
	}
	EndPrimitive();
}