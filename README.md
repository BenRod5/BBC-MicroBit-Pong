BBC micro:bit Pong
A single-player pong game for the BBC micro:bit, built as part of coursework at Lancaster University. The project demonstrates object-oriented C++ running on embedded hardware, alongside an experimental ARM Thumb assembly implementation of the ball physics logic.

What it does
The game runs on the micro:bit's 5×5 LED matrix. A ball bounces around the screen and the player controls a two-pixel paddle along the bottom row using the micro:bit's two physical buttons. Missing the ball ends the game and displays the player's score.

Button A — move paddle left
Button B — move paddle right
Score increments each time the ball is successfully returned
Game over triggers when the ball reaches the bottom row without hitting the paddle


Project structure
main.cpp          — Full C++ implementation (game logic, rendering, input handling)
updateBallPos.s   — Experimental ARM Thumb assembly reimplementation of ball physics
C++ implementation (main.cpp)
The C++ code uses an inheritance-based class hierarchy to represent game objects cleanly:

gameObject — base class holding x/y coordinates with getter and move methods
gameBall — extends gameObject, adds directional state and inversion methods
gamePaddle — holds an array of two gameObject instances representing the paddle's two pixels

Ball movement is handled in updateBallPosition(), which works by computing potential next coordinates before committing the move. This allows wall collisions, paddle collisions, and the game-over condition to all be checked and resolved before the ball actually changes position — preventing the ball from ever visually clipping through a surface.
The display loop runs on a separate fiber (the micro:bit's cooperative multitasking primitive), sleeping for one second between updates.
Assembly implementation (updateBallPos.s)
An ARM Thumb assembly version of the ball physics function was written as an exercise in low-level programming. The assembly accesses the ball and paddle state via pointer-to-pointer indirection, working around the fact that C++ class types cannot be used directly as extern symbols in assembly.
The assembly version was not integrated into the final build — it was written and studied as a learning exercise, and the C++ version ships in the working game.

How the project was built and run
This project was developed using the CODAL / micro:bit v2 runtime and compiled with the Lancaster University build toolchain for the micro:bit platform.
The general process for running micro:bit projects like this one is:

Set up the micro:bit C++ build environment (CMake + ARM GCC toolchain + the micro:bit CODAL runtime)
Compile the project, which produces a microbit.hex file
Flash the .hex to a physical micro:bit by copying it to the device over USB

Because the build environment includes runtime library files that are not part of this repository, the project cannot be compiled from these source files alone. The code is shared here as a portfolio piece rather than a standalone runnable project.

Key concepts demonstrated

OOP design in C++ (inheritance, encapsulation, access control)
Embedded systems programming on constrained hardware (5×5 display, no heap allocations)
Collision detection using lookahead (potential coordinate) logic
ARM Thumb assembly: register use, branching, memory load/store, pointer dereferencing
Cooperative multitasking with micro:bit fibers
Event-driven input handling via the micro:bit message bus


Notes
The assembly file contains detailed inline comments explaining the reasoning behind each instruction and the double-dereference pointer strategy used to access C++ object state. These comments are intentionally verbose — they were written as part of the learning process.
