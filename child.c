#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>

#define READ_BUFFER_SIZE 1024

void makeMD5 (char *argv[]);


int main(int argc, char *argv[]) {


    for(int i=1;i<argc;i++)
        makeMD5(&argv[i]);

}

void makeMD5 (char *argv){
    const char *filename = argv;
    FILE *file = fopen(filename, "rb");

    if (!file) {
        printf("No se pudo abrir el archivo.\n");
        return 1;
    }

    MD5_CTX md5Context;
    MD5_Init(&md5Context);

    unsigned char buffer[READ_BUFFER_SIZE];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        MD5_Update(&md5Context, buffer, bytesRead);
    }

    unsigned char hash[MD5_DIGEST_LENGTH];
    MD5_Final(hash, &md5Context);
    // en hash queda guardado el md5
    fclose(file);

    printf("El MD5 del archivo %s es: ", filename);
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");

    return 0;
}