// References: 
// Beej's Guide to Network Programming - 6.3 Datagram Sockets
// Brian Fraser's Youtube tutorials for pthreads (1 -3)

// INPUT READER
// runs keyboardThread
// awaits keyboard input and adds message to inputList

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "list.h"
#include "threadManager.h"
#include "inputReader.h"
#include "outputWriter.h"
#include "UDPClient.h"
#include "UDPServer.h"

// limit for UDP datagram under IPv4 is 65507
#define MAX_LEN_BUFFER 65491  // 65507 - 15 (for header) - 1 (for '\0')

static List* inputList;
static pthread_t keyboardThread;

void* readKeyboardInput() {
    while (1) {
        char *message;
        char messageBuffer[MAX_LEN_BUFFER]; 
        int numbytes;

        // run once before checking while condition
        do {
            // clear the messageBuffer to store input
            memset(&messageBuffer, 0, MAX_LEN_BUFFER);

            // store user input
            numbytes = read(0,messageBuffer, MAX_LEN_BUFFER);

            if(numbytes == -1) {
                perror("inputReader: failed to read keyboard input\n");
                exit(-1);
            }

            // store messsage
            message = (char *)malloc(sizeof(char) * (numbytes + 1));
            strncpy(message, messageBuffer, numbytes);
            message[numbytes] = '\0';

            // add message to inputList
            int res = addMessage(inputList, message);

            if(res == LIST_FAIL) {
                fprintf(stderr,"inputReader: failed to add message to inputList\n");
            }

            // stop reading if user enters "!\n"
            if (!strcmp(message, "!\n")) {
                signalUDPClient(); 
                cancelOutputWriter(); 
                cancelUDPServer();
                return NULL;
            }

            // if the user presses enter (adds '\n' to end of message), jump out of loop and stop reading input
        } while (messageBuffer[numbytes - 1] != '\n'); 

        // signal UDPClient to send the message
        signalUDPClient();
    }

    return NULL;
}

void initInputReader(List* list) {
    inputList = list;

    // create the keyboardThread - does nothing other than await input from the keyboard
    int res =  pthread_create(&keyboardThread, NULL, readKeyboardInput, NULL);
    
    if(res !=0) {
        perror("inputReader: thread creation error");
        exit(-1);
    }
}

void cancelInputReader() {
    // cancel the keyboard thread
    int res = pthread_cancel(keyboardThread);
    if (res != 0) {
        perror("inputReader: thread could not be cancelled\n");
        exit(-1);
    }
}

void closeInputReader() {
    // join (wait for and detach) keyboardThread
    int res = pthread_join(keyboardThread, NULL);
    if (res != 0) {
        perror("inputReader: thread could not be joined\n");
        exit(-1);
    }
}
