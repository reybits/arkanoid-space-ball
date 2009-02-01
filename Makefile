CC=g++
CFLAGS=-c -Wall `sdl-config --cflags`
LDFLAGS=`sdl-config --libs --cflags` -lSDL_image  -lSDL_mixer
SOURCES=src/arkanoidsb.cpp src/arkanoidsbgame.cpp src/ball.cpp src/bonus.cpp src/bullet.cpp src/coolstring.cpp src/energyhole.cpp src/exploision.cpp src/generatelevel.cpp src/glSDL.c src/leveleditor.cpp src/mainmenu.cpp src/monster.cpp src/mystring.cpp src/reminderdlg.cpp src/resource.cpp src/sinusstring.cpp src/tutorialdlg.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=arkanoidsb

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
