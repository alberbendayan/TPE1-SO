#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "sharedMemory.h"

struct SharedMemory {
    int fd;                  // Descriptor de archivo para la memoria compartida
    char buffer[BUFFERSIZE];  // Buffer compartido (arreglo fijo)
    int writePos;
};



SharedMemoryPtr createSharedMemory(const char *name) {
    
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
    
    if (memory == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return NULL;
    }
    printf("%d\n",fd);
    
    memory->fd = fd;
    memory->writePos=0;
    memset(memory->buffer, 0, sizeof(memory->buffer)); // Inicializamos el arreglo a ceros

    return memory;
}

void destroySharedMemory(SharedMemoryPtr memory) {
    if (memory != NULL) {
        munmap(memory, sizeof(struct SharedMemory));
        close(memory->fd);
    }
}

int writeInMemory(SharedMemoryPtr memory, char * msg, int size){
    if(memory->writePos + size>=BUFFERSIZE){
        perror("No hay espacio suficiente para escribir en el buffer\n");
        return -1;
    }else{
        for(int i=0;i<size;i++, memory->writePos++){
            memory->buffer[memory->writePos]=msg[i];
        }
        return 1;
    }

}

int readMemory (SharedMemoryPtr memory, char*msg,int inicialPosition,int bufferSize){
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
