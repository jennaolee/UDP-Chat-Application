// References: 
// Beej's Guide to Network Programming - 6.3 Datagram Sockets
// Brian Fraser's Youtube tutorials for pthreads (1 -3)

// OUTPUT WRITER
// runs writerThread
// get message from outputList and print on screen

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "list.h"
#include "threadManager.h"
#include "outputWriter.h"
#include "freeManager.h"

// limit for UDP datagram under IPv4 is 65507
#define MAX_LEN_BUFFER 65491  // 65507 - 15 (for header) - 1 (for '\0')

static List* outputList;
static char* message;
static pthread_t writerThread;

void* writeMessages() {
    while (1) {
        // wait for messages to print
        waitOutputWriter();
        
        do {
            // get message from outputList
            message = getMessage(outputList);

            if(message == NULL) {
                fprintf(stderr, "outputWriter: failed to get message, message is NULL\n");
                break;
            }

            // write/print message to screen
            int res = write(1, message, strlen(message)); 
            if(res == -1) {
                perror("outputWriter: failed to print message\n");
                exit(-1);
            }

            // if message is "!\n" (local) or Remote Client: !\n" (remote) then stop the writing
            if(!strcmp(message, "!\n") || !strcmp(message, "Remote Client: !\n")) {
                // free message and stop writing
                freeMessage(message);
                return NULL;
            }

            // else continue writing and free message
            freeMessage(message);

            // continue writing if there are still messages in the outputList
        } while (countList(outputList) != 0);
    }

    return NULL;
}

void initOutputWriter(List* list) {
    outputList = list;

    // create writerThread - prints character to the screen
    int res =  pthread_create(&writerThread, NULL, writeMessages, NULL);
    if(res != 0){
        perror("write thread failed");
        exit(-1);
    }
}

void cancelOutputWriter() {
    int res = pthread_cancel(writerThread);
    if (res != 0) {
        perror("outputWriter: thread could not be cancelled\n");
        exit(-1);
    }
}

void closeOutputWriter() {
    // join (wait for and detach) writerThread
    int res = pthread_join(writerThread, NULL);
    if (res != 0) {
        perror("outputWriter: thread could not be joined\n");
        exit(-1);
    }
}

