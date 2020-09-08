#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "list.h"
#include "output.h"

static pthread_t threadPID;
static List* rList;
static char* buffer;
static int resultSize;
bool progOutputEnd = false;
static int complexTestFreeCounter = 0;

static pthread_cond_t receiveListCondVar = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t receiveListMutex = PTHREAD_MUTEX_INITIALIZER;

static List* rList;

void* outputThread(void* unused){
    while(1){
        pthread_mutex_lock(&receiveListMutex);
        {
            pthread_cond_wait(&receiveListCondVar, &receiveListMutex);
            if (List_count(rList) > 0){
                buffer = List_first(rList);
                List_remove(rList);
            }
            if (List_count(rList) > 100){
                printf("Exceeded number of maximum nodes\n");
            }
        }pthread_mutex_unlock(&receiveListMutex);
        char* messageR = "Message Received: ";
        write(1, messageR, strlen(messageR));   
        resultSize = write(1, buffer, strlen(buffer));
        if (strcmp(buffer,"!\n") == 0){
            progOutputEnd = true;
        }
    }
    return NULL;
}

void Output_init(){
    List* receiveList = List_create();
    rList = receiveList;

    pthread_create(
        &threadPID,         // PID (by pointer)
        NULL,               // Attributes
        outputThread,      // Function
        NULL);
}

bool Output_end(){
    return progOutputEnd;
}

void Output_signal(char* messageRx){
    pthread_mutex_lock(&receiveListMutex);
    {
        List_append(rList, messageRx);
        pthread_cond_signal(&receiveListCondVar);  
    }
    pthread_mutex_unlock(&receiveListMutex);
}

static void complexTestFreeFn(void* pItem) 
{
    complexTestFreeCounter++;
}

void Output_shutdown(void)
{
    List_free(rList, complexTestFreeFn);
    // Cancel thread
    pthread_cancel(threadPID);

    //freeing mutex and destroying cond variable
    pthread_mutex_destroy(&receiveListMutex);
    pthread_cond_destroy(&receiveListCondVar);

    // Waits for thread to finish
    pthread_join(threadPID, NULL);

}