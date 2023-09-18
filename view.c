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
    int actualPos =0, prevPos=0;
    while(1)
    {
        actualPos = readMemory(memory,buffer,actualPos,BUFFERSIZE);
        
        if(actualPos > prevPos){
            int longitud = strlen(buffer);
            if(buffer[longitud-1]=='+'){
                buffer[longitud-1]='\0';
                printf("%s",buffer);
                exit(1);
            }
            //write(1,buffer,longitud+1);
            printf("%s",buffer);
            // limpio el buffer 
            for(int i=0;i<=longitud;i++){
                buffer[i]=0;
            }
            
            prevPos=actualPos;
            if(isFinished(memory,actualPos)){
                destroySharedMemory(memory);
                exit(1);
            }
        }
    }
}