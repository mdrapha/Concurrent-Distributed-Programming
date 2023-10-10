#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <unistd.h>

int SOMA = 0;

void cliente(int N) {
    for (int i = 0; i < N; i++) {
        {
            int local = SOMA;

            // Sleep aleatório para simular trabalho
            usleep(rand() % 2);
            printf("Thread %d na seção crítica. SOMA = %d\n", omp_get_thread_num(), SOMA);
            SOMA = local + 1;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <numero de threads> <N>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    int N = atoi(argv[2]);

    if (num_threads < 2 || N <= 0) {
        printf("Número de threads deve ser pelo menos 2 e N deve ser maior que 0.\n");
        return 1;
    }

    omp_set_num_threads(num_threads);

    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        if (thread_id == 0) {
            // Thread 0 é o servidor
            printf("Thread Servidor iniciada.\n");
        } else {
            // Demais threads são clientes
            printf("Thread Cliente %d iniciada.\n", thread_id);
            #pragma omp critical
            cliente(N);
        }
    }

    printf("SOMA final: %d\n", SOMA);

    return 0;
}
