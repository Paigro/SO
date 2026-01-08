// Hoja 3 ejercicio 11 -> Hoja 4 ejercicio 4. IMPORTANTE.
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <errno.h>

#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>

#include <linux/limits.h>

#define N_PROC 5
#define SEG_SIZE (5 + N_PROC * 5)


// Recibe la region de memoria y el proceso.
void write_char(void * _ptr, int proc)
{
	char* ptr = (char*) _ptr;
    int offset = 5 + (proc - 1) * 5;

	for(int i = 0; i < 5; ++i)
	{
		ptr[offset + i] = '0' + proc; // Esto te da el caracter ASCII. "Truquito".
	}
}

int main(int argc, char *argv[])
{
	// Preparacion del segmento de memoria. Para ello:
	// (1) Mapa compartido.
	// (2) Abrir fichero para poder mappearlo.
	// (3) Tam del fichero adecuado. 5 procesos * 5 escrituras = 25, demosle 30 porque si. (+5 porque el padre
	// escribe tambien).
	// (4) Llamar a mmap del fichero.


	// (1)(2)
	int fd_out = open("salida.txt",
			O_RDWR | O_TRUNC | O_CREAT, // Flags de permisos, trucar a 0 el tam y creacion si no existe.
			0660); // Permisos de lectura y escritura para el usuario y grupo.
	// Comprobacion.
    if (fd_out == -1) {
        perror("Error en open().");
        exit(EXIT_FAILURE);
    }

	// (3) + comprobacion.
	if (ftruncate(fd_out, SEG_SIZE) == -1)
    {
        perror("Error en ftruncate().");
        exit(EXIT_FAILURE);
    }

	// (4)
	char* ptr = mmap (NULL, // Origen.
			30, // Tam. Podria no ponerse el mismo tam del fichero.
			PROT_READ | PROT_WRITE, // Permisos de lectura y escritura.
			MAP_SHARED, // Flags de que es memoria compartida.
			fd_out, // Archivo.
			0); // Offset.	

	// Comprobacion de que se ha mappeado bien la memoria.
	if(ptr == MAP_FAILED) // Importante comparar con la cte y no con -1 que es lo que devuelve.
	{
		perror("mmap()");
		return 1;
	}

    close(fd_out);

	// Todo esto se tiene que hacer antes que los forks porque la memoria es compartida y hay que usar el ptr comun.

	for (int i = 0; i < 5; i++)
    {
        ptr[i] = '0';
    }   
	// Creacion de hijos.
	for(int i = 1; i < N_PROC + 1; ++i) // Empezar en 1 para pasarle el id 1 al hijo 1.
	{
		pid_t pid = fork(); // Todo se copia y se crea con copy_on_write.

		if(pid != 0) // Es el padre.
		{
			continue; // El padre no tiene que hacer nada mas que crear hijos.	
		}
		
		write_char(ptr, i);
		exit(i); // Los hijos no tienen que hacer nada mas que escribir asi que salir con exit.
	}

	// Esperar a los 5 hijos.
	for(int i = 0; i < 5; ++i)
	{
		int status;
		pid_t pid;
		pid = wait(&status);

		printf("HIJO: %d\n", status);
	}

	// Sincronizacion:
	msync(ptr, SEG_SIZE, MS_SYNC);

	// Liberar memoria.
	munmap(ptr, SEG_SIZE    );

	return EXIT_SUCCESS;
}