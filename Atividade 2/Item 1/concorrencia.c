#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_THREADS 100

int SOMA = 0;
int thread_id_counter = 0;

void *incrementSoma(void *arg) {
    int iterations = *((int *)arg);
    int thread_id;

    // Obter um ID de thread sequencial e incrementar o contador
    thread_id = thread_id_counter++;
    
    for (int i = 0; i < iterations; i++) {
        SOMA++;
        printf("thread %d \t| incremented: %d\n", thread_id, SOMA);
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: %s <número de threads> <número máximo de iterações>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    int iterations_per_thread = atoi(argv[2]);

    if (num_threads > MAX_THREADS) {
        printf("O número de threads excede o máximo permitido (%d)\n", MAX_THREADS);
        return 1;
    }

    pthread_t threads[MAX_THREADS];

    for (int i = 0; i < num_threads; i++) {
        int rc = pthread_create(&threads[i], NULL, incrementSoma, (void *)&iterations_per_thread);
        if (rc) {
            printf("Erro ao criar a thread %d; código de retorno: %d\n", i, rc);
            return -1;
        }
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\nfinal sum: %d\n", SOMA);

    return 0;
}