
void main() {
	vec2 z = vec2(0,0);
	int i;
	for (i = 0; i < maxIterations; ++i) {
		z = f_iterFunc(z,ftexCoord);

