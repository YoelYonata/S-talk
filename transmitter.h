#include "list.h"

#ifndef _TRANSMITTER_H_
#define _TRANSMITTER_H_

void Transmitter_init(char* remoteMachine, char* remotePort, int socketDes);

void Transmitter_shutdown();

void Transmitter_signal(char* messageTx);

#endif