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
        printf("[Hijo] dormido.\n");
        printf("[Hijo] pid: %i, ppid: %i, pgid: %i, sid: %i, seg: %i\n",
            getpid(), getppid(), getpgid(pid), getsid(pid), seg_son);
        sleep(seg_son);
        printf("[Hijo] derpierto.");
        break;
    default: // Caso padre.
        printf("[Padre] dormido.\n");
        printf("[Padre] pid: %i, ppid: %i, pgid: %i, sid: %i, seg: %i\n",
            getpid(), getppid(), getpgid(pid), getsid(pid), seg_father);
        printf("[Padre] derpierto.\n");
        break;
    }

    return EXIT_SUCCESS;
}