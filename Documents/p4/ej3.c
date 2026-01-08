// Hoja 4 ejercicio 3. IMPORTANTE.
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <errno.h>

#include <sys/mman.h>
#include <string.h>

#define SIZE 1024


int main(int argc, char* argv)
{
    void* ptr = mmap(NULL, // Origen.
                        SIZE, // Tam.
                        PROT_READ | PROT_WRITE, // Permisos de lectura y escritura.
                        MAP_PRIVATE | MAP_ANON, // Flags de memoria privada y anonima.
                        -1, // Archivo.
                        0); // Offset.

    // Control de errores.
    if(ptr == MAP_FAILED)
    {
        perror("mmap() ha ido mal.");
        return EXIT_FAILURE;
    }

    memset(ptr, 0, SIZE);
    printf("PID: %i, VMA: %p\n", getpid(), ptr);

    sleep(600);

    return EXIT_SUCCESS;
}