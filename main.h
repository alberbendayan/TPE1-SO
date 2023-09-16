#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include "sharedMemory.h"

#define SHAREDMEMORY "/my_shm"

void sendFile(char *file, int index, int *filesInSlave, int *iArgs, int fd[][2]);