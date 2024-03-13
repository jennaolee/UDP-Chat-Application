#ifndef _THREAD_MANAGER_H
#define _THREAD_MANAGER_H

#include "list.h"

int addMessage(List* list, char* message);
char* getMessage(List* list);
int countList(List* list);

void signalOutputWriter();
void waitOutputWriter();

void signalUDPClient();
void waitUDPClient();

void initMutexes();
void destroyMutexes();

void initConditionVars();
void destroyConditionVars();

#endif