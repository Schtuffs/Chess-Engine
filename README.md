# Chess-Engine
 Simple engine to play chess. To run, you will need glfw3.3
 dll's in order to run the program. Run by compiling with makefile 
 in bin directory, and using the executable.

## DLL Info
 Put "glfw3.dll" into bin directory and "libglfw3dll.a" into lib directory.
 You can download the files from https://www.glfw.org/download.html.

## Features

### Window Manager
 Deals with the creation and functionality of the window.

### Board Manager
 Handles the rendering manager for the board and pieces. Allows
 selection of board colour, adding pieces, resetting the board, and
 works with other functions to select pieces and move them around.

### Render Manager
 Deals with the rendering of stuff to the screen.

### Fps Tracker
 Prints FPS to console if '`' (backtick) is pressed.

## Pieces

### All as one
 Pieces are all one type but will call different functions based 
 on what type they are. Such as pieces calling different move methods 
 to determine their legal moves in any given position.

### Piece art
 Hand-made by me in microsoft paint a few months ago, which is part of the 
 reason im doing this project. That part was easy. Rendering these textures onto
 squares is a lot harder.
