#version 410 compatibility

out VS_OUT {
	vec2 vtexCoord;
} vs_out;

void main() {
	gl_Position = gl_Vertex;
	vs_out.vtexCoord = vec2(gl_MultiTexCoord0.x, gl_MultiTexCoord0.y);
}