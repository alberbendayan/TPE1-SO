#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define READ_BUFFER_SIZE 1024
#define MAX_LONG_RET 256
#define LONG_HASHMD5 33 // tiene uno demas x el caracter nulo

void makeMD5(char *argv, char *hash);

int main(int argc, char *argv[])
{

    char hashMD5[MAX_LONG_RET];
    char msg[READ_BUFFER_SIZE];
    int characterRead;

<<<<<<< Updated upstream
    /*for(int i=1;i<argc;i++){
        makeMD5(argv[i],hashMD5);
        write(1,hashMD5,strlen(hashMD5)+1);
    }*/
    /*while(1){
        characterRead=read(0,msg,READ_BUFFER_SIZE);
        msg[characterRead-1]='\0';
        if(characterRead>0){
            //write(1,msg,strlen(msg)+1);
            makeMD5(msg,hashMD5);
            write(1,hashMD5,strlen(hashMD5)+1);
        }
    }*/

    // while(fgets(msg,sizeof(msg),stdin) != NULL){

    /* while(characterRead=read(0,msg,READ_BUFFER_SIZE)>0){

         //characterRead=read(0,msg,READ_BUFFER_SIZE);
         //msg[characterRead-1]='\0';
         if(msg[0]!=NULL){
             //write(1,msg,strlen(msg)+1);
             makeMD5(msg,hashMD5);
             write(1,hashMD5,strlen(hashMD5)+1);
         }
     }*/
    int j = 0;
    char txt[READ_BUFFER_SIZE];
    /*while(1){
        characterRead=read(0,msg,READ_BUFFER_SIZE);*/
    while ((characterRead = read(0, msg, READ_BUFFER_SIZE)))
    {
        for (int i = 0; i < characterRead; i++)
        {
            if (msg[i] != '\n')
            {
                txt[j] = msg[i];
=======

    int j=0;
    char txt[READ_BUFFER_SIZE];
    while((characterRead=read(0,msg,READ_BUFFER_SIZE))){
        for(int i=0;i<characterRead;i++){
            if(msg[i]!='\n'){
                txt[j]=msg[i];
>>>>>>> Stashed changes
                j++;
            }
            else
            {
                txt[j] = 0;
                makeMD5(txt, hashMD5);
                write(1, hashMD5, strlen(hashMD5) + 1);
                j = 0;
            }
        }
    }
    exit(1);
}

void makeMD5(char *argv, char *hash)
{
    const char *filename = argv;
<<<<<<< Updated upstream

    pid_t pid = getpid();

    // printf("***** %s\n",filename);
    /*FILE *file = fopen(filename, "rb");

    if (!file) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }*/

=======
    
    pid_t pid = getpid();    
>>>>>>> Stashed changes
    char command[256]; // Suponemos que el comando no va a ser mas largo que 256 caracteres
    snprintf(command, sizeof(command), "md5sum ./%s", filename);

    // Ejecuta el comando md5sum
    FILE *pipeMD5 = popen(command, "r");
    if (!pipeMD5)
    {
        perror("Error al abrir el proceso");
    }

    char result[LONG_HASHMD5]; // El hash MD5 tiene 32 caracteres, más el carácter nulo = 33
    if (fgets(result, LONG_HASHMD5, pipeMD5) == NULL)
    {
        perror("Error al leer el resultado");
        pclose(pipeMD5);
        exit(1);
    }
    pclose(pipeMD5);

    snprintf(hash, MAX_LONG_RET, "PID: %d - HASH: %s - FILE: %s\n", pid, result, filename);

    return;
}
