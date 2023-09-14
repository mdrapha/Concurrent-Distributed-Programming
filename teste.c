// rainbow game of life created by: john h conway

// bidimensional board of cells (1 or 0)
// 1 = alive
// 0 = dead

// rules:
// A. any live cell with fewer than two live neighbours dies, as if caused by underpopulation.
// B. any live cell with two or three live neighbours lives on to the next generation.
// C. any live cell with more than four live neighbours dies, as if by overpopulation.
// D. any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.

// 1. any live cell with two or three live neighbours survives.
// 2. any dead cell with three live neighbours becomes a live cell.
// 3. all other live cells die in the next generation. similarly, all other dead cells stay dead.

// new live cells must have the arithmetical average of the immediate neighbours
// board must be floating point

// board NxN with infinite boundaries
// (0,0) is the upper left corner and (N-1,N-1) is the lower right corner
// live cell has value greater than 0.0

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

typedef struct cell
{
    int x;
    int y;
} cell;

#define board_size 2048+2
#define number_of_iterations 2000

void allocate_board(float ***grid);
void free_board(float **grid);
void initialize_board(float **grid);
void execute_iterations(float **grid, float **newgrid, int iterations);
void compute_live_cells(float **grid);
struct cell *get_neighbors(float **grid, int i, int j);
void update_borders(float **grid);
float average_neighbors_value(float **grid, int i, int j);


int main(int argc, char **argv)
{   
    float **grid, **newgrid;
    allocate_board(&grid);
    allocate_board(&newgrid);

    initialize_board(grid);
    
    execute_iterations(grid, newgrid, number_of_iterations);

    printf("final board:\n");
    compute_live_cells(grid);

    free_board(grid);
    free_board(newgrid);

    return 0;
}

// function to allocate board
void allocate_board(float ***grid)
{
    // allocate memory for the board
    *grid = (float **)malloc(board_size * sizeof(float *));
    for (int i = 0; i < board_size; i++)
    {
        (*grid)[i] = (float *)malloc(board_size * sizeof(float));
    }
}

// function to free board
void free_board(float **grid)
{
    // free memory for the board
    for (int i = 0; i < board_size; i++)
    {
        free(grid[i]);
    }
    free(grid);
}

// function to initialize board
void initialize_board(float **grid)
{
    for(int i = 0; i < board_size; i++)
    {
        for(int j = 0; j < board_size; j++)
        {
            grid[i][j] = 0.0;
        }
    }

    // initialize the board from position (1,1) with glider pattern
    // and a R-pentomino pattern in (10, 30)
    grid[1+1][2+1] = 1.0;
    grid[2+1][3+1] = 1.0;
    grid[3+1][1+1] = 1.0;
    grid[3+1][2+1] = 1.0;
    grid[3+1][3+1] = 1.0;

    grid[10+1][31+1] = 1.0;
    grid[10+1][32+1] = 1.0;
    grid[11+1][30+1] = 1.0;
    grid[11+1][31+1] = 1.0;
    grid[12+1][31+1] = 1.0;

    // update borders
    update_borders(grid);
}

// function to return all neighbors of a cell
struct cell *get_neighbors(float **grid, int i, int j)
{
    struct cell *neighbors = (struct cell *)malloc(8 * sizeof(struct cell));

    // clear neighbors
    for(int k = 0; k < 8; k++)
    {
        neighbors[k].x = 0;
        neighbors[k].y = 0;
    }

    // get positions
    neighbors[0].x = i - 1;
    neighbors[0].y = j - 1;
    neighbors[1].x = i - 1;
    neighbors[1].y = j;
    neighbors[2].x = i - 1;
    neighbors[2].y = j + 1;
    neighbors[3].x = i;
    neighbors[3].y = j - 1;
    neighbors[4].x = i;
    neighbors[4].y = j + 1;
    neighbors[5].x = i + 1;
    neighbors[5].y = j - 1;
    neighbors[6].x = i + 1;
    neighbors[6].y = j;
    neighbors[7].x = i + 1;
    neighbors[7].y = j + 1;

    return neighbors;
}

void update_borders(float **grid)
{
    // update upper border
    for(int i = 1; i < board_size - 1; i++)
    {
        grid[0][i] = grid[board_size - 2][i];
    }
    // update lower border
    for(int i = 1; i < board_size - 1; i++)
    {
        grid[board_size - 1][i] = grid[1][i];
    }
    // update left border
    for(int i = 1; i < board_size - 1; i++)
    {
        grid[i][0] = grid[i][board_size - 2];
    }
    // update right border
    for(int i = 1; i < board_size - 1; i++)
    {
        grid[i][board_size - 1] = grid[i][1];
    }
    // update corners
    grid[0][0] = grid[board_size - 2][board_size - 2];
    grid[0][board_size - 1] = grid[board_size - 2][1];
    grid[board_size - 1][0] = grid[1][board_size - 2];
    grid[board_size - 1][board_size - 1] = grid[1][1];
}

void execute_iterations(float **grid , float **newgrid, int iterations)
{
    for(int i = 0; i < iterations; i++)
    {
        for(int j = 1; j < board_size - 1; j++)
        {
            for(int k = 1; k < board_size - 1; k++)
            {
                struct cell *neighbors;
                int number_of_neighbors = 0;

                // clear neighbors
                for(int l = 0; l < 8; l++)
                {
                    neighbors[l].x = 0;
                    neighbors[l].y = 0;
                }

                // get positions
                neighbors = get_neighbors(grid, j, k);

                // count neighbors
                for(int l = 0; l < 8; l++)
                {
                    if(grid[neighbors[l].x][neighbors[l].y] > 0.0)
                    {
                        number_of_neighbors++;
                    }
                }

                if(grid[j][k] > 0.0)
                {
                    if(number_of_neighbors == 2 || number_of_neighbors == 3)
                    {
                        newgrid[j][k] = grid[j][k];
                    }
                    else
                    {
                        newgrid[j][k] = 0.0;
                    }
                }
                else
                {
                    if(number_of_neighbors == 3)
                    {   // calculate average of neighbors
                        newgrid[j][k] = average_neighbors_value(grid, j, k);
                    }
                    else
                    {
                        newgrid[j][k] = 0.0;
                    }
                }   

                // update borders
                update_borders(newgrid);

                // free neighbors
                free(neighbors);
            }
        }
        // copy newgrid to grid
        for(int j = 0; j < board_size; j++)
        {
            for(int k = 0; k < board_size; k++)
            {
                grid[j][k] = newgrid[j][k];
            }
        }
        printf("iteration: %d\n", i);
        compute_live_cells(grid);
        break;
    }
}

void compute_live_cells(float **grid)
{
    int live_cells = 0;
    for (int i = 1; i < board_size - 1; i++)
    {
        for (int j = 1; j < board_size - 1; j++)
        {
            if (grid[i][j] > 0.0)
            {
                live_cells++;
            }
        }
    }
    printf("live cells: %d\n", live_cells); 
    return;  
}

float average_neighbors_value(float **grid, int i, int j)
{
    struct cell *neighbors = (struct cell *)malloc(8 * sizeof(struct cell));
    float average = 0.0;

    // clear neighbors
    for(int i = 0; i < 8; i++)
    {
        neighbors[i].x = 0;
        neighbors[i].y = 0;
    }

    // get positions
    neighbors = get_neighbors(grid, i, j);

    // calculate average of neighbors
    for(int i = 0; i < 8; i++)
    {
        average += grid[neighbors[i].x][neighbors[i].y];
    }

    // divide by 8
    average /= 8.0;

    // free neighbors
    free(neighbors);

    return average;
}
