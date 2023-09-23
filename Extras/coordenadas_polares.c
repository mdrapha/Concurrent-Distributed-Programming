#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>

int N, NT;
float *buffer_theta1;
float *buffer_theta2;
float *buffer_r1;
float *buffer_r2;
float *buffer_dist;

void init_vars(){
    int pot = pow(10, N);

	buffer_theta1 = (float*)malloc(sizeof(int)*pot);
    buffer_theta2 = (float*)malloc(sizeof(int)*pot);
    buffer_r1 = (float*)malloc(sizeof(int)*pot);
    buffer_r2 = (float*)malloc(sizeof(int)*pot);
    buffer_dist = (float*)malloc(sizeof(int)*pot);
    
    srand(time(NULL));

    for (int i = 0; i < pot; i++) {
        buffer_theta1[i] = rand() % 360;
        buffer_theta2[i] = rand() % 360;
        buffer_r1[i] = rand() % 100;
        buffer_r2[i] = rand() % 100;
    }

}


void cria_threads(pthread_t *t, int *t_id, int n, void *(*func)(void*)){
	int i;
	for (i=0; i<n; i++) {
		t_id[i] = i;
		pthread_create(&t[i], NULL, func, &t_id[i]);
	}
	return;
}

void espera_threads(pthread_t *t, int n) {
	int i;
	for (i=0; i<n; i++) {
		pthread_join(t[i], NULL);
	}
}

// Calcula os valores da distância euclidiana
void * calc_dist(void *arg) {
    int pot = pow(10, N);
    int id = *(int*)arg;
    int aux = floor(pot/NT);
    int inicio = aux*id;
    int fim = aux;

	if (id == NT-1) fim = pot;
	else fim = fim * (id+1);

	printf("Thread %d: inicio = %d, fim = %d\n", id, inicio, fim);

	for(int i = inicio; i < fim; i++) {
        buffer_dist[i] = sqrt(pow(buffer_r1[i], 2) + pow(buffer_r2[i], 2) - 2*buffer_r1[i]*buffer_r2[i]*cos(buffer_theta1[i] - buffer_theta2[i]));
	}
	pthread_exit(NULL);
}

int main(int argc, char **argv) {
	if (argc != 3) {
		printf("%s <N> <NT>\n", argv[0]);
		return 1;
	}
	N = atoi(argv[1]);
	NT = atoi(argv[2]);
	
	pthread_t tp[NT];
	int id_p[NT];
    struct timeval start, end;

	init_vars();

    gettimeofday(&start, NULL);
	cria_threads(tp, id_p, NT, calc_dist);
	espera_threads(tp, NT);
    gettimeofday(&end, NULL);

    double time = (double)(end.tv_usec - start.tv_usec) / 1000000 + (double)(end.tv_sec - start.tv_sec);

    printf("Distância euclidiana calculada para 10^%d pontos\n", N);
    printf("Tempo de execução: %lf\n", time);

	return 0;
}