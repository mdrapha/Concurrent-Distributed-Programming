# Rainbow Game of Life

Created by John H Conway, the Rainbow Game of Life is a colorful variant of the classic cellular automaton, Conway's Game of Life.

## Overview

The Rainbow Game of Life features a bidimensional board of cells with two states:

- **1 or > 0.0** indicates a cell is alive.
- **0** indicates a cell is dead.

Unlike the classic Game of Life, the board values are floating point, allowing for a gradient of colors. 

## Rules

1. Any live cell with fewer than two live neighbours dies, due to underpopulation.
2. Any live cell with two or three live neighbours survives to the next generation.
3. Any live cell with more than four live neighbours dies, due to overpopulation.
4. Any dead cell with exactly three live neighbours becomes a live cell, due to reproduction.
5. All other live cells die in the next generation. Similarly, all other dead cells stay dead.

## Color Dynamics

When a new cell is born, its value (or color intensity) is determined by the arithmetical average of its immediate neighbours.

## Board Details

- The board is bidimensional and NxN in size.
- Infinite boundaries: Edges wrap around, creating a toroidal structure.
- The coordinate (0,0) refers to the upper-left corner, while (N-1,N-1) refers to the lower-right corner.
- Any cell with a value greater than 0.0 is considered alive.

## Running the Program

### Requirements for the C version

- GCC compiler
- openMP library
- pthread library
- Linux OS is recommended

### Serial Version

To run the serial version of the program, simply run the following command:

```bash
$ gcc serial.c -o serial 
$ ./serial
```

### OpenMP Version
```bash
$ gcc openmp.c -o openmp -fopenmp
$ ./openmp
```

### Pthread Version
```bash
$ gcc pthread.c -o pthread -lpthread
$ ./pthread
```

### OpenGL Visualizer Version
```bash
$ gcc graphic_rainbowl_life_game.c -o graphic -lGL -lGLU -lglut -lm -fopenmp 
```

--- 

_**Note:** This project is inspired by the original Conway's Game of Life, but introduces new dynamics and rules._
