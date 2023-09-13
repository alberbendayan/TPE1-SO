#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include "shared_memory.h"

// #define SLAVES 2
#define BUFFERSIZE 512
#define INITIALARGS 2

#define SHAREDMEMORY "myshm"

#define SHM_SIZE 1024            // Tamaño de la memoria compartida
#define SEM_NAME "/my_semaphore" // Nombre del semáforo

int cantSlaves, iArgs = 1;
struct timeval timeout;

void callSlave(char *archivo);
void printFiles(char *path, int tabs);
void sendFile(char *archivo, int indice, int *filesInSlave, int *iArgs, int fd[][2]);

void sendFile(char *archivo, int indice, int *filesInSlave, int *iArgs, int fd[][2])
{
    // printf("File enviado %s\n",archivo);    // BORRAR
    write(fd[indice][1], archivo, strlen(archivo) + 1);
    write(fd[indice][1], "\n", 1);
    filesInSlave[indice]++;
    (*iArgs)++;
}

int main(int argc, char *argv[])
{
    char pathBuffer[512];
    printf("HOLA\n");
    if (argc < 2)
    {
        printf("Ingrese un argumento\n");
        return 1;
    }

    if (argc == 2)
        cantSlaves = 1; // xq si hay un solo archivo, con un solo esclavo estamo
    else
    {
        cantSlaves = argc / 5 + 1;
        if (cantSlaves > 10)
            cantSlaves = 10;
    }

    pid_t slaves[cantSlaves];
    pid_t view;
    // Arrays de pipes para la comunicación entre slaves y padre
    int pipesToSlave[cantSlaves][2];
    int pipesFromSlave[cantSlaves][2];

    int filesInSlave[cantSlaves];

    // este for crea los pipes
    for (int i = 0; i < cantSlaves; i++)
    {
        if (pipe(pipesToSlave[i]) == -1 || pipe(pipesFromSlave[i]) == -1)
        {
            perror("Error al crear el pipe");
            exit(1);
        }

        filesInSlave[i] = 0; // inicializo ambos contadores

        slaves[i] = fork();
        if (slaves[i] == -1)
        {

            perror("Error al crear el proceso");
            exit(1);
        }

        if (slaves[i] == 0)
        { // proceso hijo
            // cierro los estandar y los que no uso

            if (close(0) == -1)
            {
                perror("No se pudo cerrar el file descriptor estandar de lectura");
                return 1;
            }
            if (close(1) == -1)
            {
                perror("No se pudo cerrar el file descriptor estandar de escritura");
                return 1;
            }
            // reasigno los fd
            dup(pipesToSlave[i][0]);
            dup(pipesFromSlave[i][1]);

            // cierro los fd repetidos

            if (close(pipesToSlave[i][0]) == -1)
            {
                perror("No se pudo cerrar el file descriptor al hijo de lectura");
                return 1;
            }
            if (close(pipesFromSlave[i][1]) == -1)
            {
                perror("No se pudo cerrar el file descriptor desde el hijo de escritura");
                return 1;
            }

            for (int j = 0; j <= i; j++)
            {

                if (close(pipesToSlave[j][1]) == -1)
                {
                    perror("No se pudo cerrar el file descriptor al hijo de escritura");
                    return 1;
                }
                if (close(pipesFromSlave[j][0]) == -1)
                {
                    perror("No se pudo cerrar el file descriptor desde el hijo de lectura");
                    return 1;
                }
            }

            char *argv1[] = {"./child", NULL}; // le puse argv1 para q sea diferente al argv
            execve("./child", argv1, NULL);
            perror("execve"); // Esto solo se ejecuta si execve falla
            exit(1);
        }
        else
        { // proceso padre
            // cierro los estandar y los que no uso

            close(pipesToSlave[i][0]);
            close(pipesFromSlave[i][1]);

            for (int k = 0; k < INITIALARGS; k++)
            {
                if (iArgs >= argc)
                {                    // es muy improbable que se llegue a este caso
                    k = INITIALARGS; // para salir del for interno
                }
                else
                {
                    sendFile(argv[iArgs], i, filesInSlave, &iArgs, pipesToSlave);
                }
            }
        }
    }

    // Configurar los descriptores de archivo para select
    fd_set read_fds;
    FD_ZERO(&read_fds);
    int max_fd = -1;

    for (int i = 0; i < cantSlaves; i++)
    {
        FD_SET(pipesFromSlave[i][0], &read_fds);
        if (pipesFromSlave[i][0] > max_fd)
        {
            max_fd = pipesFromSlave[i][0];
        }
    }

    view = fork();
    if (view == 0)
    {
        sleep(2);
        printf(SHAREDMEMORY);
        exit(1);
    }

    /*
        int shm_fd;
        void *shm_ptr;
        sem_t *sem;

        // Crear o abrir el semáforo
        sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
        if (sem == SEM_FAILED) {
            perror("sem_open");
            exit(1);
        }

        // Crear o abrir la memoria compartida
        shm_fd = shm_open("/my_shm", O_CREAT | O_RDWR, 0666);
        if (shm_fd == -1) {
            perror("shm_open");
            exit(1);
        }

        // Configurar el tamaño de la memoria compartida
        ftruncate(shm_fd, SHM_SIZE);
    */

    int counter=0;

    while (1)
    {

        fd_set tmp_fds = read_fds;
        timeout.tv_sec = 4; // 3 segundos
        timeout.tv_usec = 0;

        int ready = select(max_fd + 1, &tmp_fds, NULL, NULL, &timeout);

        if (ready == -1)
        {
            perror("Error en select");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < cantSlaves; i++)
        {

            if (FD_ISSET(pipesFromSlave[i][0], &tmp_fds))
            {

                // Leer datos del descriptor de archivo pipes[i][0] y procesarlos
                char buffer[BUFFERSIZE];
                ssize_t bytes_read = read(pipesFromSlave[i][0], buffer, sizeof(buffer));
                if (bytes_read > 0)
                {
                    //printf("%s\n",buffer);
                    write(1,buffer,bytes_read);
                    //write(1,"\n",1);

                    /*FILE *archivo;
    
                    archivo = fopen("shm.txt", "w");

                    if (archivo == NULL) {
                        printf("No se pudo abrir el archivo.\n");
                        return 1; // Salir del programa con un código de error
                    }

                    fprintf(archivo, "Este es un texto que se escribirá en el archivo.\n");
                    
                    //fclose(archivo);

                    printf("Archivo creado y escrito con éxito.\n");

                    return 0;
                    */
                    
                    char *block = attach_memory_block(SHAREDMEMORY, BLOCK_SIZE);
                    //sleep(5);
                    if(block == NULL){
                        printf("ERROR: no pudimos obtener block\n");
                        //return -1;
                    }

                    strncpy(block,buffer,bytes_read);
                    counter+=bytes_read;

                    detach_memory_block(block);
                    filesInSlave[i]--;

                    if (filesInSlave[i] == 0)
                    {
                        if (iArgs < argc)
                        {
                            sendFile(argv[iArgs], i, filesInSlave, &iArgs, pipesToSlave);
                        }
                        else
                        {
                            // Como no hay mas archivos mato al slave q acaba de entregarme
                            /*char txtKill[100];
                            snprintf(txtKill,100,"kill %d",slaves[i]);
                            system(txtKill);*/
                            if (close(pipesToSlave[i][1]) == -1)
                            {
                                perror("No se pudo cerrar el file descriptor de escritura al hijo");
                                return 1;
                            }

                            int j = 0;
                            while (filesInSlave[j] == 0) // checkeo q no haya archivos pendientes
                            {
                                j++;
                            }
                            if (j == cantSlaves)
                            {
                                for (int k = 0; k < cantSlaves; k++)
                                {
                                    if (close(pipesFromSlave[k][0]) == -1)
                                    {
                                        perror("No se pudo cerrar el file descriptor de lecutra del hijo");
                                        return 1;
                                    }
                                }
                                exit(1);
                            }
                        }
                    }
                }
                else if (bytes_read != 0)
                {
                    perror("Error al leer del pipe");
                }
            }
        }
    }
    /*

        // ESPERA A QUE TERMINEN TODOS LOS PROCESOS HIJOS
        for (int i = 0; i < cantSlaves; i++) {
            printf("Esperando que terminen los pibes\n");
            int status;
            waitpid(-1, &status, 0); // Esperar a que cualquier proceso hijo termine
            if (WIFEXITED(status)) {
                printf("Proceso hijo terminado con estado %d.\n", WEXITSTATUS(status));
            }
        }
        printf("Todos los procesos hijos han terminado. Programa principal finalizado.\n");
        */
}
