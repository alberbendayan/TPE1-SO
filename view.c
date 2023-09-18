// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "view.h"

int main(int argc, char *argv[])
{
    sleep(2);   
    char msg[1024];
    if(argc <2){
        scanf("%1023s",msg);
    }else{
        char *aux=argv[1];
        for(int i=0;i<1024 && aux[i]!=0 ;i++){
            msg[i]=aux[i];
        }
    }
    SharedMemoryPtr memory = connectToSharedMemory(msg);
    char buffer[BUFFERSIZE];
    int actualPos = 0, prevPos = 0;
    while(1)
    {
        actualPos = readMemory(memory,buffer,actualPos,BUFFERSIZE);
        
        if(actualPos > prevPos){
            if(buffer[0]=='+'){
                destroySharedMemory(memory);
                exit(1);
            }
            printf("%s",buffer);
            //sleep(1);      
            prevPos=actualPos;

        }
    }
}