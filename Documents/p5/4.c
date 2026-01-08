// Hoja 5 ejercicio 4. IMPORTANTE.
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <errno.h>

#include <pthread.h>

#define BUFFER_SIZE 10
#define P_ELEMENTS 5


typedef struct _thread_info_t
{
    int nThread; // ID para identificar al thread.
    pthread_t id; // ID de POSIX de los threads.
} thread_info_t;

typedef struct _buffer_t{
    pthread_mutex_t mutex; // Mutex para proteger el buffer.

    pthread_cond_t  produce; // Condicion para productores.
    pthread_cond_t  consume; // Condicion para consumidores.

    int in; // Indice de escritura.
    int out;// Indice de lectura.
    
    int elements; // Numero de elementos en el buffer.

    char data[BUFFER_SIZE]; // buffer de datos circular.
} buffer_t;

// Variable global para la suma paralela.
buffer_t buffer =   {PTHREAD_MUTEX_INITIALIZER, // Mutex para proteger el buffer.
                    PTHREAD_COND_INITIALIZER,   // Condicion para productores.
                    PTHREAD_COND_INITIALIZER,   // Condicion para consumidores.
                    0,                          // Indice de escritura.
                    0,                          // Indice de lectura. 
                    0};   

void* producer(void* _info)
{
    thread_info_t* info = (thread_info_t*) _info;

    for (int i = 0; i < P_ELEMENTS; i++) {
        // ENTRADA A REGION CRITICA.
        pthread_mutex_lock(&buffer.mutex); // Bloqueamos el mutex del buffer.
        
        // Esperamos mientras el buffer este lleno.
        while(buffer.elements >= BUFFER_SIZE) {
            pthread_cond_wait(&buffer.produce,  // Espera en la condicion de produccion.
                                &buffer.mutex); // Desbloquea el mutex mientras espera.
        }

        int item = 100 * info->nThread+i;

        // MODIFICACION DEL BUFFER.
        buffer.data[buffer.in] = item; // Producimos un dato.
        buffer.in = (buffer.in + 1) % BUFFER_SIZE; // Actualizamos el indice de escritura.
        buffer.elements++; // Incrementamos el numero de elementos en el buffer.

        printf("P[%i] E:%i I:%i in:%i out:%i\n", 
                info->nThread, buffer.elements, item, 
                buffer.in, buffer.out);

        // SALIDA DE REGION CRITICA.
        pthread_cond_signal(&buffer.consume); // Señalizamos a los consumidores que hay datos.
        pthread_mutex_unlock(&buffer.mutex); // Desbloqueamos el mutex del buffer.

        sleep(1); // Simulamos tiempo de produccion.
    }
    
    
    
    return NULL;
}

void* consumer(void* _info)
{
    thread_info_t* info = (thread_info_t*) _info; // casteamos el puntero void a thread_info_t.

    while (1) {
        // ENTRADA A REGION CRITICA.
        pthread_mutex_lock(&buffer.mutex); // Bloqueamos el mutex del buffer.
        
        // Esperamos mientras el buffer esté lleno.
        while(buffer.elements <= 0) {
            pthread_cond_wait(&buffer.consume,  // Espera en la condicion de consumir.
                                &buffer.mutex); // Desbloquea el mutex mientras espera.
        }

        // MODIFICACION DEL BUFFER.
        int item = buffer.data[buffer.out]; // Consumimos un dato.
        buffer.out = (buffer.out + 1) % BUFFER_SIZE; // Actualizamos el indice de lectura.
        buffer.elements--; // Reducimos el numero de elementos en el buffer.

        printf("C[%i] E:%i I:%i in:%i out:%i\n", 
                info->nThread, buffer.elements, item, 
                buffer.in, buffer.out);

        // SALIDA DE REGION CRITICA.
        pthread_cond_signal(&buffer.produce); // Señalizamos a los productores que se ha consumido un dato.
        pthread_mutex_unlock(&buffer.mutex); // Desbloqueamos el mutex del buffer.

        sleep(1); // Simulamos tiempo de produccion.
    }

    return NULL;
}

int main(int argc, char* argv[])
{
    int nProduces = atoi(argv[1]); // Cogemos de la entrada el numero de productores.
    int nConsumers = atoi(argv[2]); // Cogemos de la entrada el numero de consumidores.

    // Array y reserva de memoria para los threads.
    thread_info_t* threadPool = (thread_info_t*) malloc((nProduces + nConsumers) * sizeof(thread_info_t));

    // Creacion de productores.
    for(int i = 0; i < nProduces; i++)
    {
        threadPool[i].nThread = i;

        pthread_create(&threadPool[i].id, // ID del thread en POSIX.
                        NULL, // Atributos.
                        producer, // Funcion de inicio de thread.
                        (void*) &threadPool[i]); // Argumentos para la funcion de thread.
    }

    // Creacion de consumidores.
    for(int i = nConsumers; i < (nProduces + nConsumers); i++)
    {
        threadPool[i].nThread = i;

        pthread_create(&threadPool[i].id, // ID del thread en POSIX.
                        NULL, // Atributos.
                        consumer, // Funcion de inicio de thread.
                        (void*) &threadPool[i]); // Argumentos para la funcion de thread.
    }

    // Sincronizacion de threads.
    for(int i = 0; i < (nProduces + nConsumers); i++)
    {
        pthread_join(threadPool[i].id, NULL);
        printf("[Thread %i] Terminado.\n", threadPool[i].nThread);
    }

    return EXIT_SUCCESS;
}