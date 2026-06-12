// Hoja 2 ejercicio 4. IMPORTANTE.
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <errno.h>

#include <string.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>


int main(int argc, char * argv[])
{
    char * path = argv[1]; // Ruta pasada.
    struct stat statInfo; // Estructura que tiene la info de stat.

    if(lstat(path, &statInfo) == -1)
    {
        char *s;
        perror(s); // Cogemos el error.
        printf("Error en stat: %s\n", s);
        return EXIT_FAILURE;
    }

    // Inodo de fichero.
    printf("Inodo: %ld\n", statInfo.st_ino);
    // Numero de dispositivo. major y minor en man 2 lstat.
    printf("Numero de dispositivo: [%d, %d]\n", major(statInfo.st_dev), minor(statInfo.st_dev));

    // Tipo de fichero.
    printf("Tipo: ");
    if(S_ISREG(statInfo.st_mode))
    {
        printf("fichero regular.\n");
    }
    else if(S_ISDIR(statInfo.st_mode))
    {
        printf("directorio.\n");
    }
    else if(S_ISLNK(statInfo.st_mode))
    {
        printf("enlace simbolico.\n");
    }
    else
    {
        printf("otro.\n");
    }

    // Tiempos:
    printf("Ultimo cambio de estado: %s", ctime(&statInfo.st_ctime));
    printf("Ultimo acceso: %s", ctime(&statInfo.st_atime));
    

    return EXIT_SUCCESS;
}