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
#define SEM_NAME "/my_semaphore" // Nombre del semáforo

int main()
{
    char msg[1024];
    while (read(0, msg, strlen(msg)+1));

    printf("Desde view: %s\n",msg);


    
    SharedMemoryPtr memory = connectToSharedMemory(msg);
    char buffer[BUFFERSIZE];
    int actualPos = readMemory(memory,buffer,0,BUFFERSIZE);
    write(1,buffer,1000);
    
    while (1)
    {
        printf("HOLA2\n");
        sleep(2);
        // up
        
        /*char *block = attach_memory_block(msg, BLOCK_SIZE);
        
        if (block == NULL)
        {
            printf("ERROR: no pudimos obtener block\n");
        }
        int j;
        for(j=0;block[j];j++);
        printf("Reading %s \n", block+counter);
        counter+=j;
        detach_memory_block(block);*/
        



        
        // down
    }
    return 1;

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