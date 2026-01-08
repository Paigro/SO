// Hoja 3 ejercicio 9. IMPORTANTE.
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <errno.h>

#include <sys/wait.h>
#include <sys/types.h>

#include <linux/limits.h>


int main(int argc, char* argv[])
{
	pid_t pid;
	int rc;
	pid = fork();

	switch (pid)
    {
    	case -1: // Es error.
        	perror("fork");
        	exit(EXIT_FAILURE);
    	case 0: // Es hijo.
		    printf("Hijo: %i (padre: %i)\n", getpid(), getppid());

			// Queremos el modo vector porque ya lo tenemos en modo vector y p para que nos busque en el path.
			rc = execvp(argv[1], &argv[1]);
			if(rc == -1)
			{
				perror("execvp()");
			}			
			
            printf("No sale nada"); // Si todo va bien no deberia de escribirse este printf.
		    
            return(1);
		default: // Es padre.
        	printf("Padre: %i (hijo: %i)\n", getpid(), pid);

			int status;

			rc = waitpid(pid, &status, 0);
			if(rc == -1)
			{
				perror("waitpid()");
			}

        	if(WIFEXITED(status))
			{
				printf("Hijo termino con codigo: %i\n", WEXITSTATUS(status));
			}
			else if(WIFSIGNALED(status))
			{
				printf("Hijo termino con señal: %i\n", WTERMSIG(status));
			}
    }

	return EXIT_SUCCESS;
}