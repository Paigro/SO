#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
	int factor = atoi(argv[1]);
    int elementos = factor * 1024;

	printf("PID: %i\n", getpid());

    printf("Inicializando región de memoria\n");

    int * ptr = (int *) malloc(elementos * sizeof(int));
    for (size_t i = 0; i < elementos; i++)
	{
        ptr[i] = 1;
    }

    	sleep(600);
    	return 0;
}