#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define BUF 512
int sock;

/*  Continuously listens for messages from the server and prints them */
void *receiveLoop(void *arg) {
    (void)arg;
    char buf[BUF];
    while (1) {
        int n = recv(sock, buf, BUF - 1, 0);
        if (n <= 0) { printf("\n[!] Server disconnected.\n"); exit(0); }
        buf[n] = '\0';
        printf("\r%s> ", buf);   /* incoming message, then re-show prompt */
        fflush(stdout);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) { printf("Usage: %s <server_ip> <port>\n", argv[0]); return 1; }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); return 1; }

    struct sockaddr_in servAddr = {0};
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &servAddr.sin_addr) <= 0) {
        printf("Invalid server address.\n"); return 1;
    }
    if (connect(sock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
        perror("connect"); return 1;
    }
    printf("Connected to server %s:%s\n", argv[1], argv[2]);

    char buf[BUF], user[50], pass[50];

    /* ---- Authentication phase ---- */
    while (1) {
        printf("Username: "); scanf("%49s", user);
        printf("Password: "); scanf("%49s", pass);
        snprintf(buf, BUF, "AUTH %s %s", user, pass);
        send(sock, buf, strlen(buf), 0);

        int n = recv(sock, buf, BUF - 1, 0);
        if (n <= 0) { printf("Server closed connection.\n"); close(sock); return 1; }
        buf[n] = '\0';
        if (strncmp(buf, "OK", 2) == 0) { printf("Login successful!\n"); break; }
        printf("Authentication failed. Try again.\n");
    }
    getchar(); /* clear leftover newline */

    /* Start background thread to receive messages any time (from server or other clients) */
    pthread_t tid;
    pthread_create(&tid, NULL, receiveLoop, NULL);
    pthread_detach(tid);

    printf("\nType messages to send (type QUIT to exit):\n> ");
    while (1) {
        fgets(buf, BUF, stdin);
        buf[strcspn(buf, "\n")] = '\0';

        if (strlen(buf) == 0) { printf("> "); continue; }

        char msg[BUF + 10];
        if (strcmp(buf, "QUIT") == 0) {
            snprintf(msg, sizeof(msg), "QUIT");
            send(sock, msg, strlen(msg), 0);
            break;
        }
        snprintf(msg, sizeof(msg), "CHAT %s", buf);
        send(sock, msg, strlen(msg), 0);
        printf("> ");
    }

    close(sock);
    return 0;
}
