#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <unistd.h>
#include <time.h>

int SOMA = 0;
int N = 0;
int respond = 0;
int request = 0;

void cliente(int id) {
    while (1) {
        while (respond != id) {
            request = id;
        }

        // Simular trabalho aleatório
        usleep(rand() % 2000); // Alterado para valores maiores para facilitar a observação
        printf("Thread %d fora da seção crítica. SOMA = %d\n", id, SOMA);

        SeccaoCritica(id);

        if (SOMA >= N) {
            break;
        }

        request = 0;
    }
}

void SeccaoCritica(int id) {
    int local = SOMA;
    // Simular trabalho aleatório
    usleep(rand() % 2000);
    printf("Thread %d na seção crítica. SOMA = %d\n", id, SOMA);
    SOMA = local + 1;
    respond = 0;
}

void servidor(int id) {
    while (1) {
        while (request != 0) {
            // Aguardar até que request seja 0
        }

        request = 0;
        while (request == 0) {
            respond = respond; // Adicionado para evitar loop infinito
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <numero de threads> <N>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    N = atoi(argv[2]);

    if (num_threads < 2 || N <= 0) {
        printf("Número de threads deve ser pelo menos 2 e N deve ser maior que 0.\n");
        return 1;
    }

    omp_set_num_threads(num_threads);

    srand(time(NULL)); // Inicializar a semente para rand()

    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        if (thread_id == 0) {
            // Thread 0 é o servidor
            printf("Thread Servidor iniciada.\n");
            servidor(thread_id);
        } else {
            // Demais threads são clientes
            printf("Thread Cliente %d iniciada.\n", thread_id);
            #pragma omp critical
            cliente(thread_id);
        }
    }

    printf("SOMA final: %d\n", SOMA);

    return 0;
}
