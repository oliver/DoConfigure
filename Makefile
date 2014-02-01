DoConfigure: DoConfig.cpp
	g++ DoConfig.cpp -o DoConfigure -s $$(fltk-config --cxxflags --ldflags)

