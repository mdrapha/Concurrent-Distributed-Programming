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
// Including OpenMP for parallelization
#include <omp.h>

#define board_size 2048
#define number_of_iterations 2000

float** allocate_board();
void free_board(float **grid);
void initialize_board(float **grid);
void execute_iterations(float **grid, float **newgrid, int iterations);
void compute_live_cells(float **grid);
int get_neighbors(float **grid, int i, int j);


int main(int argc, char **argv)
{   
    omp_set_nested(1);
    float **grid, **newgrid;
    double begin, end;
    begin = omp_get_wtime();
    grid = allocate_board();
    newgrid = allocate_board();
    initialize_board(grid);

    execute_iterations(grid, newgrid, number_of_iterations);
    compute_live_cells(grid);

    free_board(grid);
    free_board(newgrid);
    end = omp_get_wtime();
    printf("time: %f\n", end - begin);

    return 0;
}

// function to allocate board
float** allocate_board()
{
    float ** grid;
    // allocate memory for the board
    grid = (float **)malloc(board_size * sizeof(float *));
    #pragma omp parallel for
    for (int i = 0; i < board_size; i++)
    {
        grid[i] = (float *)malloc(board_size * sizeof(float));
    }

    
    return grid;
}

// function to free board
void free_board(float **grid)
{
    // free memory for the board
    #pragma omp parallel for
    for (int i = 0; i < board_size; i++)
    {
        free(grid[i]);
    }
    free(grid);
}

// function to initialize board
void initialize_board(float **grid)
{   
    printf("initializing board...\n");
    // clear the board
    #pragma omp parallel for
    for(int i = 0; i < board_size; i++)
    {
        for(int j = 0; j < board_size; j++)
        {
            grid[i][j] = 0.0;
        }
    }
    printf("board cleared\n");

    // initialize the board from position (1,1) with glider pattern
    // and a R-pentomino pattern in (10, 30)
    grid[1][2] = 1.0;
    grid[2][3] = 1.0;
    grid[3][1] = 1.0;
    grid[3][2] = 1.0;
    grid[3][3] = 1.0;

    grid[10][31] = 1.0;
    grid[10][32] = 1.0;
    grid[11][30] = 1.0;
    grid[11][31] = 1.0;
    grid[12][31] = 1.0;

}

int get_neighbors(float **grid, int i, int j){
    int number_of_neighbors = 0;

    for(int k = i - 1; k <= i + 1; k++)
    {
        for(int l = j - 1; l <= j + 1; l++)
        {   

            int k_aux = k;
            int l_aux = l;

            if(k == i && l == j)
            {
                continue;
            }

            if(k == -1)
            {
                k_aux = board_size - 1;
            }
            else if (k == board_size)
            {
                k_aux = 0;
            }
            
            if(l == -1){
                l_aux = board_size - 1;
            }
            else if(l == board_size)
            {
                l_aux = 0;
            }

            if(grid[k_aux][l_aux] > 0.0)
            {   
                number_of_neighbors++;
            }
        }
    }

    return number_of_neighbors;
}

float average_neighbors_value(float** grid, int i,  int j){
    float average = 0.0;
    
    for(int k = i - 1; k <= i + 1; k++)
    {
        for(int l = j - 1; l <= j + 1; l++)
        {   

            int k_aux = k;
            int l_aux = l;

            if(k == -1)
            {
                k_aux = board_size - 1;
            }
            else if (k == board_size){
                k_aux = 0;
            }

            if(l == -1){
                l_aux = board_size - 1;
            }
            else if(l == board_size){
                l_aux = 0;
            }
            
            average += (float)grid[k_aux][l_aux];  
        }
    }

    return (float)average / (float)8.0;
}

void execute_iterations(float **grid , float **newgrid, int iterations)
{
    for(int i = 0; i < iterations; i++)
    {   
        #pragma omp parallel for collapse(2)
        for(int j = 0; j < board_size; j++)
        {
            
            for(int k = 0; k < board_size; k++)
            {
                
                // get neighbors
                int number_of_neighbors = get_neighbors(grid, j, k);


                if(grid[j][k] > 0.0)
                {
                    if(number_of_neighbors == 2 || number_of_neighbors == 3)
                    {
                        newgrid[j][k] = 1;
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
            }
        }
        
        //printf("Before copy\n");
        // copy newgrid to grid
        //printf("After copy\n");
        printf("iteration: %d ", i);
        compute_live_cells(grid);
        // Alternar grids
        float** temp = grid;
        grid = newgrid;
        newgrid = temp;
        
    }
    compute_live_cells(grid);
}

void compute_live_cells(float **grid)
{
    int live_cells = 0;
    #pragma omp parallel for reduction(+:live_cells)
    for (int i = 0; i < board_size; i++)
    {
        for (int j = 0; j < board_size; j++)
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


