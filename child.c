#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define READ_BUFFER_SIZE 1024
#define LONG_HASHMD5 33 // tiene uno demas x el caracter nulo

void makeMD5 (char *argv,char* hash);



int main(int argc, char *argv[]) {

    char hashMD5[32+1];
    char msg[READ_BUFFER_SIZE];
    int characterRead;
    while (1)
    {   
        characterRead=read(0,msg,READ_BUFFER_SIZE);
        msg[characterRead-1]='\0';
        if(characterRead>0){
            makeMD5(msg,hashMD5);
            write(1,hashMD5,strlen(hashMD5)+1);
        }
    }
    

}


void makeMD5 (char *argv,char* hash){
    const char *filename = argv;
    printf("***** %s\n",filename);
    FILE *file = fopen(filename, "rb");

    if (!file) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }
    char command[256]; // Suponemos que el comando no va a ser mas largo que 256 caracteres
    snprintf(command, sizeof(command), "md5sum %s", filename);
    //printf("Command: %s",command);

    // Ejecuta el comando md5sum
    //printf("Calculando el hash MD5...\n");

    FILE *pipeMD5 = popen(command, "r");
    if (!pipeMD5) {
        perror("Error al abrir el proceso");

        
    }
    char result[LONG_HASHMD5]; // El hash MD5 tiene 32 caracteres, más el carácter nulo = 33
    if (fgets(result, sizeof(result), pipeMD5) == NULL) {
        perror("Error al leer el resultado");
        pclose(pipeMD5);
        exit (1);
    }
    pclose(pipeMD5);
    // Elimina el espacio en blanco final si lo hay
    result[32] = '\0';
    for(int i=0;i<sizeof(result);i++){
        hash[i]=result[i];
    }
    printf("\n");
    return;
}


