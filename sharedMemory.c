// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "sharedMemory.h"

struct SharedMemory {
    int fd;
    char buffer[BUFFERSIZE];
    int writePos;
    char name[NAMESIZE];
    sem_t canRead;
};

SharedMemoryPtr createSharedMemory(const char *name){
    //semaforo
    

    int fd = shm_open(name, O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("shm_open error");
        sem_unlink(name);
        exit(1);
    }

    if (ftruncate(fd, sizeof(struct SharedMemory)) == -1) {
    //if (ftruncate(fd, BUFFERSIZE) == -1) {
        perror("ftruncate error");
        close(fd);
        sem_unlink(name);
        exit(1);
    }

    SharedMemoryPtr memory = (SharedMemoryPtr) mmap(NULL, sizeof(struct SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (memory == MAP_FAILED) {
        perror("mmap error");
        close(fd);
        sem_unlink(name);
        exit(1);
    }

    if (sem_init(&memory->canRead, 1, 0) == -1) {
        perror("sem_init error");
        exit(EXIT_FAILURE);
    }

    memory->fd = fd;
    memory->writePos = 0;

    for (int i = 0;  i < NAMESIZE && name[i] != 0; i++)
        memory->name[i] = name[i];

    return memory;
}

void disconnectSharedMemory (SharedMemoryPtr memory){
    munmap(memory, sizeof(struct SharedMemory));
}


SharedMemoryPtr connectToSharedMemory(const char *name) {
    int fd = shm_open(name, O_RDWR, S_IRUSR | S_IRGRP | S_IROTH);
    if (fd == -1) {
        perror("shm_open error");
        exit(1);
    }

    SharedMemoryPtr memory = (SharedMemoryPtr) mmap(NULL, sizeof(struct SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (memory == MAP_FAILED) {
        perror("mmap error");
        close(fd);
        exit(1);
    }

    memory->fd = fd;

    return memory;
}

int writeInMemory(SharedMemoryPtr memory, char *msg, int size) {
    if (memory->writePos + size >= BUFFERSIZE) {
        perror("There is not enough buffer writing space\n");
        return -1;
    } else {
        int i;
        for (i = 0; i < size  && msg[i] != 0; i++) {
            memory->buffer[memory->writePos + i] = msg[i];
        }
        memory->writePos += i;
        sem_post(&memory->canRead);
        return 1;
    }
}


int readMemory(SharedMemoryPtr memory, char *msg, int initialPosition, int bufferSize) {
    
    if (initialPosition >= BUFFERSIZE || initialPosition < 0) {
        perror("Out of bounds\n");
        return -1;
    }
    sem_wait(&memory->canRead);
    int i;
    for (i = 0; i < bufferSize &&  memory->buffer[initialPosition + i]!='\n' && initialPosition + i <= memory->writePos; i++) {
        msg[i] = memory->buffer[initialPosition + i];
    }
    if(memory->buffer[initialPosition + i]=='\n'){
        msg[i++]='\n';
    }
    msg[i]='\0';
    return initialPosition + i;
}

void destroySharedMemory(SharedMemoryPtr memory) {
    if (memory != NULL) {
        sem_destroy(&memory->canRead);
        close(memory->fd);
        shm_unlink(memory->name);
        munmap(memory, sizeof(struct SharedMemory));

    }
}

