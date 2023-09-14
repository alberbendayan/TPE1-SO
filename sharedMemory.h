#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <stddef.h>

#define BUFFERSIZE 65536

struct SharedMemory;

typedef struct SharedMemory * SharedMemoryPtr;

SharedMemoryPtr createSharedMemory(const char *name);
void destroySharedMemory(SharedMemoryPtr memory);
int writeInMemory(SharedMemoryPtr memory, char * msg, int size);
int readMemory (SharedMemoryPtr memory, char*msg,int inicialPosition,int bufferSize);
//char *getBuffer(SharedMemoryPtr memory);
size_t getSize(SharedMemoryPtr memory);

#endif
