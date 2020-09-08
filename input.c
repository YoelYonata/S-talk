#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "list.h"
#include "transmitter.h"
#include "input.h"

#define MSG_MAX_LEN 1024

static pthread_t threadPID;
static char *buffer;
int resultsize;
bool progInputEnd = false;

static pthread_cond_t freeCondVar = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t freeMutex = PTHREAD_MUTEX_INITIALIZER;

void* inputThread(void* unused){
    while(1){
        buffer = malloc(MSG_MAX_LEN);
        if(fgets(buffer, MSG_MAX_LEN, stdin)){
            Transmitter_signal(buffer);
            if (strcmp(buffer,"!\n") == 0){
                progInputEnd = true;
            }
        }
        pthread_cond_wait(&freeCondVar, &freeMutex);

    }
    return NULL;
}

void signaltoFree(){
    pthread_mutex_lock(&freeMutex);
    {
        free(buffer);
        pthread_cond_signal(&freeCondVar);
    }
    pthread_mutex_unlock(&freeMutex);
}

bool Input_end(){
    return progInputEnd;
}

void Input_init(){
    pthread_create(
        &threadPID,         // PID (by pointer)
        NULL,               // Attributes
        inputThread,      // Function
        NULL);
}

void Input_shutdown(void)
{
    // Cancel thread
    pthread_cancel(threadPID);

    //freeing mutex and destroying cond variable
    pthread_mutex_destroy(&freeMutex);
    pthread_cond_destroy(&freeCondVar);

    // Waits for thread to finish
    pthread_join(threadPID, NULL);

    //cleanup memory
    free(buffer);
    buffer = NULL;

}
