#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int SOMA = 0;
int turn = 0;
int *interested;

void secao_critica(int id) {
    int local = SOMA;
    sleep(rand() % 2);
    SOMA = local + 1;
    printf("Thread %d na seção crítica. SOMA = %d\n", id+1, SOMA);
}

void remainder(int id) {
    printf("Thread %d na seção restante.\n", id+1);
}

void manna_pnueli(int id, int N) {
    for (int i = 0; i < N; i++) {
        interested[id] = 1;
        while (interested[1 - id]) {
            while (interested[1 - id]) {
                while (turn != id) {
                    if (turn != id) {
                        interested[id] = 0;
                        while (turn != id) {}
                        interested[id] = 1;
                    }
                }  
            }

            
        }
        secao_critica(id);
        turn = 1 - id;
        interested[id] = 0;
        remainder(id);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <Número de Threads> <Valor de N>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    int N = atoi(argv[2]);

    if (num_threads < 2 || N <= 0) {
        printf("Número de threads deve ser pelo menos 2 e N deve ser maior que 0.\n");
        return 1;
    }

    interested = (int *)malloc(num_threads * sizeof(int));

    #pragma omp parallel num_threads(num_threads)
    {
        int id = omp_get_thread_num();
        manna_pnueli(id, N);
    }

    free(interested);

    return 0;
}
