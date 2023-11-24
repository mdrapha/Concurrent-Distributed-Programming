/*
 * MPI implementation of the Game of Life
 *
 
 * Authors: Marco Antonio Coral dos Santos - 158467
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
#include <mpi.h>



// Constants of the program
const int board_size = 2048;
#define number_of_iterations 2000

float** allocate_board();
void free_board(float **grid);
void initialize_board(float **grid);
void execute_iterations(float **sub_grid, float **sub_newgrid, int rows_per_process, int board_size);
void exchange_boundaries(float **sub_grid, int world_rank, int world_size, int rows_per_process, int board_size);
void compute_live_cells(float **grid);
int get_neighbors(float **grid, int i, int j, int rows_per_process, int board_size);
float average_neighbors_value(float **grid, int i, int j);
void show_50_50_grid(float **grid);


int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Calculate the number of rows per process
    int base_rows_per_process = board_size / world_size;
    int extra_rows = board_size % world_size;
    int rows_per_process = base_rows_per_process + (world_rank < extra_rows);

    // Prepare for uneven distribution of rows
    int *sendcounts = malloc(world_size * sizeof(int));
    int *displs = malloc(world_size * sizeof(int));
    int sum = 0;
    for (int i = 0; i < world_size; i++) {
        sendcounts[i] = (base_rows_per_process + (i < extra_rows)) * board_size;
        displs[i] = sum;
        sum += sendcounts[i];
    }

    // Allocate memory for the full grid in the root process
    float **full_grid = NULL;
    if (world_rank == 0) {
        full_grid = allocate_board(board_size, board_size);
        initialize_board(full_grid);
    }

    // Allocate memory for sub-grids in all processes (+2 for boundary rows)
    float **sub_grid = allocate_board(rows_per_process + 2, board_size);
    float **sub_newgrid = allocate_board(rows_per_process + 2, board_size);

    // Scatter the rows of the board using MPI_Scatterv
    MPI_Scatterv(full_grid ? &full_grid[0][0] : NULL, sendcounts, displs, MPI_FLOAT,
                 &sub_grid[1][0], rows_per_process * board_size, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Main computation loop
    for (int iter = 0; iter < number_of_iterations; ++iter) {
        // Perform computation on sub_grid
        execute_iterations(sub_grid, sub_newgrid, rows_per_process, board_size);

        // Exchange boundary information with neighboring processes
        exchange_boundaries(sub_grid, world_rank, world_size, rows_per_process, board_size);

        // Swap pointers for next iteration
        float **temp = sub_grid;
        sub_grid = sub_newgrid;
        sub_newgrid = temp;

        printf("iter: %d\n", iter);


    }

    // Gather all sub-grids back to the root process using MPI_Gatherv
    if (world_rank == 0) {
        float **final_grid = allocate_board(board_size, board_size);
        MPI_Gatherv(&sub_grid[1][0], rows_per_process * board_size, MPI_FLOAT,
                    full_grid ? &full_grid[0][0] : NULL, sendcounts, displs, MPI_FLOAT,
                    0, MPI_COMM_WORLD);

        // Display or process the final grid
        // ...

        free_board(final_grid);
    } else {
        MPI_Gatherv(&sub_grid[1][0], rows_per_process * board_size, MPI_FLOAT,
                    NULL, NULL, NULL, MPI_FLOAT, 0, MPI_COMM_WORLD);
    }

    free(sendcounts);
    free(displs);
    free_board(sub_grid);
    free_board(sub_newgrid);

    if (world_rank == 0) {
        free_board(full_grid);
    }

    MPI_Finalize();
    return 0;
}

void exchange_boundaries(float **sub_grid, int world_rank, int world_size, int rows_per_process, int board_size) {
    MPI_Status status;

    // Calculate the ranks of the next and previous processes in the ring
    int next_rank = (world_rank + 1) % world_size;
    int prev_rank = (world_rank - 1 + world_size) % world_size;

    // Exchange the top boundary row with the previous process and the bottom boundary row with the next process
    if (world_rank % 2 == 0) {
        // Even ranks send first and then receive
        MPI_Send(sub_grid[0], board_size, MPI_FLOAT, prev_rank, 0, MPI_COMM_WORLD);
        MPI_Send(sub_grid[rows_per_process - 1], board_size, MPI_FLOAT, next_rank, 0, MPI_COMM_WORLD);
        MPI_Recv(sub_grid[rows_per_process], board_size, MPI_FLOAT, next_rank, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(sub_grid[-1], board_size, MPI_FLOAT, prev_rank, 0, MPI_COMM_WORLD, &status);
    } else {
        // Odd ranks receive first and then send
        MPI_Recv(sub_grid[rows_per_process], board_size, MPI_FLOAT, next_rank, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(sub_grid[-1], board_size, MPI_FLOAT, prev_rank, 0, MPI_COMM_WORLD, &status);
        MPI_Send(sub_grid[0], board_size, MPI_FLOAT, prev_rank, 0, MPI_COMM_WORLD);
        MPI_Send(sub_grid[rows_per_process - 1], board_size, MPI_FLOAT, next_rank, 0, MPI_COMM_WORLD);
    }
}

float** allocate_board(int rows, int cols) {
    float **grid = (float **)malloc(rows * sizeof(float *));
    for (int i = 0; i < rows; i++) {
        grid[i] = (float *)malloc(cols * sizeof(float));
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

int get_neighbors(float **grid, int i, int j, int rows_per_process, int board_size) {
    int number_of_neighbors = 0;

    for (int di = -1; di <= 1; di++) {
        for (int dj = -1; dj <= 1; dj++) {
            if (di == 0 && dj == 0) continue; // Skip the cell itself

            int ni = i + di, nj = (j + dj + board_size) % board_size; // Wrap around horizontally

            if (ni >= 0 && ni < rows_per_process + 2) { // Check vertical bounds
                number_of_neighbors += (grid[ni][nj] > 0.0);
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

void execute_iterations(float **sub_grid, float **sub_newgrid, int rows_per_process, int board_size) {
    for (int i = 1; i <= rows_per_process; ++i) {
        for (int j = 0; j < board_size; ++j) {
            int num_neighbors = get_neighbors(sub_grid, i, j, rows_per_process, board_size);

            // Game of Life rules
            if (sub_grid[i][j] > 0.0) { // Cell is alive
                if (num_neighbors < 2 || num_neighbors > 3) {
                    sub_newgrid[i][j] = 0.0; // Cell dies
                } else {
                    sub_newgrid[i][j] = 1.0; // Cell stays alive
                }
            } else { // Cell is dead
                if (num_neighbors == 3) {
                    sub_newgrid[i][j] = 1.0; // Cell becomes alive
                } else {
                    sub_newgrid[i][j] = 0.0; // Cell stays dead
                }
            }
        }
    }

    // Swap the sub-grids after the iteration
    float **temp = sub_grid;
    sub_grid = sub_newgrid;
    sub_newgrid = temp;

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


