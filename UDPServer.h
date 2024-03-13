#ifndef _UDP_SERVER_H
#define _UDP_SERVER_H

#include "list.h"

void* listenForMessages();
void initUDPServer(char* myPort, List* list);
void cancelUDPServer();
void closeUDPServer();
char *addHeader(char messageBuffer[], int numbytes);

#endif