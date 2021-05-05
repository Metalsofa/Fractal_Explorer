
	float Value = float(maxIterations-i) / float(maxIterations+1);
	FragColor = f_IterColorFunc(Value);
};

