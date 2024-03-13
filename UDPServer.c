// References: 
// Beej's Guide to Network Programming - 6.3 Datagram Sockets
// Brian Fraser's Youtube tutorials for pthreads (1 -3)

// UDP SERVER
// runs listenerThread
// await UDP datagram and add message to outputList

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
#include "outputWriter.h"
#include "UDPServer.h"
#include "inputReader.h"
#include "UDPClient.h"
 
// limit for UDP datagram under IPv4 is 65507
#define MAX_LEN_BUFFER 65491  // 65507 - 15 (for header) - 1 (for '\0')

static int sockfd;
static struct addrinfo *servinfo;
static char* myPortNumber;
static List* outputList;
static pthread_t listenerThread;

void* listenForMessages() {
    int sockfd, gaiVal, bindVal, numbytes;
    struct addrinfo hints, *servinfo, *p;
    char messageBuffer[MAX_LEN_BUFFER];
    char* message;
    struct sockaddr_in remoteAddr;
    socklen_t remoteAddrLen;

    // clear hints to store values
    memset(&hints, 0 ,sizeof (hints));
    hints.ai_family = AF_INET; // using IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // fills my IP for me

    // get linked list of addrinfo structures and store results in servinfo
    gaiVal = getaddrinfo(NULL, myPortNumber, &hints, &servinfo);

    if (gaiVal != 0 ) {
        fprintf(stderr, "UDPServer: getaddrinfo error: %s\n", gai_strerror(gaiVal));
        exit(-1);
    }

    // iterate over the results until a socket is successfully created
    for(p = servinfo; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

        if (sockfd ==-1) {
            perror("UDPServer: socket() error\n");
            continue;
        }

        // bind the socket
        bindVal = bind(sockfd, p->ai_addr, p->ai_addrlen);

        if(bindVal == -1) {
            // if the socket could not bind, then close the socket
            close(sockfd);
            perror("UDPServer bind() error\n");
            continue;
        }

        break;
    }

    // exit program if socket could not be created and bound
    if(p == NULL) {
        fprintf(stderr, "UDPServer: could not bind socket\n");
        exit(-1);
    }

    // free the linked list of results
    freeaddrinfo(servinfo);

    while (1) {
        do {
            // clear the messageBuffer to store the message
            memset(&messageBuffer, 0, MAX_LEN_BUFFER);

            // receive the message
            remoteAddrLen = sizeof(remoteAddr);
            numbytes = recvfrom(sockfd, messageBuffer, MAX_LEN_BUFFER, 0, (struct sockaddr* ) &remoteAddr, &remoteAddrLen);
            
            if(numbytes ==-1) {
                perror("UDPServer recvfrom error");
                exit(-1);
            }

            // add the message header and store the message
            message = addHeader(messageBuffer, numbytes);

            // add the message to the outputList
            int res = addMessage(outputList, message);
            if(res == -1) {
                fprintf(stderr,"UDPServer: could not add message to list\n");
            }

            // if the message is "!\n" (local) or "Remote Client: !\n" (remote), stop listening for messages
            if(!strcmp(message, "!\n") || !strcmp(message, "Remote Client: !\n")) {
                signalOutputWriter(); // outputWriter can write the message, then stop
                cancelInputReader();
                cancelUDPClient();
                return NULL;
            }

            // continue listening for messages if user has not pressed enter (added '\n' to end of message)
        } while (messageBuffer[numbytes - 1] != '\n');

        // once user enters, then signal outputWriter to print the message
        signalOutputWriter();
    }

    return NULL;
}

void initUDPServer(char* myPort, List* list) {
    myPortNumber = myPort;
    outputList = list;

    // create listenerThread - does nothing other than await a UDP datagram 
    int res = pthread_create(&listenerThread, NULL, listenForMessages, NULL);
    if(res != 0) {
        perror("UDPServer: listenerThread could not be created\n");
        exit(-1);
    }
}

void cancelUDPServer() {
    int res = pthread_cancel(listenerThread);
    if (res != 0) {
        perror("UDPServer: thread could not be cancelled\n");
        exit(-1);
    }
}

void closeUDPServer() {
    // free the linked list of results
    freeaddrinfo(servinfo);

    // close the socket
    close(sockfd);

    // join (wait for and detach) senderThread
    int res = pthread_join(listenerThread, NULL);
    if (res != 0) {
        perror("UDPServer: thread could not be joined\n");
        exit(-1);
    }
}

char *addHeader(char messageBuffer[], int numbytes) {
    // message header
    char *header = "Remote Client: ";

    // dynamically allocate memory to store the full message
    char *res = (char *)malloc(sizeof(char) * (numbytes + strlen(header) + 1));

    // copy the header to res (at start)
    memcpy(res, header, strlen(header));

    // copy the message to res (after the header)
    memcpy(res + strlen(header), messageBuffer, numbytes + 1);

    // add '\0' to end of res
    res[numbytes + strlen(header)] = '\0';

    return res;
}