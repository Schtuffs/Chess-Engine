CXX      = g++

EXE		 = Chess-Engine

SRC		 = ../src
INCLUDE	 = ../include

FLAGS	 = -std=c++17 -I$(INCLUDE) -L../lib
CXXFLAGS = -c -Wall $(FLAGS)
LDFLAGS	 = $(FLAGS)

OBJECTS	 = glad.o main.o WindowManager.o Callbacks.o

all: $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -lglfw3dll -o $(EXE)

glad.o: $(SRC)/glad.c
	$(CXX) $(CXXFLAGS) $<

main.o: $(SRC)/main.cpp
	$(CXX) $(CXXFLAGS) $<

WindowManager.o: ${SRC}/WindowManager.cpp $(INCLUDE)/WindowManager.h
	$(CXX) $(CXXFLAGS) $<

Callbacks.o: $(SRC)/Callbacks.cpp $(INCLUDE)/Callbacks.h
	$(CXX) $(CXXFLAGS) $<

clean:
	del -q *.o *.exe

