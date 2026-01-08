// Hoja 2 ejercicio 8. IMPORTANTE.
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <errno.h>

#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

#define TBUFF 8192


// Suponemos que siempre se ejecuta con 5 argumentos.
int main(int argc, char * argv [])
{
    char* input_file = argv[1];
    char* output_file = argv[2];
    int block_size = atoi(argv[3]);
    int block_count = atoi(argv[4]);
    int seek = atoi(argv[5]);

    // Si el size que nos dan es mayor al definido usamos el definido.
    if(block_size > TBUFF) block_size = TBUFF;

    char buffer[TBUFF]; // Buffer.
    // Descriptores de ficheros in y out.
    int fd_in;
    int fd_out;

    // Abrir fichero de entrada.
    if(strcmp(input_file, "-") == 0)
    {
        fd_in = STDIN_FILENO;
    }
    else
    {
        fd_in = open(input_file, O_RDONLY);
        if(fd_in < 0)
        {
            perror("Error en la entrada.");
            return EXIT_FAILURE;
        }
    }

    // Abrir fichero de salida.
    if(strcmp(output_file, "-") == 0)
    {
        fd_out = STDOUT_FILENO;
    }
    else
    {
        fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0664);
        if(fd_out < 0)
        {
            perror("Error en la salida.");
            return EXIT_FAILURE;
        }
    }

    // Saltar el seek.
    if(seek > 0)
    {
        lseek(fd_out, (off_t)seek * block_size, SEEK_SET);
    }

    int total_bytes = 0; // Bytes leidos.
    while(total_bytes < (block_count * block_size))
    {
        int actual_bytes = 0; // Bytes leidos en un ciclo.
        
        // Leemos bloque a bloque.
        while(actual_bytes < block_size)
        {
            int n_read = 0; // Numero de bytes leidos.
            n_read = read(fd_in, // Desde donde.
                            buffer + (total_bytes + actual_bytes), // Donde.
                            block_size - actual_bytes); // Cuanto.
            
            // Si no leemos ningun byte adios.
            if(n_read == 0)
            {
                break;
            }
            printf("Total bytes %i | Bytes leidos %i\n",total_bytes, n_read);
            actual_bytes += n_read;
        }

        // Si no se ha leido nada adios.
        if(actual_bytes == 0)
        {
            break;
        }
        else
        {
            total_bytes += actual_bytes;
        }

        // Escritura de un bloque.
        int write_bytes = 0;
        while(write_bytes < actual_bytes)
        {
            int n_write = 0; // Numero de bytes escritos.
            n_write = write(fd_out, // Donde.
                            buffer + write_bytes, // Desde donde.
                            actual_bytes - write_bytes); // Cuanto.
            
            // Si no se ha escrito nada adios.
            if(n_write == 0)
            {
                break;
            }

            write_bytes += n_write;
        }
    }

    close(fd_in);
    close(fd_out);

    return EXIT_SUCCESS;
}