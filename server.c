/* Multi-Client TCP Server with Authentication
   Protocol: AUTH <user> <pass>  -> "OK" / "FAIL"
             MSG <text>          -> server echoes "ECHO: <text>"
             QUIT                -> closes connection
   Compile: gcc server.c -o server -lpthread
   Run    : ./server 8080
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define BUF 512
#define UNAME "admin"
#define PASS  "admin123"

void *handleClient(void *arg) {
    int sock = *(int *)arg;
    free(arg);
    char buf[BUF], user[50], pass[50];
    int authenticated = 0;

    /* ---- Authentication phase ---- */
    while (!authenticated) {
        int n = recv(sock, buf, BUF - 1, 0);
        if (n <= 0) { close(sock); return NULL; }
        buf[n] = '\0';
        if (sscanf(buf, "AUTH %49s %49s", user, pass) == 2 &&
            strcmp(user, UNAME) == 0 && strcmp(pass, PASS) == 0) {
            send(sock, "OK\n", 3, 0);
            authenticated = 1;
        } else {
            send(sock, "FAIL\n", 5, 0);
        }
    }
    printf("[+] Client authenticated (socket %d)\n", sock);

    /* ---- Message exchange phase ---- */
    while (1) {
        int n = recv(sock, buf, BUF - 1, 0);
        if (n <= 0) { printf("[-] Client disconnected (socket %d)\n", sock); break; }
        buf[n] = '\0';
        buf[strcspn(buf, "\r\n")] = '\0'; /* strip newline */

        if (strncmp(buf, "QUIT", 4) == 0) {
            send(sock, "BYE\n", 4, 0);
            break;
        } else if (strncmp(buf, "MSG ", 4) == 0 && strlen(buf) > 4 && strlen(buf) < BUF) {
            char reply[BUF + 10];
            snprintf(reply, sizeof(reply), "ECHO: %s\n", buf + 4);
            send(sock, reply, strlen(reply), 0);
        } else {
            send(sock, "ERR: invalid or empty command\n", 31, 0);
        }
    }
    close(sock);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) { printf("Usage: %s <port>\n", argv[0]); return 1; }
    int port = atoi(argv[1]);

    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0) { perror("socket"); return 1; }

    int opt = 1;
    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(serverSock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind"); return 1;
    }
    if (listen(serverSock, 10) < 0) { perror("listen"); return 1; }
    printf("Server listening on port %d...\n", port);

    while (1) {
        int *clientSock = malloc(sizeof(int));
        *clientSock = accept(serverSock, NULL, NULL);
        if (*clientSock < 0) { perror("accept"); free(clientSock); continue; }

        pthread_t tid;
        pthread_create(&tid, NULL, handleClient, clientSock);
        pthread_detach(tid); /* auto-cleanup thread resources */
    }

    close(serverSock);
    return 0;
}
