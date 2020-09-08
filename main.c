#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include "receiver.h"
#include "transmitter.h"
#include "output.h"
#include "input.h"
#include "list.h"

int main(int argc, char** args)
{
    printf("Welcome to s-talk\n");

    int port = atoi(args[1]);
    char* remoteMachine = args[2];
    char* remotePort = args[3];

    // Address
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;                   // Connection may be from network
	sin.sin_addr.s_addr = htonl(INADDR_ANY);    // Host to Network long
	sin.sin_port = htons(port);                 // Host to Network short
	
	// Create the socket for UDP
	int socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);

    if (socketDescriptor == -1){
        printf("ERROR: Invalid socket descriptor\n");
    }

	// Bind the socket to the port (PORT) that we specify
	if (bind (socketDescriptor, (struct sockaddr*) &sin, sizeof(sin)) == -1){
        printf("ERROR: socket bind error\n");
    }

    // Startup modules
    Input_init();
    Receiver_init(socketDescriptor);
    Transmitter_init(remoteMachine, remotePort, socketDescriptor);
    Output_init();

    printf("Enter ! to end the s-talk session\n");
    while(!(Input_end() || Output_end())){
    }

    // Shutdown modules
    Receiver_shutdown();
    Transmitter_shutdown();
    Output_shutdown();
    Input_shutdown();

    if (close(socketDescriptor) == -1){
        printf("ERROR: socket close error\n");
    }

    printf("done\n");
    return 0;
}