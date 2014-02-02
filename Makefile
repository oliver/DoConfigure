
HAVE_SDL:=$(shell sdl-config --version 2>/dev/null)
ifeq ($(HAVE_SDL),)
    SDL_OPTIONS:=
else
    SDL_OPTIONS:=$$(sdl-config --cflags --static-libs) \
	-DHAVE_SDL=1
endif

DIST_FILES:=DoConfigure DoConfigure_nosdl COPYING.txt README.txt DoConfig.cpp Makefile


all: DoConfigure

clean:
	rm -f DoConfigure DoConfigure_nosdl

RELEASE_VERSION:=$(shell date '+%Y%m%d-%H%M%S')
release: DoConfigure DoConfigure_nosdl
	mkdir DoConfigure-$(RELEASE_VERSION)
	cp $(DIST_FILES) DoConfigure-$(RELEASE_VERSION)/
	zip -r DoConfigure-$(RELEASE_VERSION).zip DoConfigure-$(RELEASE_VERSION)/
	rm -rf DoConfigure-$(RELEASE_VERSION)/


DoConfigure: DoConfig.cpp
	g++ $+ -o $@ -s $$(fltk-config --cxxflags --ldstaticflags) $(SDL_OPTIONS)

DoConfigure_nosdl: DoConfig.cpp
	g++ $+ -o $@ -s $$(fltk-config --cxxflags --ldstaticflags)
