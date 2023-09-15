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



int main()
{
    sleep(2);
    char msg[1024];
    
    //read(0, msg, 1024);

    scanf("%s",msg);

    
    SharedMemoryPtr memory = connectToSharedMemory(msg);
    char buffer[BUFFERSIZE];
    
    int actualPos = 0, posVieja =0;

    while(1){

        actualPos=readMemory(memory,buffer,actualPos,BUFFERSIZE);
        if(actualPos>posVieja){
            printf("%s\n",buffer);
            posVieja=actualPos;
            if(isFinished(memory))
                exit(1);
        }
    }


}