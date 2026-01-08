#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFFER_SIZE 10
#define P_ELEMENTS 5

// Definicion de la estructura para pasar argumentos a los threads.
typedef struct _thread_info_t{ // typedef sirve para definir un nuevo tipo de dato.
    int         num_thread; // Numero de thread.
    pthread_t   id;         // Identificador del thread a nivel de POSIX.
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
                    0};                         // Numero de elementos en el buffer.


void* productor(void* _info)
{
    thread_info_t* info = (thread_info_t*) _info; // casteamos el puntero void a thread_info_t.

    for (int i = 0; i < P_ELEMENTS; i++) {
        // ENTRADA A REGION CRITICA.
        pthread_mutex_lock(&buffer.mutex); // Bloqueamos el mutex del buffer.
        
        // Esperamos mientras el buffer este lleno.
        while(buffer.elements >= BUFFER_SIZE) {
            pthread_cond_wait(&buffer.produce,  // Espera en la condicion de produccion.
                                &buffer.mutex); // Desbloquea el mutex mientras espera.
        }

        int item = 100*info->num_thread+i;

        // MODIFICACION DEL BUFFER.
        buffer.data[buffer.in] = item; // Producimos un dato.
        buffer.in = (buffer.in + 1) % BUFFER_SIZE; // Actualizamos el indice de escritura.
        buffer.elements++; // Incrementamos el numero de elementos en el buffer.

        printf("P[%i] E:%i I:%i in:%i out:%i\n", 
                info->num_thread, buffer.elements, item, 
                buffer.in, buffer.out);

        // SALIDA DE REGION CRITICA.
        pthread_cond_signal(&buffer.consume); // Señalizamos a los consumidores que hay datos.
        pthread_mutex_unlock(&buffer.mutex); // Desbloqueamos el mutex del buffer.

        sleep(1); // Simulamos tiempo de produccion.
    }
    return NULL;
}

void* consumidor(void* _info)
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
                info->num_thread, buffer.elements, item, 
                buffer.in, buffer.out);

        // SALIDA DE REGION CRITICA.
        pthread_cond_signal(&buffer.produce); // Señalizamos a los productores que se ha consumido un dato.
        pthread_mutex_unlock(&buffer.mutex); // Desbloqueamos el mutex del buffer.

        sleep(1); // Simulamos tiempo de produccion.
    }
}

int main(int argc, char *argv[]) {
    // Primer argumento es el numero de productores.
    int p = atoi(argv[1]);
    // Segundo argumento es el tamaño de consumidores.
    int c = atoi(argv[2]);

    // reservamos memoria para los threads "malloc(nt * sizeof(thread_info_t))".
    // Creamos un array de estructuras thread_info_t.
    thread_info_t * pool = (thread_info_t*) 
        malloc((p + c) * sizeof(thread_info_t));

    // Creamos los threads escritores.
    for (int i = 0; i < p; i++) {
        // Inicializamos la estructura de cada thread.
        pool[i].num_thread = i;

        pthread_create(&pool[i].id,         // Identificador del thread.
                        NULL,               // Atributos por defecto. 0 == NULL.
                        productor,          // Funcion de inicio del thread.
                        (void *) &pool[i]); // Argumentos para la funcion del thread.
    }

    // Creamos los threads lectores.
    for (int i = p; i < p + c; i++) {
        // Inicializamos la estructura de cada thread.
        pool[i].num_thread = i - p;

        pthread_create(&pool[i].id,         // Identificador del thread.
                        NULL,               // Atributos por defecto. 0 == NULL.
                        consumidor,           // Funcion de inicio del thread.
                        (void *) &pool[i]); // Argumentos para la funcion del thread.
    }

    for (int i = 0; i < (p + c); i++) {
        // Sincronizamos los threads.
        pthread_join(pool[i].id,
                    NULL);
        printf("Thread %i termino\n", pool[i].num_thread);
    }

    free(pool); // Liberamos la memoria reservada.

    return 0;
}