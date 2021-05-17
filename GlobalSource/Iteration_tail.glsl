
		if (f_escapeTestFunc(z)) break;
	}
	float Value = 1.0 - float(maxIterations-i) / float(maxIterations+1);
	FragColor = f_IterColorFunc(Value);
};

