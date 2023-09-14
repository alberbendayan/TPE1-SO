#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H

#include <stdbool.h>

<<<<<<< Updated upstream
#define BLOCK_SIZE 65536
=======
#define BLOCK_SIZE 4096
>>>>>>> Stashed changes

char *attach_memory_block(char *filename, int size);

bool detach_memory_block(char *block);

bool destroy_memory_block(char *filename);

#endif