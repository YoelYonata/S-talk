#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>			// for strncmp()
#include <unistd.h>			// for close()
#include <pthread.h>
#include <signal.h>
#include "input.h"
#include "transmitter.h"
#include "list.h"

static pthread_t threadPID;
static int socketDescriptor = 0;
static char* remoteMachine;
static char* remotePort;
static List* tList;
static int complexTestFreeCounter = 0;

struct addrinfo* results;

static pthread_cond_t sendListCondVar = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t sendListMutex = PTHREAD_MUTEX_INITIALIZER;

void* transmitThread(void* unused) {
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    printf("Sending to remote machine %s and port %s\n", remoteMachine, remotePort);

    int s = getaddrinfo(remoteMachine, remotePort, &hints, &results);

    if(s != 0) {
        printf("ERROR: getaddrinfo error\n");
        exit(EXIT_FAILURE);
    }

    while(1) {
        static char* s_txMessage = "";
        pthread_mutex_lock(&sendListMutex);
        {
            pthread_cond_wait(&sendListCondVar, &sendListMutex);
            if (List_count(tList) > 0){
                s_txMessage = List_first(tList);
                List_remove(tList);
            }
            if (List_count(tList) > 100){
                printf("Exceeded number of maximum nodes\n");
            }
        }pthread_mutex_unlock(&sendListMutex);

        if(sendto(socketDescriptor, s_txMessage, strlen(s_txMessage), 0,
        results->ai_addr, results->ai_addrlen) == -1) {
            printf("Sending Error\n");
        }
        if (strcmp(s_txMessage, "!\n") != 0){
            signaltoFree();
        }
        
    }
    return NULL;
}

void Transmitter_init(char* remoteMach, char* remotePortNum, int socketDes) {
    remoteMachine = remoteMach;
    remotePort = remotePortNum;
    socketDescriptor = socketDes;
    List* transmitList = List_create();
    tList = transmitList;
    pthread_create(
        &threadPID,
        NULL,
        transmitThread,
        NULL);
}

void Transmitter_signal(char* messageTx){
    pthread_mutex_lock(&sendListMutex);
    {
        List_append(tList, messageTx);
        pthread_cond_signal(&sendListCondVar);
    }
    pthread_mutex_unlock(&sendListMutex);
    
}

static void complexTestFreeFn(void* pItem) 
{
    complexTestFreeCounter++;
}

void Transmitter_shutdown() {
    List_free(tList, complexTestFreeFn);
    // Cancel thread
    pthread_cancel(threadPID);

    //freeing mutex and destroying cond variable
    pthread_mutex_destroy(&sendListMutex);
    pthread_cond_destroy(&sendListCondVar);

    // Waits for thread to finish
    pthread_join(threadPID, NULL);

    // Cleanup memory
    freeaddrinfo(results);
}

