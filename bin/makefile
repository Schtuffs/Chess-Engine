CXX      = g++

EXE		 = Chess-Engine

SRC		 = ../src
INCLUDE	 = ../include

FLAGS	 = -std=c++17 -I$(INCLUDE) -L../lib
CXXFLAGS = -c -Wall $(FLAGS)
LDFLAGS	 = $(FLAGS) # -mwindows

OBJECTS	 = glad.o stb_image.o main.o Library.o WindowManager.o RenderManager.o BindManager.o EventManager.o BoardManager.o MoveManager.o Callbacks.o FpsTracker.o Piece.o

all: $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -lglfw3dll -o $(EXE)

glad.o: $(SRC)/glad.c
	$(CXX) $(CXXFLAGS) $<

stb_image.o: $(SRC)/stb_image.cpp $(INCLUDE)/stb_image.h
	$(CXX) $(CXXFLAGS) $<

main.o: $(SRC)/main.cpp
	$(CXX) $(CXXFLAGS) $<

Library.o: $(SRC)/Library.cpp $(INCLUDE)/Library.h
	$(CXX) $(CXXFLAGS) $<

WindowManager.o: ${SRC}/WindowManager.cpp $(INCLUDE)/WindowManager.h
	$(CXX) $(CXXFLAGS) $<

RenderManager.o: ${SRC}/RenderManager.cpp $(INCLUDE)/RenderManager.h
	$(CXX) $(CXXFLAGS) $<

BindManager.o: ${SRC}/BindManager.cpp $(INCLUDE)/BindManager.h
	$(CXX) $(CXXFLAGS) $<

EventManager.o: ${SRC}/EventManager.cpp $(INCLUDE)/EventManager.h
	$(CXX) $(CXXFLAGS) $<

BoardManager.o: ${SRC}/BoardManager.cpp $(INCLUDE)/BoardManager.h
	$(CXX) $(CXXFLAGS) $<

MoveManager.o: ${SRC}/MoveManager.cpp $(INCLUDE)/MoveManager.h
	$(CXX) $(CXXFLAGS) $<

Callbacks.o: $(SRC)/Callbacks.cpp $(INCLUDE)/Callbacks.h
	$(CXX) $(CXXFLAGS) $<

FpsTracker.o: $(SRC)/FpsTracker.cpp $(INCLUDE)/FpsTracker.h
	$(CXX) $(CXXFLAGS) $<

Piece.o: $(SRC)/Piece.cpp $(INCLUDE)/Piece.h
	$(CXX) $(CXXFLAGS) $<

clean:
	del -q *.o *.exe

