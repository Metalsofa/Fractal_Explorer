#version 410 core
layout (points) in;
layout (line_strip, max_vertices = 128) out;

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


in VS_OUT {
	vec2 vtexCoord;
} gs_in[];
