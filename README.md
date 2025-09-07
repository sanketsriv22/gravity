# Gravity Sim (C++, OpenGL)

A Gravity Simulation created with OpenGL.

## GIF
<!-- ![Pong Gameplay](assets/pong.gif) -->

___

## Features
<!-- - Classic Pong Gameplay (player vs. CPU)
- Player and CPU paddles with collision detection.
- Scoreboard system (first to 3 points wins).
- Multiple game states (Main Menu, Playing, Win/Lose).
- Structured with classes (Ball, Paddle, Human/CpuPaddle) and method implementation to showcase encapsulation, inheritance, and polymorphism. -->

---

## Getting Started

### Prerequisites
- **C++ compiler** ('clang++' or 'g++')
- **glfw** (installed via Homebrew on macOS):
```bash
brew install glfw
```
## Clone the Repo
```bash
git clone https://github.com/sanketsriv22/gravity.git
cd gravity
```

## Build and Run
If using VSCode, just run build task (Cmd + Shift + B)
```bash
g++ -std=c++20 -I/opt/homebrew/Cellar/glfw/3.4/include \
    -L/opt/homebrew/Cellar/glfw/3.4/lib \
    -lglfw *.cpp -o Gravity
./Gravity
```
Or if you prefer cmake (I've added my CMakeList.txt)
```bash
brew install cmake
```
Create a cmake build in the repo
```bash
cmake -S . -B build
```
Compile/Build (Cmd + Shift + B)
And run
```bash
./build/gravity
```

## Controls
<!-- - W (up), S (down) -> move player paddle
- Enter -> start game / restart after game over
- Esc -> quit -->

## Future Improvements
<!-- - Usage of 'Game' class to hold all environment object instances (merged 9/5)
- Two-player mode
- Sound effects
- Better AI for CPU paddle
- Different game modes/difficulties
- Customizable win conditions -->
