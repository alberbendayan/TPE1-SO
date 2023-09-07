#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

//#define SLAVES 2

int cantSlaves;

void callSlave (char * archivo);
void printFiles(char* path,int tabs);

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Ingrese un argumento\n");
        return 1;
        //return 1;
    }

    cantSlaves = argc/10 + 1;
    if (cantSlaves >10)
        cantSlaves=10;

    printf("cant slaves %d\n",cantSlaves);
    // Un esclavo cada 10 archivos y como maximo 10 esclavos

    pid_t slaves[cantSlaves];

    int pipesToSlave[cantSlaves][2]; // Un array de pipes para la comunicación entre slaves y padre
    int pipesFromSlave[cantSlaves][2];

    // este for crea los pipes
    for (int i = 0; i < cantSlaves; i++) {
        if (pipe(pipesToSlave[i]) == -1 || pipe(pipesFromSlave[i]) == -1) {
            perror("Error al crear el pipe");
            exit(1);
        }
    }

    for (int i=0; i<cantSlaves; i++){
        // MODULARIZAR
        slaves[i] = fork();
        if(slaves[i]==-1){
            perror("Error al crear el proceso");
            exit(1);
        }

        if(slaves[i]==0){ // proceso hijo
            // CLOSE 0 1
        }
    }
/*
    for (int i = 0; i < slaves; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("Error al crear el proceso");
            exit(1);
        } else if (pid == 0) {
            // Código del proceso hijo
            if (i > 0) {
                // Redirigir la entrada estándar del proceso hijo desde el pipe anterior
                close(pipes[i - 1][1]);
                dup2(pipes[i - 1][0], STDIN_FILENO);
                close(pipes[i - 1][0]);
            }

            if (i < slaves - 1) {
                // Redirigir la salida estándar del proceso hijo al pipe
                close(pipes[i][0]);
                dup2(pipes[i][1], STDOUT_FILENO);
                close(pipes[i][1]);
            }

            // Ejecutar un programa usando execve
            char *argv[] = {"./child","./child", NULL}; // EL SEGUNDO PARAMETRO ES EL ARCHIVOO
            execve("./child", argv, NULL);
            perror("execve"); // Esto solo se ejecuta si execve falla
            exit(1);
        } else {
            slaves[i] = pid;
        }
    }

    //printFiles(argv[1],0);
*/

    // ESPERA A QUE TERMINEN TODOS LOS PROCESOS HIJOS
    for (int i = 0; i < cantSlaves; i++) {
        int status;
        waitpid(-1, &status, 0); // Esperar a que cualquier proceso hijo termine
        if (WIFEXITED(status)) {
            printf("Proceso hijo terminado con estado %d.\n", WEXITSTATUS(status));
        }
    }
    printf("Todos los procesos hijos han terminado. Programa principal finalizado.\n");

}

/*
void printFiles(char* path, int tabs){
    struct stat stat1;
    struct dirent* dirent1;

    if(tabs>=2)
        return;

    if (stat(path, &stat1)==-1)
        return;

    if ((stat1.st_mode & S_IFMT)==S_IFDIR){
        DIR* aux= opendir(path);
        while((dirent1=readdir(aux))!=NULL){
            if ((dirent1)->d_name[0]!='.'){
                char buff[256];
                sprintf(buff,"%s/%s",path,dirent1->d_name);
                printFiles(buff, tabs+1);   // reutilizamos el codigo hecho en el ejer 1 de l guia 0
            }
        }
        //chdir("../");
        closedir(aux);
    }else     {
        //printf("%s\n",path);
        callSlave(path);
        return;
    }
}
*/

/*void callSlave (char * archivo){
    pid_t child_pid;
    child_pid = fork();

    if(child_pid == -1){
        perror("fork");
        exit (1);
    }else if (child_pid ==0){
        char * args[]={"./child",archivo,NULL}; // siempre al final de estos arrays tiene que haber un null
        execve (args[0],args,NULL); // siempre al final de estos arrays tiene que haber un null
        perror("execve");
        exit(1);
    }
}
*/


//void forkAndExecuteSlave (pid_t *slave){}