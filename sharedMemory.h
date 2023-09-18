#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <errno.h>
#include <stddef.h>
#include <stdbool.h>

#define BUFFERSIZE 65536
#define NAMESIZE 128

struct SharedMemory;

typedef struct SharedMemory * SharedMemoryPtr;

SharedMemoryPtr createSharedMemory(const char *name);
void destroySharedMemory(SharedMemoryPtr memory);
int writeInMemory(SharedMemoryPtr memory, char * msg, int size);
SharedMemoryPtr connectToSharedMemory(const char *name);
int readMemory (SharedMemoryPtr memory, char*msg,int inicialPosition,int bufferSize);
size_t getSize(SharedMemoryPtr memory);
bool isFinished(SharedMemoryPtr memory,int readingPosition);
void finishedWriting(SharedMemoryPtr memory);
void disconectSharedMemory (SharedMemoryPtr memory);
#endif
