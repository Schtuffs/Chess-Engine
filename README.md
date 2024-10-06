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

### Render Manager
 Deals with the rendering of stuff to the screen.

### Fps Tracker
 Prints FPS to console if '`' (backtick) is pressed.

## Pieces

### Inheritance
 All pieces will inherit from the pieces virtual class, forcing them to 
 make a few functions. This will also allow some polymorphism when trying to
 do functions, such as calculating moves.

### Piece art
 Hand-made by me in microsoft paint a few months ago, which is part of the 
 reason im doing this project. That part was easy. Rendering these textures onto
 squares is a lot harder.
