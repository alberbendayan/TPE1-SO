#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "sharedMemory.h"
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>  



struct SharedMemory {
    int fd;                  // Descriptor de archivo para la memoria compartida
    char buffer[BUFFERSIZE];  // Buffer compartido (arreglo fijo)
    int writePos;
    char name[NAMESIZE];
};



SharedMemoryPtr createSharedMemory(const char *name) {
    shm_unlink(name); // ELIMINO LA MEMORIA ANTERIOR SI ES Q EXISTE
    
    int fd = shm_open(name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("shm_open");
        return NULL;
    }
    // Establece el tamaño de la memoria compartida
    if (ftruncate(fd, sizeof(struct SharedMemory)) == -1) {
        perror("ftruncate");
        close(fd);
        return NULL;
    }
    SharedMemoryPtr memory = (SharedMemoryPtr) mmap(NULL, sizeof(struct SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    for(int i=0;name[i]!=0;i++)
        memory->name[i]=name[i];

    if (memory == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return NULL;
    }

    memory->fd = fd;
    memory->writePos=0;
    //memset(memory->buffer, 0, sizeof(memory->buffer)); // Inicializamos el arreglo a ceros

    return memory;
}

SharedMemoryPtr connectToSharedMemory(const char *name) {
    int fd = shm_open(name, O_RDWR, S_IRUSR | S_IRGRP | S_IROTH);
    if (fd == -1) {
        perror("shm_open");
        return NULL;
    }
    // Establece el tamaño de la memoria compartida
    if (ftruncate(fd, sizeof(struct SharedMemory)) == -1) {
        perror("ftruncate");
        close(fd);
        return NULL;
    }
    SharedMemoryPtr memory = (SharedMemoryPtr) mmap(NULL, sizeof(struct SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    if (memory == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return NULL;
    }

    memory->fd = fd;    
    return memory;
}

void destroySharedMemory(SharedMemoryPtr memory) {
    if (memory != NULL) {
        munmap(memory, sizeof(struct SharedMemory));
        close(memory->fd);
        shm_unlink(memory->name); // ELIMINO LA MEMORIA
    
    }
}

int writeInMemory(SharedMemoryPtr memory, char * msg, int size){
    // wait (profe)
    // (manual no hace nada)
    if(memory->writePos + size>=BUFFERSIZE){
        perror("No hay espacio suficiente para escribir en el buffer\n");
        return -1;
    }else{
        int i;
        for(i=0;i<size-1;i++){
            memory->buffer[memory->writePos + i]=msg[i];
        }
        // memcopy
        memory->writePos+=i;
        return 1;
    }
    // post (profe / manual)

}

int readMemory (SharedMemoryPtr memory, char*msg,int inicialPosition,int bufferSize){
    // wait (profe y manual)
    if(inicialPosition >=BUFFERSIZE || inicialPosition < 0){
        perror("Out of bounds\n");
        return -1;
    }
    int i;
    for (i = 0; i < bufferSize && inicialPosition + i<= memory->writePos; i++, inicialPosition++)
    {
        msg[i]=memory->buffer[inicialPosition];
    }
    return inicialPosition;
    // post (solo profe)
}

// char *getBuffer(SharedMemoryPtr memory) {
//     if (memory != NULL) {
//         return memory->buffer;
//     }
//     return NULL;
// }

size_t getSize(SharedMemoryPtr memory) {
    return BUFFERSIZE;
}

