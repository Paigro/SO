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
    char full_path[PATH_MAX]; 

    // Leemos las entradas del directorio.
    while((dent = readdir(dir)) != NULL)
    {
        snprintf(full_path, PATH_MAX, "%s/%s", path, dent->d_name); // Guarda en full_path, con tam PATH_MAX, el path con / y el nombre del archivo.

        switch (dent->d_type)
        {
        case DT_REG:
            struct stat fileStat; // Esctructura que tiene el info del stat del archivo.
            
            if(stat(full_path, &fileStat) == 0)
            {
                if(fileStat.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
                {
                    printf("%s*\n", dent->d_name); // Con permisos de ejecucion.
                }
                else
                {
                    printf("%s\n", dent->d_name); // Si no tiene permisos de ejecucion.
                }
            }
            else
            {
                printf("%s\n", dent->d_name); // Si falla el stat al menos escribimos el nombre.
            }
	        break;
        case DT_DIR:
            printf("%s/\n", dent->d_name);
	        break;
        case DT_LNK:
            char dest[PATH_MAX];
            if (readlink(full_path, dest, PATH_MAX) == -1) // Readlink usa un path, donde guardar el destino y el tam.
            {
	            perror("readlink");
	            continue;
            }
            printf("%s -> %s\n", full_path, dest);
	        break;
        }
    }

    closedir(dir);
    return EXIT_SUCCESS;
}