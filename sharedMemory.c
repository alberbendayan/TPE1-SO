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
};

struct SharedMemory *createSharedMemory(const char *name) {
    int fd = shm_open(name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("shm_open");
        return NULL;
    }

    struct SharedMemory *memory = (struct SharedMemory *)mmap(NULL, sizeof(struct SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (memory == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return NULL;
    }

    memory->fd = fd;
    memset(memory->buffer, 0, BUFFERSIZE); // Inicializamos el arreglo a ceros

    return memory;
}

void destroySharedMemory(struct SharedMemory *memory) {
    if (memory != NULL) {
        munmap(memory, sizeof(struct SharedMemory));
        close(memory->fd);
    }
}

char *getBuffer(struct SharedMemory *memory) {
    if (memory != NULL) {
        return memory->buffer;
    }
    return NULL;
}

size_t getSize(struct SharedMemory *memory) {
    return MAX_BUFFER_SIZE;
}
