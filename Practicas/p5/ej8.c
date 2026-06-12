//Ejercicio 8 hoja 5.
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>


#define BUFFER_SIZE 5

typedef struct _thread_info_t
{
	int num; // ID nuestro.
	pthread_t id; // ID de POSIX.
} thread_info_t;

typedef struct _buffer_t
{
	pthread_mutex_t mutex; // Mutex.

	// Variables de condicion, tantas como predicados haya.
	pthread_cond_t produce; // Si se puede producir o no. (elements < buffer_size).
	pthread_cond_t consume; // Si se puede consumir o no. (elements > 0).

	char data[BUFFER_SIZE][PATH_MAX]; // Este buffer es como el anterior pero con punteros a paths. PATH_MAX lo da el 						// sistema.

	int in;
	int out;

	int nElements;
} buffer_t;

typedef struct _suma_coop_t
{
	pthread_mutex_t mutex;
	unsigned long long total; // A lo loco para que no se desborde la variable.
} suma_coop_t;


suma_coop_t suma = { PTHREAD_MUTEX_INITIALIZER, // Mutex.
			0 }; // Total.
buffer_t buffer = { PTHREAD_MUTEX_INITIALIZER, // Mutex.
			PTHREAD_COND_INITIALIZER, // Produce.
			PTHREAD_COND_INITIALIZER, // Consume.
			0, // In.
			0, // Out.
			0 }; // nElements.


void * escribeBuffer(char t, int id, const char* item)
{
	pthread_mutex_lock(&buffer.mutex); // Entrar a la region critica.
	while(buffer.nElements == BUFFER_SIZE) // Normalmente para esperar se pone la condicion negada.
	{
		pthread_cond_wait(&buffer.produce, &buffer.mutex); // Esperar.		
	}

	// Modificacion del buffer.
	strcpy(buffer.data[buffer.in], item);
	buffer.in = (buffer.in + 1) % BUFFER_SIZE; // Buffer circular.

	buffer.nElements++;

	printf("%c[%i] E: %i I: %s in: %i out: %i\n",
		t, id, buffer.nElements, item, buffer.in, buffer.out);

	pthread_cond_signal(&buffer.consume); // Depertar al siguiente en consume.
	pthread_mutex_unlock(&buffer.mutex);  // Desbloquear el mutex.

	sleep(2); // Esperar para que no intente ir otra vez a por el mutex inmediatamente.
	
	return NULL;
}

// Para "/home/user/.bashrc" => path = "/home/user/" y dent => ".bashrc".
void* productor(void *_path)
{ 
	char * path = (char*) _path;// Casting para poder utilizarlo de void a char.

	char item[PATH_MAX];

	DIR * dir = opendir(path);

	struct dirent * dent;
	while((dent = readdir(dir)) != NULL) // Lee las entradas, la guarda y sigue en el bucle hasta que sea NULL.
	{
		sprintf(item, "%s/%s",
				path, dent->d_name);

		escribeBuffer('P', 0, item); // Por enunciado solo hay un porductor.
	}
	return NULL;
}

void* consumidor(void *_info)
{
	thread_info_t *info = (thread_info_t*) _info; // Casting para poder utilizarlo.

	char item[PATH_MAX];
	struct stat statbuf;

	while(1)
	{
		pthread_mutex_lock(&buffer.mutex); // Entrar a la region critica.
		while(buffer.nElements == 0) // Al reves y suponiendo que no pueden ser < 0.
		{
			pthread_cond_wait(&buffer.consume, &buffer.mutex); // Esperar.
		}

		strcpy(item, buffer.data[buffer.out]); // Nos guardamos el elemento que toca en item. Hay que copiarla.
		buffer.out = (buffer.out + 1) % BUFFER_SIZE; // Buffer circular.

		buffer.nElements--;

		pthread_cond_signal(&buffer.produce); // Despertamos al siguiente que produce.
		pthread_mutex_unlock(&buffer.mutex); // Desbloqueamos el mutex.

		if(item == '/0') // Pildora envenada para acabar la consumicion. Valor a elegir. Cadena vacia.
		{
			break;
		}
		
		stat(item, &statbuf); 
		
		if(S_ISREG(statbuf.st_mode)) // Es true si es fichero regular.
		{
			printf("C[%i]: %s - %i\n",
					info->num, item, statbuf.st_size);
			pthread_mutex_lock(&suma.mutex); // Bloquear el mutex.
			suma.total += statbuf.st_size; // Sumamos el tamaño del fichero.
			pthread_mutex_unlock(&suma.mutex); // Debloquear el mutex.
		}
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	int consumidores = atoi(argv[2]);

	thread_info_t *pool = (thread_info_t*)  malloc(consumidores * sizeof(thread_info_t)); 
	
	// Creacion de el productor.
	pthread_t productor_id;
	pthread_create(&productor_id, NULL, productor, (void*) argv[1]);

	// Creacion de consumidores.
	for(int i = 0; i < consumidores; ++i)
	{
		pool[i].num = i + 1;
		pthread_create(&(pool[i].id), NULL, consumidor, (void*) &pool[i]);
	}

	pthread_join(productor_id, NULL);
	printf("Productor acabado");

	// Escribir pildoras.
	for(int i = 0; i < consumidores; ++i)
	{	
		escribe_buffer('M', 0, "");
	}

	// Sincronizar los threads consumidores.
	for(int i = 0; i < consumidores; ++i)
	{
		pthread_join(pool[i].id, NULL); // NULL porque no esperamos nada de retorno.
		printf("Thread %i termino\nj", pool[i].num);
	}

	printf("Tamaño total: %lli\n", suma.total);

	free(pool); // Liberar memoria.
	return 0;
}