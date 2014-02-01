DoConfigure - r2

Changelog:
	[r2]
	*Now supports Joysticks.
	*Compiled against fltk 1.3.
	*Still no checking for errors in file.
	[r1]
	*Basic editing of Config.dat.
	*Wasn't sure if Joystick was supported in Linux.
	*Compiled against fltk 1.2.
	*No checking for errors in file
	
Compiling:

Compile it as you would any other FLTK program, I used:

g++ DoConfig.cpp -o DoConfigure -s $(fltk-config --cxxflags --ldflags)

-Sean Baker