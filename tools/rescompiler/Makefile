CC=g++
CFLAGS=-O2 -std=c++11 -c -Wall `sdl-config --cflags`
LDFLAGS=`sdl-config --libs --cflags`
SOURCES=src/resource.cpp src/rescompiler.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=rescompiler

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f src/*.o

