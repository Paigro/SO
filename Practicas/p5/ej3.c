// Hoja 5 ejercicio 3. IMPORTANTE.
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <errno.h>

#include <pthread.h>


// Tipo para guardar argumentos de los threads.
typedef struct _thread_info_t
{
    int nThread; // ID para identificar al thread.
    int blockSize; // Tam de bloque para sumar.

    pthread_t id; // ID de POSIX de los threads.
} thread_info_t;

// Estructura para guardar la acumulacion de suma y el mutex que lo controla. Zona critica.
typedef struct _suma_total_t
{
    pthread_mutex_t mutex; // Mutex para porteger la zona critica.
    int sumaAcc; // Suma acumulada entre todos los threads.
} suma_total_t;

// Variable global para simplificar las cosas. Inicializamos el mutex y empezamos en 0.
suma_total_t sum = { PTHREAD_MUTEX_INITIALIZER, 0 };

// Lo que tiene que hacer el thread.
void* execThread(void* info)
{   
    thread_info_t* _info = (thread_info_t*) info;

    int auxSuma = 0;
    int ini = _info->blockSize * _info->nThread; // Numero inicial.
    int fin = _info->blockSize * (_info->nThread + 1); // Numero final.

    // Sumamos.
    for(int i = ini; i < fin; i++)
    {
        auxSuma += i;
    }

    // Bloqueamos el mutex de la region critica.
    pthread_mutex_lock(&sum.mutex);

    // Metemos lo sumado.
    sum.sumaAcc = sum.sumaAcc + auxSuma;

    // Desbloqueamos el mutex al acabar de hacer cosas en la region critica.
    pthread_mutex_unlock(&sum.mutex);

    return NULL;
}

// Main.
int main(int argc, char* argv[])
{
    int nThreads = atoi(argv[1]); // Cogemos de la entrada el numero de threads.
    int blockSize = atoi(argv[2]); // Cogemos de la entrada el tam de bloque.

    // Array y reserva de memoria para los threads.
    thread_info_t* threadPool = (thread_info_t*) malloc(nThreads * sizeof(thread_info_t));

    // Creacion de threads.
    for(int i = 0; i < nThreads; i++)
    {
        threadPool[i].nThread = i; // Guardamos su ID propio.
        threadPool[i].blockSize = blockSize;

        pthread_create(&threadPool[i].id, // ID del thread en POSIX.
                        NULL, // Atributos.
                        execThread, // Funcion de inicio de thread.
                        (void*) &threadPool[i]); // Argumentos para la funcion de thread.
    }

    // Sincronizacion de threads al final.
    for(int i = 0; i < nThreads; i++)
    {
        pthread_join(threadPool[i].id, // ID de POSIX.
                        NULL);
        printf("[Thread %i] Termino.\n", threadPool[i].nThread);
    }

    // La suma de los n primeros numeros naturales es n*(n+1)/2 (suma de Gauss)
    int aux = (nThreads * blockSize) - 1;
    printf("Suma total: %i / %i\n", sum.sumaAcc, (aux *(aux + 1)/2));

    return EXIT_SUCCESS;
}