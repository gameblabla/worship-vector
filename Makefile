# path to RETRO68
PREFIX=$(RETRO68)/powerpc-apple-macos
CC=$(RETRO68)/bin/powerpc-apple-macos-gcc
CXX=$(RETRO68)/bin/powerpc-apple-macos-g++
REZ=$(RETRO68)/bin/Rez

CFLAGS = -D__MACOS__ -DSDLMIX_SOUND -DSCALING -Ofast -fdata-sections -ffunction-sections
CXXFLAGS = $(CFLAGS)

LDFLAGS=-lRetroConsole -lm  -lSDL_mixer -lSDL -lDialogsLib -Wl,--as-needed -Wl,--gc-sections 
RINCLUDES=$(PREFIX)/RIncludes
REZFLAGS=-I$(RINCLUDES)

SRC = 	gameframe.c \
	gamestate.c \
	initgame.c \
	mapdata.c \
	playeriteraction.c \
	renderobjects.c \
	vars.c \
	gamegui.c \
	gamestep.c \
	main.c \
	mobs.c \
	postrender.c \
	sblit.c \
	vlines.c \
	zmath.c \
	gameloop.c \
	gpu3d.c \
	mainmenu.c \
	palette.c \
	rendermap.c \
	ssystem.c \
	waveai.c
CFLAGS += $(DEFS)

OBJS = $(SRC:.c=.o)

HelloWorld.bin HelloWorld.APPL HelloWorld.dsk: HelloWorld.xcoff
	MakePEF HelloWorld.xcoff -o HelloWorld.pef
	$(REZ) $(REZFLAGS) ./SDL.r -I$(RINCLUDES) -DCFRAG_NAME="\"HelloWorld\"" -o HelloWorld.bin --cc HelloWorld.dsk --cc HelloWorld.APPL --cc %HelloWorld.ad -t APPL -c ???? --data HelloWorld.pef


HelloWorld.xcoff: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS)	# C++ used for linking because RetroConsole needs it

.PHONY: clean
clean:
	rm -f HelloWorld.bin HelloWorld.APPL HelloWorld.dsk HelloWorld.xcoff HelloWorld.xcoff.gdb *.o
