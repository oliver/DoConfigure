
DoConfigure: DoConfig.cpp
	g++ DoConfig.cpp -o DoConfigure -s $$(fltk-config --cxxflags --ldflags) $$(sdl-config --cflags --static-libs)

