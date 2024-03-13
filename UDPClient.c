// References: 
// Beej's Guide to Network Programming - 6.3 Datagram Sockets
// Brian Fraser's Youtube tutorials for pthreads (1 -3)

// UDP CLIENT
// runs senderThread
// get message from inputList and send message over network

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
 
#include "list.h"
#include "threadManager.h"
#include "UDPClient.h"
#include "freeManager.h"
 
static int sockfd;
static struct addrinfo *servinfo;
static char *remoteHostName, *remotePortNumber, *message;
static List* inputList;
static pthread_t senderThread;
 
void *sendMessages() {
    struct addrinfo hints, *p;
    int gaiVal, numbytes;

    // clear hints to store values
    memset(&hints, 0 ,sizeof(hints));
    hints.ai_family = AF_INET; // using IPv4
    hints.ai_socktype = SOCK_DGRAM;

    // get linked list of addrinfo structures and store results in servinfo
    gaiVal = getaddrinfo(remoteHostName, remotePortNumber, &hints, &servinfo);

    if (gaiVal != 0 ) {
        fprintf(stderr, "UDPClient: getaddrinfo error: %s\n", gai_strerror(gaiVal));
        exit(-1);
    }
    
    // iterate over the results until a socket is successfully created
    for(p = servinfo; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    
        if (sockfd ==-1) {
            perror("UDPClient: socket() error");
            continue;
        }
        break;
    }

    // exit program if socket could not be created
    if (p == NULL) {
        fprintf(stderr, "UDPClient: failed to create socket");
        exit(-1);
    }
    
    while (1) {
        // wait for signal that messages are available to be sent over the network
        waitUDPClient();

        do {
            // get message from the inputList to send
            message = getMessage(inputList);

            if (message == NULL) {
                fprintf(stderr, "UDPClient: failed to get message, message is NULL\n");
                break;
            }

            // send the message
            numbytes = sendto(sockfd, message, strlen(message), 0, p->ai_addr, p->ai_addrlen);
            if (numbytes == -1) {
                perror("UDPClient: sendto() error\n");
                exit(-1);
            }
            
            // if user enters "!\n", free message and stop sending messages
            if (!strcmp(message,"!\n")) {
                freeMessage(message);
                return NULL;
            }
            
            // else free message and continue sending
            freeMessage(message);

            // continue sending messages if there are still messages in the list
        } while (countList(inputList) != 0);
    }

    return NULL;
}

void initUDPClient(char* remoteName, char* remotePort, List* list) {
    remoteHostName=remoteName;
    remotePortNumber = remotePort;
    inputList = list;
    
    // create senderThread - sends data to the remote UNIX process over the network using UDP
    int res = pthread_create(&senderThread, NULL, sendMessages, NULL);
    if(res != 0) {
        perror("UDPClient: senderThread could not be created\n");
        exit(-1);
    }
}

void cancelUDPClient() {
    int res = pthread_cancel(senderThread);
    if (res != 0) {
        perror("UDPClient: thread could not be cancelled\n");
        exit(-1);
    }
}
 
void closeUDPClient() {
    // free the linked list of results
    freeaddrinfo(servinfo);

    // close the socket
    close(sockfd);

    // join (wait for and detach) senderThread
    int res = pthread_join(senderThread, NULL); 
    if (res != 0) {
        perror("UDPClient: thread could not be joined\n");
        exit(-1);
    }
}

