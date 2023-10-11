#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// variaveis globais
int sum = 0;
int target_sum = 100;

// funcoes do servidor e cliente
void critical_section(int id);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Uso: %s <N>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);

    #pragma omp parallel num_threads(num_threads)
    {
        int id = omp_get_thread_num();
        
        while (sum < target_sum)
        {
            critical_section(id);
        }
    }

    return 0;
}

// secao critica
void critical_section(int id)
{
    if (sum >= target_sum)
        return;

    printf("thread %d: critical section\n", id);
    sleep(1);
    sum += 1;
    printf("thread %d: sum = %d\n", id, sum);
}
