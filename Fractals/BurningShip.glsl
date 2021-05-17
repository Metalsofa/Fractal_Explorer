
//Required: Function used to iterate z, using c as a parameter
vec2 f_iterFunc(vec2 z, vec2 c) {
	return cmult(abs(z),abs(z)) + vec2(c.x, -c.y);
}

//Required: Function for testing whether z has escaped
bool f_escapeTestFunc(vec2 z) {
	return (length(z) > 2.0);
}

