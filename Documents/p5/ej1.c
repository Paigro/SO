#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>


typedef struct _thread_info_t
{
	int num; // ID nuestro.
	pthread_t id; // ID de POSIX.
} thread_info_t;

void* haz_thread(void *_info)
{
	thread_info_t *info = (thread_info_t*) _info; // Casting para poder utilizarlo.
    printf("Thread %i, sleeping...\n", info->num);
	sleep(info->num * 1000);
	return NULL; 
}

int main(int argc, char *argv[])
{
	int nThreads = atoi(argv[1]); // Cogemos el numero de threads que queremos de los atributos de entrada.

	thread_info_t *pool = (thread_info_t*) malloc (nThreads * sizeof(thread_info_t)); // Para definir un array dinamico.
	
	// Crear los threads y guardar su informacion.
	for(int i = 0; i < nThreads; ++i)
	{
		pool[i].num = i;

		pthread_create(&pool[i].id, 0, haz_thread, (void*) &pool[i]);
		//pthread_create(&pool[i].id, NULL, haz_thread, (void*) &pool[i]); // Igual que lo anterior.
	}
	
	// Sincronizar los threads.
	for(int i = 0; i < nThreads; ++i)
	{
		pthread_join(pool[i].id, NULL); // NULL porque no esperamos nada de retorno.
		printf("Thread %i termino\n", pool[i].num);

	}

	free(pool); // Liberar memoria.

	return 0;
}