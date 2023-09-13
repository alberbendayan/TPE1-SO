#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <stdbool.h>

#define BLOCK_SIZE 65536

char *attach_memory_block(char *filename, int size);

bool detach_memory_block(char *block);

bool destroy_memory_block(char *filename);

#endif