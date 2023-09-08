#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/select.h>

//#define SLAVES 2
#define BUFFERSIZE 512
#define INICIALARGS 2

int cantSlaves,iArgs=1;
struct timeval timeout;



void callSlave (char * archivo);
void printFiles(char* path,int tabs);

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Ingrese un argumento\n");
        return 1;
        //return 1;
    }

    // Un esclavo cada 10 archivos y como maximo 10 esclavos
    cantSlaves = argc/5 + 1;
    if (cantSlaves >10)
        cantSlaves=10;


    pid_t slaves[cantSlaves];
     // Arrays de pipes para la comunicación entre slaves y padre
    int pipesToSlave[cantSlaves][2];
    int pipesFromSlave[cantSlaves][2];
    int filesInSlave[cantSlaves];
    
    

    for (int i=0; i<cantSlaves; i++){
        // este for crea los pipes
        if (pipe(pipesToSlave[i]) == -1 || pipe(pipesFromSlave[i]) == -1) {
            perror("Error al crear el pipe");
            exit(1);
        }
       
        slaves[i] = fork();
        if(slaves[i]==-1){
            
            perror("Error al crear el proceso");
            exit(1);
        }
        
        if(slaves[i]==0){ // proceso hijo
            //cierro los estandar y los que no uso
            
            if (close(0) == -1) {
                perror("No se pudo cerrar el file descriptor");
                 return 1;
            } 
            if (close(1) == -1) {
                perror("No se pudo cerrar el file descriptor");
                return 1;
            } 
            if (close(pipesToSlave[i][1]) == -1) {
                perror("No se pudo cerrar el file descriptor");
                 return 1;
            } 
            if (close(pipesFromSlave[i][0]) == -1) {
                perror("No se pudo cerrar el file descriptor");
                return 1;
            } 
            //reasigno los fd
            dup(pipesToSlave[i][0]);
            dup(pipesFromSlave[i][1]);
            //cierro los fd repetidos
            if (close(pipesToSlave[i][0]) == -1) {
                perror("No se pudo cerrar el file descriptor");
                 return 1;
            } 
            if (close(pipesFromSlave[i][1]) == -1) {
                perror("No se pudo cerrar el file descriptor");
                return 1;
            }            

            
            // En lugar de mandar una parte de los archivos por aca y otra por pipes
            // como equipo definimos enviar todos los archivos por pipes

            char *argv1[] = {"./child", NULL}; // le puse argv1 para q sea diferente al argv
            execve("./child", argv1, NULL);
            perror("execve"); // Esto solo se ejecuta si execve falla
            exit(1);

        }
        else{//proceso padre
            //cierro los estandar y los que no uso
            
            close(pipesToSlave[i][0]);
            close(pipesFromSlave[i][1]);
            
            for(int k=0;k<INICIALARGS;k++){
                
                if(iArgs >= argc){ // es muy improbable que se llegue a este caso
                    k=INICIALARGS; // para salir del for interno
                }else{
                    write(pipesToSlave[i][1],argv[iArgs],strlen(argv[iArgs]));
                    filesInSlave[i]++;
                    iArgs++;
                }
            }
        }
    }


    // Configurar los descriptores de archivo para select
    fd_set read_fds;
    FD_ZERO(&read_fds);
    int max_fd = -1;

    for (int i = 0; i < cantSlaves; i++) {
        FD_SET(pipesFromSlave[i][0], &read_fds);
        if (pipesFromSlave[i][0] > max_fd) {
            max_fd = pipesFromSlave[i][0];
        }
    }
    while(1){
        fd_set tmp_fds = read_fds;
        int ready = select(max_fd + 1, &tmp_fds, NULL, NULL, NULL);
        if (ready == -1) {
            perror("Error en select");
            exit(EXIT_FAILURE);
        }
         for (int i = 0; i < cantSlaves; i++) {
            if (FD_ISSET(pipesFromSlave[i][0], &tmp_fds)) {
                // Leer datos del descriptor de archivo pipes[i][0] y procesarlos
                char buffer[BUFFERSIZE];
                ssize_t bytes_read = read(pipesFromSlave[i][0], buffer, sizeof(buffer));
                if (bytes_read > 0) {
                    // ESCRIBIR EN LA SHARE MEMORY LO DEL BUFFER
                    int hashlength=strlen(buffer);
                
                    write(1,buffer,hashlength);
                    for(int h=0;h<hashlength;h++){
                        buffer[h]=0;
                    }
                } else if (bytes_read != 0) {
                    perror("Error al leer del pipe");
                }
            }
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
        printf("Esperando que terminen los pibes\n");
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