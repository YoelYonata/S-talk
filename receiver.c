#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>			// for strncmp()
#include <unistd.h>			// for close()
#include <pthread.h>
#include <signal.h>

#include "receiver.h"
#include "output.h"
#include "list.h"

#define MSG_MAX_LEN 1024

static pthread_t threadPID;
static List* rList;
static int socketDescriptor;

void* receiveThread(void* unused)
{
	while (1) {
		// Get the data (blocking)
		// Will change sin (the address) to be the address of the client.
		// Note: sin passes information in and out of call!
		struct sockaddr_in sinRemote;
		unsigned int sin_len = sizeof(sinRemote);
		char messageRx[MSG_MAX_LEN];
		int bytesRx = recvfrom(socketDescriptor,
			messageRx, MSG_MAX_LEN, 0,
			(struct sockaddr *) &sinRemote, &sin_len);

        if(strcmp(messageRx, "!") == 0) {
            printf("End now");
        }
        int terminateIndex = (bytesRx < MSG_MAX_LEN) ? bytesRx : MSG_MAX_LEN-1;

        messageRx[terminateIndex] = 0;

        // Do something amazing with the received message!
        Output_signal(messageRx);
	}
    // NOTE NEVER EXECUTES BECEAUSE THREAD IS CANCELLED
	return NULL;
}

void Receiver_init(int socketDes)
{
    List* receiveList = List_create();
    rList = receiveList;
    socketDescriptor = socketDes;

    pthread_create(
        &threadPID,         // PID (by pointer)
        NULL,               // Attributes
        receiveThread,      // Function
        NULL);
}



void Receiver_shutdown(void)
{
    // Cancel thread
    pthread_cancel(threadPID);

    // Waits for thread to finish
    pthread_join(threadPID, NULL);

    // Cleanup memory
}


