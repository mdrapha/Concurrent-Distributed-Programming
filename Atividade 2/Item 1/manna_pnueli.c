#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

// variaveis globais
int sum = 0;
int respond = 0;
int request = 0;

// funcoes do servidor e cliente
void Server(int id);
void Client(int id);
void critical_section(int id);

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("Uso: %s <N>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);

    #pragma omp parallel num_threads(num_threads)
    {
        int id = omp_get_thread_num();
        if(id == 0) // cria uma thread servidor
        {
            Server(id);
        }
        else // cria varias threads clientes
        {
            Client(id);
        }
    }

    return 0;
}

// implementacao do servidor baseado no algoritmo de Manna-Pnueli
void Server(int id)
{
    while(true)
    {
        while(request == 0);
        respond = request;
        while(respond != 0);
        request = 0;
    }
}

// implementacao do cliente baseado no algoritmo de Manna-Pnueli
void Client(int id)
{
    while(true)
    {
        while(respond != id)
        {
            request = id;
        }
        critical_section(id);
        respond = 0;
    }
}

// secao critica
void critical_section(int id)
{
    printf("thread %d: critical section\n", id);
    sleep(1);
    sum += 1;
    printf("thread %d: sum = %d\n", id, sum);
}
