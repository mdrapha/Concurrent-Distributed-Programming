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

// function declarations
float** allocate_board();
void free_board(float **grid);
void initialize_board(float **grid);
void execute_iterations(float **grid, float **newgrid, int iterations);
void compute_live_cells(float **grid);
int get_neighbors(float **grid, int i, int j);
float average_neighbors_value(float **grid, int i, int j);
void show_50_50_grid(float **grid);

int main(int argc, char **argv)
{   
    omp_set_nested(1);

    float **grid, **newgrid; // board and new board
    double begin, end; // time variables

    begin = omp_get_wtime(); // get time

    grid = allocate_board(); // allocate board 
    newgrid = allocate_board(); // allocate new board

    initialize_board(grid); // initialize board

    execute_iterations(grid, newgrid, number_of_iterations); // execute iterations
    compute_live_cells(grid);   // compute final live cells

    free_board(grid); // free board
    free_board(newgrid); // free new board

    end = omp_get_wtime(); // get time
    printf("time: %f\n", end - begin); // print time

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

// function to get number of neighbors
int get_neighbors(float **grid, int i, int j)
{
    // get number of neighbors
    int number_of_neighbors = 0;

    // check neighbors
    for(int k = i - 1; k <= i + 1; k++)
    {
        for(int l = j - 1; l <= j + 1; l++)
        {   
            // get neighbors
            int k_aux = k;
            int l_aux = l;

            if(k == i && l == j) // skip current cell if it is not a neighbor
            {
                continue;
            }

            if(k == -1) // if k is -1, then k_aux is the last position of the board
            {
                k_aux = board_size - 1;
            }
            else if (k == board_size) // if k is board_size, then k_aux is the first position of the board
            {
                k_aux = 0;
            }
            
            if(l == -1) // if l is -1, then l_aux is the last position of the board
            {
                l_aux = board_size - 1;
            }
            else if(l == board_size) // if l is board_size, then l_aux is the first position of the board
            {
                l_aux = 0;
            }

            if(grid[k_aux][l_aux] > 0.0) // if neighbor is alive, then increment number of neighbors
            {   
                number_of_neighbors++;
            }
        }
    }

    return number_of_neighbors;
}

// function to get average of neighbors
float average_neighbors_value(float** grid, int i,  int j)
{
    float average = 0.0;
    
    // get average of neighbors
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
            
            average += (float)grid[k_aux][l_aux];  
        }
    }

    return (float)average / (float)8.0;
}

// function to execute iterations
void execute_iterations(float **grid , float **newgrid, int iterations)
{
    for(int i = 0; i < iterations; i++)
    {   
        #pragma omp parallel for collapse(2) // collapse to parallelize nested for loops
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
        
        // print iteration
        printf("iteration: %d ", i);

        // compute live cells
        compute_live_cells(grid);

        // swap grids
        float** temp = grid;
        grid = newgrid;
        newgrid = temp;

        if(iterations < 5)
        {
            show_50_50_grid(grid);
        }
    }
    compute_live_cells(grid);
}

// function to compute live cells
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

// function to show 50x50 grid
void show_50_50_grid(float **grid)
{
    for(int i = 0; i < board_size; i++)
    {
        for(int j = 0; j < board_size; j++)
        {
            if (grid[i][j] > 0.0)
            {
                printf("1 ");
            }
            else
            {
                printf("0 ");
            }
        }
        printf("\n");
    }
    printf("\n");
}
