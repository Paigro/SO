//Ejercicio 4 hoja 5 con PILDORA.
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>


#define BUFFER_SIZE 10
#define NUM_ELEMENTOS_A_PRODUCIR 3

typedef struct _thread_info_t
{
	int num; // ID nuestro.
	pthread_t id; // ID de POSIX.
} thread_info_t;

typedef struct _buffer_t
{
	pthread_mutex_t mutex;

	// Variables de condicion, tantas como predicados haya.
	pthread_cond_t produce; // Si se puede producir o no. (elements < buffer_size).
	pthread_cond_t consume; // Si se puede consumir o no. (elements > 0).

	int data[BUFFER_SIZE];

	int in;
	int out;

	int nElements;
} buffer_t;

typedef struct _shared_int_t
{
	pthread_rwlock_t mutex; // Mutex para proteger el valor.
	int valor;
} shared_int_t;

buffer_t buffer = { PTHREAD_MUTEX_INITIALIZER, // Mutex.
			PTHREAD_COND_INITIALIZER, // Produce.
			PTHREAD_COND_INITIALIZER, // Consume.
			0, // In.
			0, // Out.
			0 }; // nElements.

void* productor(void *_info)
{
	thread_info_t *info = (thread_info_t*) _info; // Casting para poder utilizarlo.

	for(int i = 0; i < NUM_ELEMENTOS_A_PRODUCIR; ++i)
	{
		pthread_mutex_lock(&buffer.mutex); // Entrar a la region critica.
		while(buffer.nElements == BUFFER_SIZE) // Normalmente para esperar se pone la condicion negada.
		{
			pthread_cond_wait(&buffer.produce, &buffer.mutex); // Esperar.
		}
		// Modificacion del buffer.
		int item = 100 * info->num + i;
		buffer.data[buffer.in] = item;
		buffer.in = (buffer.in + 1) % BUFFER_SIZE; // Buffer circular.

		buffer.nElements++;

		printf("Pro[%i] nEl: %i Item: %i in: %i out: %i\n",
			info->num, buffer.nElements, item, buffer.in, buffer.out);

		pthread_cond_signal(&buffer.consume); // Depertar al siguiente en consume.
		pthread_mutex_unlock(&buffer.mutex);  // Desbloquear el mutex.

		sleep(1); // Esperar para que no intente ir otra vez a por el mutex inmediatamente.
	}
	return NULL;
}

void* consumidor(void *_info)
{
	thread_info_t *info = (thread_info_t*) _info; // Casting para poder utilizarlo.

	while(1)
	{
		pthread_mutex_lock(&buffer.mutex); // Entrar a la region critica.
		while(buffer.nElements == 0) // Al reves y suponiendo que no pueden ser < 0.
		{
			pthread_cond_wait(&buffer.consume, &buffer.mutex); // Esperar.
		}
		int item = buffer.data[buffer.out];
		buffer.out = (buffer.out + 1) % BUFFER_SIZE; // Buffer circular.

		buffer.nElements--;

		printf("Con[%i] nEl: %i Item: %i in: %i out: %i\n",
			info->num, buffer.nElements, item, buffer.in, buffer.out);

		pthread_cond_signal(&buffer.produce); // Despertamos al siguiente que produce.
		pthread_mutex_unlock(&buffer.mutex); // Desbloqueamos el mutex.

		if(item == -1) // Pildora envenada para acabar la consumicion. Valor a elegir.
		{
			break;
		}

		sleep(2); // Esperar para que no intente ir otra vez a por el mutex inmediatamente.
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	int productores = atoi(argv[1]);
	int consumidores = atoi(argv[2]);

	thread_info_t *pool = (thread_info_t*)  malloc((productores + consumidores) * sizeof(thread_info_t)); 

	// Creacion de producotres.	
	for(int i = 0; i < productores; ++i)
	{
		pool[i].num = i;
		pthread_create(&pool[i].id, NULL, productor, (void*) &pool[i]);
	}

	// Creacion de consumidores.
	for(int i = productores; i < (productores + consumidores); ++i)
	{
		pool[i].num = i - productores;
		pthread_create(&pool[i].id, NULL, consumidor, (void*) &pool[i]);
	}

	// Escribir pildoras. Una por consumidor. Lo hace el thread principal.
	// Consumidores.
	for(int i = 0; i < consumidores; ++i)
	{
        pthread_mutex_lock(&buffer.mutex); // Entrar a la region critica.
		while(buffer.nElements == BUFFER_SIZE) // Normalmente para esperar se pone la condicion negada.
		{
			pthread_cond_wait(&buffer.produce, &buffer.mutex); // Esperar.
		}
        printf("hola");
		buffer.data[buffer.in] = -1; // Meter pildora.
		buffer.in = (buffer.in + 1) % BUFFER_SIZE; // Buffer circular.

		buffer.nElements++;

		pthread_cond_signal(&buffer.consume); // Depertar al siguiente en consume.
		pthread_mutex_unlock(&buffer.mutex); // Desbloquear el mutex. ???
	}

	// Sincronizar los threads.
	for(int i = 0; i < (productores + consumidores); ++i)
	{

		pthread_join(pool[i].id, NULL); // NULL porque no esperamos nada de retorno.
		printf("Thread %i termino\n", pool[i].num);
	}

	free(pool); // Liberar memoria.
	return 0;
}