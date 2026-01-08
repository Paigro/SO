// Hoja 2 ejercicio 9. IMPORTANTE.
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <errno.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include <linux/limits.h>


int main(int argc, char * argv [])
{
    char* path = argv[1];
    struct stat statInfo; // Estructura que tiene la info de stat.

    // Coger la info del stat.
    if(stat(path, &statInfo) == -1)
    {
        perror("Eror stat.");
        return EXIT_FAILURE;
    }

    // Ver que es un directorio.
    if(!S_ISDIR(statInfo.st_mode))
    {
        perror("Error directorio.");
        return EXIT_FAILURE;
    }

    // Comprobar si tenemos acceso.
    if(access(path, R_OK | X_OK) == -1)
    {
        perror("Error acceso.");
        return EXIT_FAILURE;
    }

    DIR* dir = opendir(path); // Directorio.
    struct dirent* dent; // Estrctura con la info del directorio.

    // Leemos las entradas del directorio.
    while((dent = readdir(dir)) != NULL)
    {
        switch (dent->d_type)
        {
        case DT_REG:
            printf("%s*\n", dent->d_name);
	        break;
        case DT_DIR:
            printf("%s/\n", dent->d_name);
	        break;
        case DT_LNK:
            char ori[PATH_MAX];
            snprintf(ori, PATH_MAX, "%s/%s", path, dent->d_name); // Guarda en ori, con tam PATH_MAX, el path con / y el nombre del archivo.

            char dest[PATH_MAX];
            if (readlink(ori, dest, PATH_MAX) == -1) // Readlink usa un path, donde guardar y el tam.
            {
	            perror("readlink");
	            continue;
            }
            printf("%s -> %s\n", ori, dest);
	        break;
        }
    }

    closedir(dir);
    return EXIT_FAILURE;
}