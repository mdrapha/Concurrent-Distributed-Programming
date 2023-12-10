#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define board_size 2048
#define number_of_iterations 2000

float** allocate_subboard(int rows);
void free_subboard(float **grid, int rows);
void initialize_subboard(float **grid, int start_row, int rows);
void execute_iterations(float **grid, float **newgrid, int start_row, int rows, int rank, int size);
int get_neighbors(float **grid, int i, int j, int rows);
float average_neighbors_value(float **grid, int i, int j, int rows);
void compute_live_cells(float **grid, int rows);

int main(int argc, char **argv)
{
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int rows_per_process = board_size / size;
    int extra_rows = board_size % size;
    int start_row = rank * rows_per_process;
    int rows = (rank < extra_rows) ? rows_per_process + 1 : rows_per_process;

    float **grid = allocate_subboard(rows);
    float **newgrid = allocate_subboard(rows);

    initialize_subboard(grid, start_row, rows);

    double start_time = MPI_Wtime();
    execute_iterations(grid, newgrid, start_row, rows, rank, size);
    double end_time = MPI_Wtime();

    if (rank == 0) {
        printf("Total time: %f seconds\n", end_time - start_time);
    }

    free_subboard(grid, rows);
    free_subboard(newgrid, rows);

    MPI_Finalize();
    return 0;
}

float** allocate_subboard(int rows) {
    float **grid = (float **)malloc((rows + 2) * sizeof(float *)); // Allocate 2 extra rows
    for (int i = 0; i < rows + 2; i++) {
        grid[i] = (float *)malloc(board_size * sizeof(float));
    }
    return grid;
}


void free_subboard(float **grid, int rows) {
    for (int i = 0; i < rows; i++) {
        free(grid[i]);
    }
    free(grid);
}

void initialize_subboard(float **grid, int start_row, int rows) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < board_size; j++) {
            grid[i][j] = 0.0;
        }
    }

    // Initialize with specific patterns only if they fall within this subboard
    if (start_row <= 1 && start_row + rows > 1) {
        // Glider pattern
        grid[1 - start_row][2] = 1.0;
        grid[2 - start_row][3] = 1.0;
        grid[3 - start_row][1] = 1.0;
        grid[3 - start_row][2] = 1.0;
        grid[3 - start_row][3] = 1.0;
    }

    if (start_row <= 10 && start_row + rows > 10) {
        // R-pentomino pattern
        grid[10 - start_row][31] = 1.0;
        grid[10 - start_row][32] = 1.0;
        grid[11 - start_row][30] = 1.0;
        grid[11 - start_row][31] = 1.0;
        grid[12 - start_row][31] = 1.0;
    }
}

int get_neighbors(float **grid, int i, int j, int rows) {
    int number_of_neighbors = 0;

    for (int k = i - 1; k <= i + 1; k++) {
        for (int l = j - 1; l <= j + 1; l++) {
            if (k == i && l == j) continue;

            int k_aux = (k < 0) ? rows - 1 : (k >= rows) ? 0 : k;
            int l_aux = (l < 0) ? board_size - 1 : (l >= board_size) ? 0 : l;

            if (grid[k_aux][l_aux] > 0.0) {
                number_of_neighbors++;
            }
        }
    }

    return number_of_neighbors;
}

float average_neighbors_value(float **grid, int i, int j, int rows) {
    float average = 0.0;

    for (int k = i - 1; k <= i + 1; k++) {
        for (int l = j - 1; l <= j + 1; l++) {
            int k_aux = (k < 0) ? rows - 1 : (k >= rows) ? 0 : k;
            int l_aux = (l < 0) ? board_size - 1 : (l >= board_size) ? 0 : l;

            average += grid[k_aux][l_aux];
        }
    }

    return average / 8.0;
}

void compute_live_cells(float **grid, int rows) {
    int live_cells = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < board_size; j++) {
            if (grid[i][j] > 0.0) {
                live_cells++;
            }
        }
    }
    printf("live cells: %d\n", live_cells);
}

void execute_iterations(float **grid, float **newgrid, int start_row, int rows, int rank, int size) {


    MPI_Status status;

    for (int iter = 0; iter < number_of_iterations; iter++) {
        // Send and receive border rows
        if (rank > 0) {
            MPI_Sendrecv(grid[1], board_size, MPI_FLOAT, rank - 1, 0,
                         grid[0], board_size, MPI_FLOAT, rank - 1, 0,
                         MPI_COMM_WORLD, &status);
        }
        if (rank < size - 1) {
            MPI_Sendrecv(grid[rows], board_size, MPI_FLOAT, rank + 1, 0,
                         grid[rows + 1], board_size, MPI_FLOAT, rank + 1, 0,
                         MPI_COMM_WORLD, &status);
        }

        // Perform Game of Life iteration on each cell of the sub-board
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < board_size; j++) {
                int num_neighbors = get_neighbors(grid, i, j, rows);
                float cell = grid[i][j];

                // Game of Life rules
                if (cell > 0.0) {
                    // Cell is alive: It remains alive with 2 or 3 neighbors
                    newgrid[i][j] = (num_neighbors == 2 || num_neighbors == 3) ? 1.0 : 0.0;
                } else {
                    // Cell is dead: It becomes alive if exactly 3 neighbors are alive
                    newgrid[i][j] = (num_neighbors == 3) ? average_neighbors_value(grid, i, j, rows) : 0.0;
                }
            }
        }

        // Swap the old and new grids
        float **temp = grid;
        grid = newgrid;
        newgrid = temp;

        // Lets just the rank 0 process do it
        if (rank == 0)
        compute_live_cells(grid, rows);

    }
}
