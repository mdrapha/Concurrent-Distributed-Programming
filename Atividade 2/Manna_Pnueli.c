#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define NUM_ITERATIONS 1000000000

int SOMA = 0; // Variável global compartilhada

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Uso: %s <num_threads>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    if (num_threads <= 0) {
        printf("O número de threads deve ser maior que 0.\n");
        return 1;
    }

    int i;

    omp_set_num_threads(num_threads);

    #pragma omp parallel private(i)
    {
        int thread_id = omp_get_thread_num();
        int local_sum = 0;

        #pragma omp for
        for (i = 0; i < NUM_ITERATIONS; i++) {
            #pragma omp critical
            {
                int local = SOMA;
                SOMA = local + 1;
            }
        }

        #pragma omp critical
        {
            printf("Thread %d: Local sum = %d\n", thread_id, local_sum);
        }
    }

    printf("Resultado final da variável SOMA: %d\n", SOMA);

    return 0;
}
