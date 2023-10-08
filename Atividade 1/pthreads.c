/*
 * Pthreads implementation of the Game of Life
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
#include <pthread.h>
#include <sys/time.h>

int NUM_THREADS;
#define board_size 2048
#define number_of_iterations 2000

typedef struct {
    int start_row;
    int end_row;
    float** grid;
    float** newgrid;
} ThreadData;

float** allocate_board();
void free_board(float **grid);
void initialize_board(float **grid);
void execute_iterations(float **grid, float **newgrid, int iterations);
void compute_live_cells(float **grid);
int get_neighbors(float **grid, int i, int j);
void* thread_work(void* args);
float average_neighbors_value(float **grid, int i, int j);
void show_50_50_grid(float **grid);

int main(int argc, char **argv)
{   
    struct timeval start, finish, begin, end;
    gettimeofday(&start, NULL); // start time of the program

    if(argc != 2)
    {
        printf("Usage: %s <number of threads>\n", argv[0]);
        exit(1);
    }

    // The number of threads is passed as an argument to the program
    NUM_THREADS = atoi(argv[1]);

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

// function to execute the work of each thread
void* thread_work(void* args) 
{
    ThreadData* data = (ThreadData*)args; // get the thread data
    for (int j = data->start_row; j < data->end_row; j++) // iterate over the rows
    {
        for (int k = 0; k < board_size; k++)  // iterate over the columns
        {
            int number_of_neighbors = get_neighbors(data->grid, j, k);
            if(data->grid[j][k] > 0.0)  // if the cell is alive
            {
                if(number_of_neighbors == 2 || number_of_neighbors == 3)  // if the cell has 2 or 3 neighbors, it survives
                {
                    data->newgrid[j][k] = 1;
                } 
                else  // if the cell has less than 2 or more than 3 neighbors, it dies
                {
                    data->newgrid[j][k] = 0.0;
                }
            } 
            else // if the cell is dead
            {
                if(number_of_neighbors == 3)  // if the cell has 3 neighbors, it becomes alive
                {
                    data->newgrid[j][k] = average_neighbors_value(data->grid, j, k);
                } 
                else // if the cell has less than 3 or more than 3 neighbors, it stays dead
                {
                    data->newgrid[j][k] = 0.0;
                }
            }
        }
    }
    return NULL;
}


// function to allocate board
float** allocate_board()
{
    float ** grid;
    // allocate memory for the board
    grid = (float **)malloc(board_size * sizeof(float *));
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

// function to get the number of neighbors of a cell
int get_neighbors(float **grid, int i, int j)
{
    int number_of_neighbors = 0;

    // check the 8 neighbors of the cell
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

            if(k == -1) // if the cell is in the border, get the value from the other side
            {
                k_aux = board_size - 1;
            }
            else if (k == board_size) // if the cell is in the border, get the value from the other side
            {
                k_aux = 0;
            }
            
            if(l == -1) // if the cell is in the border, get the value from the other side
            {
                l_aux = board_size - 1;
            }
            else if(l == board_size) // if the cell is in the border, get the value from the other side
            {
                l_aux = 0;
            }

            if(grid[k_aux][l_aux] > 0.0) // if the neighbor is alive
            {   
                number_of_neighbors++;
            }
        }
    }
 
    // printf("number of neighbors: %d\n", number_of_neighbors);
    return number_of_neighbors;
}

// function to get the average value of the neighbors of a cell
float average_neighbors_value(float** grid, int i,  int j)
{
    float average = 0.0; // average value of the neighbors
    
    // check the 8 neighbors of the cell
    for(int k = i - 1; k <= i + 1; k++)
    {
        for(int l = j - 1; l <= j + 1; l++)
        {   

            int k_aux = k;
            int l_aux = l;

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
            
            average += (float)grid[k_aux][l_aux]; 
        }
    }

    return (float)average / (float)8.0; // return the average value of the neighbors
}

// function to execute iterations
void execute_iterations(float **grid , float **newgrid, int iterations) 
{
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    for (int i = 0; i < iterations; i++) 
    {
        int rows_per_thread = board_size / NUM_THREADS;
        for (int t = 0; t < NUM_THREADS; t++) 
        {
            thread_data[t].start_row = t * rows_per_thread;
            thread_data[t].end_row = (t == NUM_THREADS - 1) ? board_size : (t + 1) * rows_per_thread;
            thread_data[t].grid = grid;
            thread_data[t].newgrid = newgrid;

            pthread_create(&threads[t], NULL, thread_work, &thread_data[t]); // create the threads
        }

        // wait for all the threads to finish
        for (int t = 0; t < NUM_THREADS; t++) 
        {
            pthread_join(threads[t], NULL); 
        }

        compute_live_cells(grid);
        float** temp = grid;
        grid = newgrid;
        newgrid = temp;

        if(i < 5)
        {
            show_50_50_grid(grid);
        }
    }
    compute_live_cells(grid);
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
