// Hoja 3 ejercicio 5. IMPORTANTE.
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <errno.h>

#include <sys/wait.h>
#include <sys/types.h>

#include <linux/limits.h>


int main(int argc, char* argv[])
{
    int seg_father = atoi(argv[1]);
    int seg_son = atoi(argv[2]);

    pid_t pid = fork();

    switch(pid)
    {
    case -1: // Caso error.
        perror("Error en el fork.\n");
        exit(EXIT_FAILURE);
        break;
    case 0: // Caso hijo.

        // Los hijos crean su propia sesion.
        if(setsid() == -1)
        {
            perror("[Hijo] Nueva sesion no creada correctamente.");
        }

        printf("[Hijo] dormido.\n");
        printf("[Hijo] pid: %i, ppid: %i, pgid: %i, sid: %i, seg: %i\n",
            getpid(), getppid(), getpgid(0), getsid(0), seg_son);
        sleep(seg_son);
        printf("[Hijo] derpierto.");
        exit(EXIT_SUCCESS);
        break;
    default: // Caso padre.
        printf("[Padre] dormido.\n");
        printf("[Padre] pid: %i, ppid: %i, pgid: %i, sid: %i, seg: %i\n",
            getpid(), getppid(), getpgid(0), getsid(0), seg_father);
        sleep(seg_father);
        printf("[Padre] derpierto.\n");

        int status;
        wait(&status); // Para esperar al hijo (opcional).
        
        break;
    }

    return EXIT_SUCCESS;
}

/* PREGUNTAS:
-1:
    Si el hijo acaba antes que el padre, el hijo se convierte en zombie hasta que el padre haga wait().
    Si el padre acaba antes que el hijo, el hijo es adoptado por el init.
-2:
    Al hacer Ctrl+C el padre muerte pero el hijo no porque al crease la nueva sesion, las signals 
    de la consola actual no le llegan. Se seguira ejecutando hasta que acabe su cuenta.
*/