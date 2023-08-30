// este va a ser el archivo padre
// juega de 5 (Enzo Fernandez), distribuye entre los hijo

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PROCESSES 2

void callSlave (char * archivo);

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Ingrese un argumento\n");
        //return 1;
    }

    for(int i=0; i<PROCESSES; i++){
        char * archivo;
        if(i%2==0)
            archivo="README.md";
        else
            archivo="child.c";
        callSlave(archivo);
    }

    for (int i = 0; i < PROCESSES; i++) {
        int status;
        waitpid(-1, &status, 0); // Esperar a que cualquier proceso hijo termine
        if (WIFEXITED(status)) {
            printf("Proceso hijo terminado con estado %d.\n", WEXITSTATUS(status));
        }
    }


    printf("Todos los procesos hijos han terminado. Programa principal finalizado.\n");

}

void callSlave (char * archivo){
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