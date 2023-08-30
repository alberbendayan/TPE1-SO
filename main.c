// este va a ser el archivo padre
// juega de 5 (Enzo Fernandez), distribuye entre los hijo

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PROCESSES 5

int main(int argc, char *argv[]) {
    pid_t child_pid;

    if (argc < 2) {
        printf("Muy pocos argumentos, ingrese un argumento mas\n");
        //return 1;
    }

    for(int i=0; i<PROCESSES; i++){
        child_pid = fork();

        if(child_pid == -1){
            perror("fork");
            exit (1);
        }else if (child_pid ==0){
            char * args[]={"./child","README.md",NULL}; // siempre al final de estos arrays tiene que haber un null
            execve (args[0],args,NULL); // siempre al final de estos arrays tiene que haber un null
            perror("execve");
            exit(1);
        }
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
