#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include "sharedMemory.h"



#define SHM_SIZE 1024            // Tamaño de la memoria compartida

int main()
{
    sleep(2);
    char msg[1024];
    
    //read(0, msg, 1024);

    scanf("%s",msg);

    printf("Desde view: %s\n",msg);


    
    SharedMemoryPtr memory = connectToSharedMemory(msg);
    char buffer[BUFFERSIZE];
    int actualPos =0, posVieja=0;

    while(1)
    {
        actualPos = readMemory(memory,buffer,actualPos,BUFFERSIZE);
        if(actualPos > posVieja){
            write(1,buffer,strlen(buffer)+1);
            posVieja=actualPos;
            if(isFinished(memory)){
                exit(1);
            }
        }
    }

    /*while (1)
    {
        sleep(2);
    }
    return 1;*/

    /*int shm_fd;
    void *shm_ptr;
    sem_t *sem;

    // Abrir el semáforo existente
    sem = sem_open(SEM_NAME, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    // Abrir la memoria compartida existente
    shm_fd = shm_open("/my_shm", O_RDONLY, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    // Mapear la memoria compartida en el espacio de direcciones del proceso
    shm_ptr = mmap(0, SHM_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    while (1)
    {
        sem_wait(sem);
        // Leer desde la memoria compartida
        printf("Saliendo desde la view: %s\n", (char *)shm_ptr);
        // Liberar el semáforo
        sem_post(sem);
    }*/
}