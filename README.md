# Fractal_Explorer
OpenGL-based application for exploring Mandelbrot, Julia, and other fractals
The controls are currently very simple:

Use left-click to pan around the image,

Use right-click to alter the fractal parameter (if the current fractal requires one)

Use `shift+scrollwheel` to change the number of fractal iterations

Press `c` to go to the next colorscheme, or `shift+C` to go to the previous one

Press `m` to go to the next fractal, or `shift+M` to go to the previous one

Press `h` at any time to return to the default view, in case you get lost

Press `ctrl+<any key>` to save the current camera position and display settings (they will be bound to the key you pressed)

Press `ctrl+shift+<any key>` to load a previously saved camera position/display settings (it'll load the one saved under the key you pressed)

For example, if you find something cool, do `ctrl+A` to save it under the `A` key, then later press `ctrl+shift+A` to return to that saved spot.

The fractals are defined in the Shaders folder. 
The first four of these are classic fractals, being the Mandelbrot, Mandelbrot-Julia, Burning Ship, and Burning Ship-Julia fractals in order.
The fifth one (MiscShader1.frag) is meant for some user experimentation--since shaders are compiled upon program startup,
you can modify the GLSL shader code to make your own fractals! It's fun to experiment with this.

Things I still haven't added:
-An easier way to make your own fractals without needing to know too much about GLSL
-A way for users to make their own colorschemes (instead of just having to use the ones I made)
-An native way for users to take screencaps or larger-than-screen captures
