#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <stddef.h>

#define BUFFERSIZE 4096

struct SharedMemory;

struct SharedMemory *createSharedMemory(const char *name, size_t size);
void destroySharedMemory(struct SharedMemory *memory);
char *getBuffer(struct SharedMemory *memory);
size_t getSize(struct SharedMemory *memory);

#endif
