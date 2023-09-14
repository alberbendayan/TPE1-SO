#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shared_memory.h"

#define IPC_RESULT_ERROR (-1)

static int get_shared_block(char *filename, int size)
{
    key_t key;

    // La key esta linkeada al nombre asi otros programas
    // pueden acceder tmb

    key = ftok(filename, 0);

    if (key == IPC_RESULT_ERROR)
    {
        return IPC_RESULT_ERROR;
    }

    // Devuelve un shm y si no existe la crea
    return shmget(key, size, 0644 | IPC_CREAT);
}

char *attach_memory_block(char *filename, int size)
{
    int shared_block_id = get_shared_block(filename, size);

    if (shared_block_id == IPC_RESULT_ERROR)
    {
        return NULL;
    }

    char *result;
    result = shmat(shared_block_id, NULL, 0);
    if (result == (char *)IPC_RESULT_ERROR)
    {
        return NULL;
    }

    return result;
}

bool detach_memory_block(char *block)
{
    return (shmdt(block) != IPC_RESULT_ERROR);
}

bool destroy_memory_block(char *filename)
{
    int shared_block_id = get_shared_block(filename, 0);

<<<<<<< Updated upstream
    if (shared_block_id == IPC_RESULT_ERROR)
    {
=======
    if(shared_block_id == IPC_RESULT_ERROR){
>>>>>>> Stashed changes
        return false;
    }

    return (shmctl(shared_block_id, IPC_RMID, NULL) != IPC_RESULT_ERROR);
}