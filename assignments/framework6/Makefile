PROGNAME = main
SRCFILES = main.cpp levels.cpp
TARNAME = box2d.tgz

CC = g++

WARNFLAGS = -Wall -Wshadow -pedantic-errors
CFLAGS = -g -O2 $(WARNFLAGS) -Iinclude/

UNAME=$(shell uname -s)

ifeq ($(UNAME), Darwin)
LDFLAGS=-framework Box2D -framework GLUT -framework OpenGL -framework Cocoa -framework GLEW
else
LDFLAGS= -lGL -lglut -lBox2D -lGLEW
endif

# Do some substitution to get a list of .o files from the given .cpp files.
OBJFILES = $(patsubst %.cpp,%.o,$(SRCFILES))

.PHONY: all run clean dist todo

all: $(PROGNAME)

$(PROGNAME): $(OBJFILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CC) -c $(CFLAGS) -o $@ $<

run: $(PROGNAME)
	LD_LIBRARY_PATH=/usr/local/lib ./$(PROGNAME)

dist:
	tar cvzf $(TARNAME) Makefile *.cpp *.h

clean:
	rm -fv $(PROGNAME) $(OBJFILES) $(TARNAME)
