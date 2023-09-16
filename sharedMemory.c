#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <errno.h>
#include "sharedMemory.h"


#define BUFFERSIZE 65536

struct SharedMemory {
    int fd;
    char buffer[BUFFERSIZE];
    int writePos;
    char name[NAMESIZE];
    sem_t *sem;
    bool finished;
};

typedef struct SharedMemory* SharedMemoryPtr;

SharedMemoryPtr createSharedMemory(const char *name) {
    shm_unlink(name);
    sem_t *sem = sem_open(name, O_CREAT, S_IRUSR | S_IWUSR, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        return NULL;
    }

    int fd = shm_open(name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("shm_open");
        sem_close(sem);
        sem_unlink(name);
        return NULL;
    }

    if (ftruncate(fd, sizeof(struct SharedMemory)) == -1) {
        perror("ftruncate");
        close(fd);
        sem_close(sem);
        sem_unlink(name);
        return NULL;
    }

    SharedMemoryPtr memory = (SharedMemoryPtr) mmap(NULL, sizeof(struct SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (memory == MAP_FAILED) {
        perror("mmap");
        close(fd);
        sem_close(sem);
        sem_unlink(name);
        return NULL;
    }

    memory->fd = fd;
    memory->writePos = 0;
    memory->sem = sem;
    memory->finished=false;

    for (int i = 0; name[i] != 0 && i < NAMESIZE; i++)
        memory->name[i] = name[i];

    return memory;
}

void finishedWriting(SharedMemoryPtr memory){
    memory->finished=true;
}

bool isFinished(SharedMemoryPtr memory,int readingPosition){
    return memory->finished  && readingPosition >= memory->writePos;
}

SharedMemoryPtr connectToSharedMemory(const char *name) {
    int fd = shm_open(name, O_RDWR, S_IRUSR | S_IRGRP | S_IROTH);
    if (fd == -1) {
        perror("shm_open");
        return NULL;
    }

    if (ftruncate(fd, sizeof(struct SharedMemory)) == -1) {
        perror("ftruncate");
        close(fd);
        return NULL;
    }

    sem_t *sem = sem_open(name, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        close(fd);
        return NULL;
    }

    SharedMemoryPtr memory = (SharedMemoryPtr) mmap(NULL, sizeof(struct SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (memory == MAP_FAILED) {
        perror("mmap");
        close(fd);
        sem_close(sem);
        return NULL;
    }

    memory->fd = fd;
    memory->sem = sem;

    return memory;
}

int writeInMemory(SharedMemoryPtr memory, char *msg, int size) {
    if (memory->writePos + size >= BUFFERSIZE) {
        perror("No hay espacio suficiente para escribir en el buffer\n");
        return -1;
    } else {
        //printf("msg: %s",msg);
        // sleep(1);

        int i;
        for (i = 0; i < size  && msg[i] != 0; i++) {
            memory->buffer[memory->writePos + i] = msg[i];
        }
        memory->writePos += i;
        sem_post(memory->sem);
        return 1;
    }
}


int readMemory(SharedMemoryPtr memory, char *msg, int inicialPosition, int bufferSize) {
    
    if (inicialPosition >= BUFFERSIZE || inicialPosition < 0) {
        perror("Out of bounds\n");
        return -1;
    }
    if(memory->finished==false){ //Esto permite que llame al proceso view varias veces a pesar de correr el main una sola vez
        sem_wait(memory->sem);
    }
    int i;
    for (i = 0; i < bufferSize &&  memory->buffer[inicialPosition + i]!='\n' && inicialPosition + i <= memory->writePos; i++) {
        msg[i] = memory->buffer[inicialPosition + i];
    }
    if(memory->buffer[inicialPosition + i]=='\n'){
        msg[i++]='\n';
    }
    msg[i]='\0';
    return inicialPosition + i;
}

void destroySharedMemory(SharedMemoryPtr memory) {
    if (memory != NULL) {
        sem_close(memory->sem);
        munmap(memory, sizeof(struct SharedMemory));
        close(memory->fd);
        shm_unlink(memory->name);
    }
}

