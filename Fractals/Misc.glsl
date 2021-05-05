//Required: Function used to iterate z, using c as a parameter
vec2 f_iterFunc(vec2 z, vec2 c) {
	return cmult(z,cmult(z,z)) + c;
}

//Required: Function for testing whether z has escaped
bool f_escapeTestFunc(vec2 z) {
	return (length(z) > 2.0);
}

//Required: Function for picking a color based on the final z state and iteration count
vec4 f_IterColorFunc(float i) {
	return texture1D(Gradient, i);
}

//Required: Function for picking a color based on the final z state and iteration count
vec4 f_FinalstateColorFunc(vec2 z) {
	return texture1D(Gradient, 1.0 / (length(z) + 1));
}
