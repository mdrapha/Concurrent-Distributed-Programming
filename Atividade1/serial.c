/*
 * Serial implementation of the Game of Life
 *
 * Authors: Eduardo Verissimo Faccio - 148859 
 *          Marco Antonio Coral dos Santos - 158467
 *          Raphael Damasceno Rocha de Moraes - 156380
 * 
 * Professor: Alvaro Luiz Fazenda
 * 
 * */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>


// Constants of the program
#define board_size 2048
#define number_of_iterations 2000

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
    struct timeval start, finish, begin, end;
    
    gettimeofday(&start, NULL); // start time of the program

    float **grid, **newgrid;

    // allocate memory for the board 
    grid = allocate_board();
    newgrid = allocate_board();

    initialize_board(grid); // initialize board
    
    gettimeofday(&begin, NULL); // start time of the algorithm

    execute_iterations(grid, newgrid, number_of_iterations); // execute iterations
    compute_live_cells(grid); // compute live cells at the end of the program

    gettimeofday(&end, NULL); // end time of the algorithm

    // free memory for the board
    free_board(grid); 
    free_board(newgrid);

    gettimeofday(&finish, NULL); // end time of the program

    double running_time = (end.tv_sec - begin.tv_sec) +
                     (end.tv_usec - begin.tv_usec) / 1000000.0;
    printf("Running time: \t%f seconds\n", running_time);
    
    double elapsed = (finish.tv_sec - start.tv_sec) +
                     (finish.tv_usec - start.tv_usec) / 1000000.0;
    printf("Total time: \t%f seconds\n", elapsed);

    return 0;
}


float** allocate_board()
{
    float ** grid;
    
    grid = (float **)malloc(board_size * sizeof(float *));
    for (int i = 0; i < board_size; i++)
    {
        grid[i] = (float *)malloc(board_size * sizeof(float));
    }

    
    return grid;
}


void free_board(float **grid)
{
    // free memory for the board
    for (int i = 0; i < board_size; i++)
    {
        free(grid[i]);
    }
    free(grid);
}


void initialize_board(float **grid)
{   
    printf("initializing board...\n");
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

            if(k == i && l == j) // skip the cell itself
            {
                continue;
            }
            
            // if the cell is in the border, get the value from the other side
            if(k == -1) // if the cell is in the upper border
            {
                k_aux = board_size - 1;
            }
            else if (k == board_size) // if the cell is in the lower border
            {
                k_aux = 0;
            }
            
            if(l == -1) // if the cell is in the left border
            { 
                l_aux = board_size - 1;
            }
            else if(l == board_size) // if the cell is in the right border
            {
                l_aux = 0;
            }

            // if the cell is alive, add to the number of neighbors
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

            // same idea as get_neighbors function
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
        for(int j = 0; j < board_size; j++)
        {
            for(int k = 0; k < board_size; k++)
            {
                
                // get the number of neighbors of the cell
                int number_of_neighbors = get_neighbors(grid, j, k);

                // apply the rules of the game
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

        // swap grids for the next iteration
        float** temp = grid;
        grid = newgrid;
        newgrid = temp;

        // show 50x50 grid for the first 5 iterations
        if(i < 5)
        {
            show_50_50_grid(grid);
        }
    }
    compute_live_cells(grid);
}

void show_50_50_grid(float **grid)
{
    for(int i = 0; i < 50; i++)
    {
        for(int j = 0; j < 50; j++)
        {
            if (grid[i][j] > 0.0)
            {
                printf("@ ");
            }
            else
            {
                printf(". ");
            }
        }
        printf("\n");
    }
    printf("\n");
}

void compute_live_cells(float **grid)
{
    int live_cells = 0;
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


