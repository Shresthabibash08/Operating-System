#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define BUF 512          // Buffer size for messages
#define MAXC 50          // Maximum number of connected clients
#define UNAME "admin"    // Server username
#define PASS  "admin123" // Server password

// Stores connected client sockets
int clients[MAXC], nclients = 0;

// Mutex to synchronize access to shared client list
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// Adds a new client to the client list
void addClient(int s) {
    pthread_mutex_lock(&lock);

    if (nclients < MAXC)
        clients[nclients++] = s;

    pthread_mutex_unlock(&lock);
}

// Removes a disconnected client from the client list
void removeClient(int s) {
    pthread_mutex_lock(&lock);

    for (int i = 0; i < nclients; i++)
        if (clients[i] == s) {
            clients[i] = clients[--nclients];
            break;
        }

    pthread_mutex_unlock(&lock);
}

// Sends a message to all connected clients except the sender
void broadcast(const char *msg, int excludeSock) {

    pthread_mutex_lock(&lock);

    for (int i = 0; i < nclients; i++)
        if (clients[i] != excludeSock)
            send(clients[i], msg, strlen(msg), 0);

    pthread_mutex_unlock(&lock);
}

// Handles communication with one client
void *handleClient(void *arg) {

    int sock = *(int *)arg;
    free(arg);

    char buf[BUF], user[50], pass[50];
    int authenticated = 0;

    // Authenticate client
    while (!authenticated) {

        int n = recv(sock, buf, BUF - 1, 0);

        if (n <= 0) {
            close(sock);
            return NULL;
        }

        buf[n] = '\0';

        if (sscanf(buf, "AUTH %49s %49s", user, pass) == 2 &&
            strcmp(user, UNAME) == 0 &&
            strcmp(pass, PASS) == 0) {

            send(sock, "OK\n", 3, 0);
            authenticated = 1;

        } else {

            send(sock, "FAIL\n", 5, 0);
        }
    }

    addClient(sock);

    printf("[+] Client %d authenticated and connected.\n", sock);

    // Receive client messages
    while (1) {

        int n = recv(sock, buf, BUF - 1, 0);

        if (n <= 0) {
            printf("[-] Client %d disconnected.\n", sock);
            break;
        }

        buf[n] = '\0';

        // Remove newline
        buf[strcspn(buf, "\r\n")] = '\0';

        // Client exits
        if (strncmp(buf, "QUIT", 4) == 0) {

            send(sock, "BYE\n", 4, 0);
            break;

        }

        // Valid chat message
        else if (strncmp(buf, "CHAT ", 5) == 0 && strlen(buf) > 5) {

    char message[BUF];

    snprintf(message, sizeof(message),
             "Client %d: %s\n", sock, buf + 5);

    printf("%s", message);

    broadcast(message, sock);      // Broadcast to all other clients
}
        // Invalid command
        else {

            send(sock, "ERR: invalid or empty command\n", 31, 0);
        }
    }

    removeClient(sock);

    close(sock);

    return NULL;
}

// Allows the server operator to broadcast messages
void *serverInputLoop(void *arg) {

    (void)arg;

    char line[BUF];

    while (fgets(line, BUF, stdin)) {

        line[strcspn(line, "\n")] = '\0';

        if (strlen(line) == 0)
            continue;

        char msg[BUF + 20];

        snprintf(msg, sizeof(msg), "Server: %s\n", line);

        broadcast(msg, -1);
    }

    return NULL;
}

// Main function
int main(int argc, char *argv[]) {

    // Check command-line argument
    if (argc != 2) {

        printf("Usage: %s <port>\n", argv[0]);

        return 1;
    }

    int port = atoi(argv[1]);

    // Create server socket
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSock < 0) {

        perror("socket");

        return 1;
    }

    // Allow socket reuse
    int opt = 1;

    setsockopt(serverSock,
               SOL_SOCKET,
               SO_REUSEADDR,
               &opt,
               sizeof(opt));

    // Configure server address
    struct sockaddr_in addr = {0};

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    // Bind socket
    if (bind(serverSock,
            (struct sockaddr *)&addr,
            sizeof(addr)) < 0) {

        perror("bind");

        return 1;
    }

    // Start listening
    if (listen(serverSock, 10) < 0) {

        perror("listen");

        return 1;
    }

    printf("Server listening on port %d...\n", port);

    printf("Type a message anytime and press Enter to broadcast it to all clients.\n");

    // Thread for server broadcast messages
    pthread_t inputTid;

    pthread_create(&inputTid, NULL, serverInputLoop, NULL);

    pthread_detach(inputTid);

    // Accept client connections continuously
    while (1) {

        int *clientSock = malloc(sizeof(int));

        *clientSock = accept(serverSock, NULL, NULL);

        if (*clientSock < 0) {

            perror("accept");

            free(clientSock);

            continue;
        }

        pthread_t tid;

        // Create one thread per client
        pthread_create(&tid, NULL, handleClient, clientSock);

        pthread_detach(tid);
    }

    close(serverSock);

    return 0;
}
