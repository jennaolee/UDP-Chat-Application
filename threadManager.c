#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "threadManager.h"
#include"list.h"

// listMutex = mutex that handles shared list access
static pthread_mutex_t listMutex = PTHREAD_MUTEX_INITIALIZER;

// writeMessageMutex = mutex that handles writing access
static pthread_mutex_t writeMessageMutex = PTHREAD_MUTEX_INITIALIZER;

// writeMessageFlag = condition variable that manages thread synchonization for writing messages
static pthread_cond_t writeMessageFlag = PTHREAD_COND_INITIALIZER;

// sendMessageMutex = mutex that handles sending access
static pthread_mutex_t sendMessageMutex = PTHREAD_MUTEX_INITIALIZER;

// sendMessageFlag = condition variable that manages thread synchonization for sending messages
static pthread_cond_t sendMessageFlag = PTHREAD_COND_INITIALIZER;

int addMessage(List* list, char* message) {
    int success;

    pthread_mutex_lock(&listMutex);
    success = List_prepend(list, message); // critical section - list access
    pthread_mutex_unlock(&listMutex);

    return success;
}
char* getMessage(List* list) {
    char* message;

    pthread_mutex_lock(&listMutex); 
    message = List_trim(list); // critical section - list access
    pthread_mutex_unlock(&listMutex);

    return message;
}

int countList(List* list) {
    int count;

    pthread_mutex_lock(&listMutex);
    count = List_count(list); // critical section - list access
    pthread_mutex_unlock(&listMutex);

    return count;
}

// outputWriter Mutexes
void signalOutputWriter() {
    pthread_mutex_lock(&writeMessageMutex);
    pthread_cond_signal(&writeMessageFlag); // critical section - signal outputWriter to write messages
    pthread_mutex_unlock(&writeMessageMutex);
}

void waitOutputWriter() {
    pthread_mutex_lock(&writeMessageMutex);
    pthread_cond_wait(&writeMessageFlag, &writeMessageMutex); // critical section - wait outputWriter until messages are available to write
    pthread_mutex_unlock(&writeMessageMutex);
}

// UDPClient Mutexes 
void signalUDPClient(){
    pthread_mutex_lock(&sendMessageMutex);
    pthread_cond_signal(&sendMessageFlag); // critical section - signal UDPClient to send messages
    pthread_mutex_unlock(&sendMessageMutex);
}

void waitUDPClient() {
    pthread_mutex_lock(&sendMessageMutex);
    pthread_cond_wait(&sendMessageFlag, &sendMessageMutex); // critical section - wait UDPClient until messages are available to send
    pthread_mutex_unlock(&sendMessageMutex);
}

// start up: create the condition variables
void initMutexes() {
    pthread_mutex_init(&listMutex, NULL);
    pthread_mutex_init(&writeMessageMutex, NULL);
    pthread_mutex_init(&sendMessageMutex, NULL);
}

// clean up: destroy mutexes before ending program
void destroyMutexes() {
    pthread_mutex_destroy(&listMutex);
    pthread_mutex_destroy(&writeMessageMutex);
    pthread_mutex_destroy(&sendMessageMutex);
}

// start up: create the condition variables
void initConditionVars() {
    pthread_cond_init(&writeMessageFlag, NULL);
    pthread_cond_init(&sendMessageFlag, NULL);
}

// clean up: destroy condition variables before ending program
void destroyConditionVars() {
    pthread_cond_destroy(&writeMessageFlag);
    pthread_cond_destroy(&sendMessageFlag);
}