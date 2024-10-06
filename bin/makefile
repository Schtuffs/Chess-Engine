CXX      = g++

EXE		 = Chess-Engine

SRC		 = ../src
INCLUDE	 = ../include

FLAGS	 = -std=c++17 -I$(INCLUDE) -L../lib
CXXFLAGS = -c -Wall $(FLAGS)
LDFLAGS	 = $(FLAGS) # -mwindows

OBJECTS	 = glad.o stb_image.o main.o WindowManager.o RenderManager.o Callbacks.o FpsTracker.o Pawn.o

all: $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -lglfw3dll -o $(EXE)

glad.o: $(SRC)/glad.c
	$(CXX) $(CXXFLAGS) $<

stb_image.o: $(SRC)/stb_image.cpp $(INCLUDE)/stb_image.h
	$(CXX) $(CXXFLAGS) $<

main.o: $(SRC)/main.cpp
	$(CXX) $(CXXFLAGS) $<

WindowManager.o: ${SRC}/WindowManager.cpp $(INCLUDE)/WindowManager.h
	$(CXX) $(CXXFLAGS) $<

RenderManager.o: ${SRC}/RenderManager.cpp $(INCLUDE)/RenderManager.h
	$(CXX) $(CXXFLAGS) $<

Callbacks.o: $(SRC)/Callbacks.cpp $(INCLUDE)/Callbacks.h
	$(CXX) $(CXXFLAGS) $<

FpsTracker.o: $(SRC)/FpsTracker.cpp $(INCLUDE)/FpsTracker.h
	$(CXX) $(CXXFLAGS) $<

Pawn.o: $(SRC)/Pawn.cpp $(INCLUDE/Pawn.h)
	$(CXX) $(CXXFLAGS) $<

clean:
	del -q *.o *.exe

