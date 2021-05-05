
uniform vec2 C;
void main() {
	vec2 z = ftexCoord;
	int i;
	for (i = 0; i < maxIterations; ++i) {
		z = f_iterFunc(z,C);
		if (f_escapeTestFunc(z)) break;
	}

