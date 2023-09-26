// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include "sharedMemory.h"

int main(int argc, char *argv[])
{
    
    //setvbuf(stdout, NULL, _IONBF, 0);
    char buffer [BUFFERSIZE];
    int fifo=open("/tmp/namedPipe",O_RDONLY);
    
    while(read(fifo,buffer,BUFFERSIZE))
    {
        printf("%s",buffer);       
    }
}