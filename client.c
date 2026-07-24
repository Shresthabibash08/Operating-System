#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define BUF 512          // Buffer size for messages

int sock;                // Global socket descriptor

// Continuously receives messages from the server
void *receiveLoop(void *arg) {

    (void)arg;

    char buf[BUF];

    while (1) {

        int n = recv(sock, buf, BUF - 1, 0);

        if (n <= 0) {
            printf("\n[!] Server disconnected.\n");
            exit(0);
        }

        buf[n] = '\0';

        // Display received message
        printf("\r%s> ", buf);

        fflush(stdout);
    }

    return NULL;
}

int main(int argc, char *argv[]) {

    // Check command-line arguments
    if (argc != 3) {

        printf("Usage: %s <server_ip> <port>\n", argv[0]);

        return 1;
    }

    // Create client socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {

        perror("socket");

        return 1;
    }

    // Configure server address
    struct sockaddr_in servAddr = {0};

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(atoi(argv[2]));

    // Convert IP address to binary form
    if (inet_pton(AF_INET, argv[1], &servAddr.sin_addr) <= 0) {

        printf("Invalid server address.\n");

        return 1;
    }

    // Connect to the server
    if (connect(sock,
               (struct sockaddr *)&servAddr,
               sizeof(servAddr)) < 0) {

        perror("connect");

        return 1;
    }

    printf("Connected to server %s:%s\n", argv[1], argv[2]);

    char buf[BUF], user[50], pass[50];

    // ---------- Authentication ----------
    while (1) {

        printf("Username: ");
        scanf("%49s", user);

        printf("Password: ");
        scanf("%49s", pass);

        // Send login credentials
        snprintf(buf, BUF, "AUTH %s %s", user, pass);

        send(sock, buf, strlen(buf), 0);

        // Receive authentication result
        int n = recv(sock, buf, BUF - 1, 0);

        if (n <= 0) {

            printf("Server closed connection.\n");

            close(sock);

            return 1;
        }

        buf[n] = '\0';

        if (strncmp(buf, "OK", 2) == 0) {

            printf("Login successful!\n");

            break;
        }

        printf("Authentication failed. Try again.\n");
    }

    // Clear input buffer
    getchar();

    // Create thread to receive server messages
    pthread_t tid;

    pthread_create(&tid, NULL, receiveLoop, NULL);

    pthread_detach(tid);

    printf("\nType messages to send (type QUIT to exit):\n> ");

    while (1) {

        fgets(buf, BUF, stdin);

        // Remove newline character
        buf[strcspn(buf, "\n")] = '\0';

        // Ignore empty messages
        if (strlen(buf) == 0) {

            printf("> ");

            continue;
        }

        char msg[BUF + 10];

        // Exit request
        if (strcmp(buf, "QUIT") == 0) {

            snprintf(msg, sizeof(msg), "QUIT");

            send(sock, msg, strlen(msg), 0);

            break;
        }

        // Send chat message to server
        snprintf(msg, sizeof(msg), "CHAT %s", buf);

        send(sock, msg, strlen(msg), 0);

        printf("> ");
    }

    // Close socket before exiting
    close(sock);

    return 0;
}
