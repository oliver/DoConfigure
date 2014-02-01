
HAVE_SDL:=$(shell sdl-config --version 2>/dev/null)
ifeq ($(HAVE_SDL),)
    SDL_OPTIONS:=
else
    SDL_OPTIONS:=$$(sdl-config --cflags --static-libs) \
	-DHAVE_SDL=1
endif

all: DoConfigure

clean:
	rm -f DoConfigure

DoConfigure: DoConfig.cpp
	g++ DoConfig.cpp -o DoConfigure -s $$(fltk-config --cxxflags --ldflags) $(SDL_OPTIONS)

