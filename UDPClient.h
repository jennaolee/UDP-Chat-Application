#ifndef _UDP_CLIENT_H
#define _UDP_CLIENT_H

#include "list.h"

void *sendMessages();
void initUDPClient(char* remoteName, char* remotePort, List* list);
void signalUDPClient();
void cancelUDPClient();
void closeUDPClient();

#endif