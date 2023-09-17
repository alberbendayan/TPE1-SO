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
    sleep(2);   
    char msg[1024];
    if(argc <2){
        scanf("%s",msg);
    }else{
        char *aux=argv[1];
        for(int i=0;i<1024 && aux[i]!=0 ;i++){
            msg[i]=aux[i];
        }
    }
    SharedMemoryPtr memory = connectToSharedMemory(msg);
    char buffer[BUFFERSIZE];
    int actualPos =0, prevPos=0;
    while(1)
    {
        actualPos = readMemory(memory,buffer,actualPos,BUFFERSIZE);
        
        if(actualPos > prevPos){
            int longitud = strlen(buffer);
            //write(1,buffer,longitud+1);
            printf("%s",buffer);
            // limpio el buffer 
            for(int i=0;i<=longitud;i++){
                buffer[i]=0;
            }
            
            prevPos=actualPos;
            if(isFinished(memory,actualPos)){
                exit(1);
            }
        }
    }
}