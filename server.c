#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PORT 8080
#define BUFFER 1024

const char USER[] = "admin";
const char PASS[] = "1234";

void handleClient(int clientSock)
{
    char buffer[BUFFER];
    char username[50], password[50];

    memset(buffer,0,sizeof(buffer));

    // Receive login details
    int bytes = recv(clientSock, buffer, sizeof(buffer)-1, 0);

    if(bytes <= 0)
    {
        printf("Client disconnected.\n");
        close(clientSock);
        exit(0);
    }

    buffer[bytes]='\0';

    // Expected format:
    // username password

    if(sscanf(buffer,"%49s %49s",username,password)!=2)
    {
        send(clientSock,"INVALID FORMAT",14,0);
        close(clientSock);
        exit(0);
    }

    if(strcmp(username,USER)!=0 || strcmp(password,PASS)!=0)
    {
        send(clientSock,"AUTH FAILED",11,0);
        close(clientSock);
        exit(0);
    }

    send(clientSock,"AUTH SUCCESS",12,0);

    while(1)
    {
        memset(buffer,0,sizeof(buffer));

        bytes=recv(clientSock,buffer,sizeof(buffer)-1,0);

        if(bytes<=0)
        {
            printf("Client disconnected.\n");
            break;
        }

        buffer[bytes]='\0';

        // Input validation
        if(strlen(buffer)==0)
        {
            send(clientSock,"Empty message",13,0);
            continue;
        }

        if(strcmp(buffer,"exit")==0)
        {
            send(clientSock,"Goodbye",7,0);
            break;
        }

        printf("\nClient : %s\n",buffer);

        char reply[BUFFER];

        sprintf(reply,"Server received: %s",buffer);

        send(clientSock,reply,strlen(reply),0);
    }

    close(clientSock);

    exit(0);
}
