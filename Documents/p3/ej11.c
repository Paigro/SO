// Hoja 3 ejercicio 11. IMPORTANTE.
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <errno.h>

#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <linux/limits.h>

#define N_FILES 5
#define N_NUMBERS 10


//
void writeInOut(int n_son)
{
    int fd_out = open("./output11.txt", O_WRONLY); // Abrimos el archivo.

    if(fd_out == -1) // Error al abrir el archivo.
    {
        perror("Error al ir a escribir desde hijo.");
        exit(EXIT_FAILURE);
    }

    char buffer[N_NUMBERS + 1]; // Son +1 numero mas el '\0'.
    for(int i = 0; i < N_NUMBERS; i++)
    {
        buffer[i] = '0' + n_son; // Rellenamos el buffer con 0 + el numero de hijo que toque.
    }
    buffer[N_NUMBERS] = '\0'; // Ponemos el '\0' final.

    off_t writeOffset = N_NUMBERS * n_son; // Offset para escritura.
    lseek(fd_out, writeOffset, SEEK_SET); // Cambiamos donde escribe (le metemos el offset).
    write(fd_out, buffer, N_NUMBERS); // Escribimos en el archivo. No hace falta escrbir el '\0'.
    close(fd_out);
}

int main(int argc, char* argv[])
{
    // Creamos el fichero de salida y guardamos su descriptor.
    int fd_out = open("./output11.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    write(fd_out, "00000", 5);
    close(fd_out);

    // Creamos los N_FILES hijos necesarios.
    for(int i = 0; i < N_FILES; i++)
    {
        pid_t pid = fork();

        switch(pid)
        {
            case -1: // Caso error.
                perror("Error en el fork().");
                exit(EXIT_FAILURE);
                break;
            case 0: // Caso hijo.
                printf("[Hijo %i] PID: %i\n", i, pid);
                writeInOut(i + 1); // Lamamos a la funcion, como i empienza en 0 lo llamamos con +1.
                exit(i);
                break;
            default: // Caso padre.
                printf("[Padre] PID: %i\n", getpid());
                break;
        }
    }

    int status = 0;
    // Esperamos a los hijos.
    for(int i = 0; i < N_FILES; i++)
    {
        pid_t pid = wait(&status);
        printf("Hijo %i con PID %i termino.\n", WEXITSTATUS(status), pid);
    }

	return EXIT_SUCCESS;
}