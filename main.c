#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "inputReader.h"
#include "outputWriter.h"
#include "UDPServer.h"
#include "UDPClient.h"
#include "freeManager.h"
#include "threadManager.h"

int main (int argc, char * argv[]) {
    // check to make sure all arguments are given
    if (argc != 4) {
        printf("Please enter the arguments: ./s-talk [my port number] [remote machine name] [remote port number]\n");
        return -1;
    }

    // store the arguments
    char* localPort = argv[1];
    char* remoteHostname = argv[2];
    char* remotePort = argv[3];

    // create the shared lists
    List *inputList = List_create(); // this list stores the messages to be sent
    List *outputList = List_create(); // this list stores the messages to be displayed

    // init pthreads: mutexes and condition variables
    initMutexes();
    initConditionVars();

    // init processes
    initInputReader(inputList);
    initUDPClient(remoteHostname, remotePort, inputList);
    initUDPServer(localPort, outputList);
    initOutputWriter(outputList);

    // close processes 
    closeInputReader();
    closeUDPClient();
    closeUDPServer();
    closeOutputWriter();

    // destroy pthreads: mutexes and condition variables
    destroyMutexes();
    destroyConditionVars();

    printf("Session was ended\n");

    return 0;
}