// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
    while(fgets(msg,READ_BUFFER_SIZE,stdin)){
        makeMD5(msg,hashMD5);
        // lo q nos pasa con valgrind es que nos escribe tan rapido q el main lee todo como un solo bloque
        // sin valgrind no es necesario este sleep
        sleep(1);
        write(1,hashMD5,strlen(hashMD5)+1);
    }
    exit(1);
}

void makeMD5(char *argv, char *hash)
{
    const char *filename = argv;
    pid_t pid = getpid();
    char command[256]; // Suponemos que el comando no va a ser mas largo que 256 caracteres
    snprintf(command, sizeof(command), "md5sum ./%s", filename);
    // Ejecuta el comando md5sum
    FILE *pipeMD5 = popen(command, "r");
    if (pipeMD5==NULL)
    {
        perror("Failed to open the process");
        exit(1);
    }
    char result[LONG_HASHMD5]; // El hash MD5 tiene 32 caracteres, más el carácter nulo = 33
    if (fgets(result, LONG_HASHMD5, pipeMD5) == NULL)
    {
        perror("Failed to read the result");
        pclose(pipeMD5);
        exit(1);
    }
    pclose(pipeMD5);
    snprintf(hash, MAX_LONG_RET, "PID: %d - HASH: %s - FILE: %s\n", pid, result, filename);
    return;
}
