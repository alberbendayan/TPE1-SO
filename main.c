// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
#include "sharedMemory.h"

#define SHAREDMEMORY "/my_shm"

void sendFile(char *file, int index, int *filesInSlave, int *iArgs, int fd[][2]);

int slavesQuantity, iArgs = 1,initialArgs;
struct timeval timeout;

int main(int argc, char *argv[])
{
    setvbuf(stdout, NULL, _IONBF, 0);

    if (argc < 2)
    {
        perror("Wrong arguments\n");
        return 1;
    }
    slavesQuantity = argc / 5 + 1;
    if (slavesQuantity > 10)
        slavesQuantity= 10;

    initialArgs = argc / 15 + 2;
 

    pid_t slaves[slavesQuantity];
    // Arrays de pipes para la comunicación entre slaves y padre
    int pipesToSlave[slavesQuantity][2];
    int pipesFromSlave[slavesQuantity][2];

    int filesInSlave[slavesQuantity];

    // este for crea los pipes
    for (int i = 0; i < slavesQuantity; i++)
    {
        if (pipe(pipesToSlave[i]) == -1 || pipe(pipesFromSlave[i]) == -1)
        {
            perror("Pipe could not be created");
            exit(1);
        }
        filesInSlave[i] = 0; // inicializo ambos contadores
        slaves[i] = fork();
        if (slaves[i] == -1)
        {
            perror("Error initializing the process");
            exit(1);
        }
        if (slaves[i] == 0)
        { // proceso hijo
            // cierro los estandar y los que no uso
            if (close(0) == -1)
            {
                perror("Failed to close reading standard file descriptor");
                return 1;
            }
            if (close(1) == -1)
            {
                perror("Failed to close writing standard file descriptor");
                return 1;
            }
            // reasigno los fd
            dup(pipesToSlave[i][0]);
            dup(pipesFromSlave[i][1]);
            // cierro los fd repetidos
            if (close(pipesToSlave[i][0]) == -1)
            {
                perror("Failed to close reading standard file descriptor to slave");
                return 1;
            }
            if (close(pipesFromSlave[i][1]) == -1)
            {
                perror("Failed to close writing standard file descriptor from slave");
                return 1;
            }
            for (int j = 0; j <= i; j++)
            {
                if (close(pipesToSlave[j][1]) == -1)
                {
                    perror("Failed to close writing standard file descriptor to slave");
                    return 1;
                }
                if (close(pipesFromSlave[j][0]) == -1)
                {
                    perror("Failed to close reading standard file descriptor from slave");
                    return 1;
                }
            }

            char *argv1[] = {"./child", NULL}; // le puse argv1 para q sea diferente al argv
            execve("./child", argv1, NULL);
            perror("execve failed"); // Esto solo se ejecuta si execve falla
            exit(1);
        }
        else
        { // proceso padre
            // cierro los estandar y los que no uso

            close(pipesToSlave[i][0]);
            close(pipesFromSlave[i][1]);

            for (int k = 0; k < initialArgs; k++)
            {    
                if (iArgs >= argc)
                {   
                    k = initialArgs; 
                }
                else
                {
                    sendFile(argv[iArgs], i, filesInSlave, &iArgs, pipesToSlave);
                }
            }
        }
    }
    FILE *file;
    file = fopen("result.txt", "w");
    if (file == NULL) {
        perror("File could not be opened.\n");
        
        return 1; // Salir del programa con un código de error
    }
    SharedMemoryPtr memory = createSharedMemory(SHAREDMEMORY);
    puts(SHAREDMEMORY);
    while (1)
    {
        // Configurar los descriptores de archivo para select
        fd_set read_fds;
        FD_ZERO(&read_fds);
        int max_fd = -1;
        for (int i = 0; i < slavesQuantity; i++)
        {
            FD_SET(pipesFromSlave[i][0], &read_fds);
            if (pipesFromSlave[i][0] > max_fd)
            {
                max_fd = pipesFromSlave[i][0];
            }
        }
        fd_set tmp_fds = read_fds;
        int ready = select(max_fd + 1, &tmp_fds, NULL, NULL,NULL);
        if (ready == -1)
        {
            perror("Error in select");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < slavesQuantity; i++)
        {
            if (FD_ISSET(pipesFromSlave[i][0], &tmp_fds))
            {
                // Leer datos del descriptor de archivo pipes[i][0] y procesarlos
                char buffer[BUFFERSIZE];
                ssize_t bytesRead = read(pipesFromSlave[i][0], buffer, sizeof(buffer));
                if (bytesRead > 0)
                {
                    writeInMemory(memory,buffer,bytesRead);  
                    fprintf(file,"%s", buffer);
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
                            if (close(pipesToSlave[i][1]) == -1)
                            {
                                perror("Failed to close writing file descriptor to slave");
                                return 1;
                            }
                            int j = 0;
                            for(j=0;j <slavesQuantity && filesInSlave[j] == 0;j++);
                            if (j == slavesQuantity)
                            {
                                for (int k = 0; k < slavesQuantity; k++)
                                {
                                    if (close(pipesFromSlave[k][0]) == -1)
                                    {
                                        perror("Failed to close reading file descriptor from slave");
                                        return 1;
                                    }
                                } 
                                writeInMemory(memory,"+",2); 
                                disconnectSharedMemory(memory);
                                fclose(file);                           
                                exit(1);
                            }
                        }
                    }
                }
                else if (bytesRead != 0)
                {
                    perror("Failed to read from pipe");
                }
            }
        }
    }
}

void sendFile(char *file, int index, int *filesInSlave, int *iArgs, int fd[][2])
{
    write(fd[index][1], file, strlen(file) + 1);
    write(fd[index][1], "\n", 1);
    filesInSlave[index]++;
    (*iArgs)++;
}
