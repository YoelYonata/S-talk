#include "list.h"

#ifndef _INLIST_H_
#define _INLIST_H_

// Start background receive thread
void Input_init();

// Stop background receive thread and cleanup
bool Input_end();

void Input_shutdown(void);

void signaltoFree();

#endif