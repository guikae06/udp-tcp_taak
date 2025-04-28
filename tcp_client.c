// tcp_client.c
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    int guess, network_guess;
    char server_reply[2000];
    int recv_size;
    char server_ip[100];

    printf("Enter server IP address: ");
    scanf("%s", server_ip);

    printf("Initialising Winsock...\n");
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("Failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Initialised.\n");

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Socket created.\n");

    server.sin_addr.s_addr = inet_addr(server_ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(8888);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Connect error\n");
        return 1;
    }
    printf("Connected to server.\n");

    while (1) {
        printf("Enter your guess (-1 to quit): ");
        scanf("%d", &guess);

        network_guess = htonl(guess);
        send(sock, (char *)&network_guess, sizeof(network_guess), 0);

        if ((recv_size = recv(sock, server_reply, 2000, 0)) == SOCKET_ERROR) {
            puts("recv failed");
            break;
        }
        server_reply[recv_size] = '\0';
        printf("Server response: %s\n", server_reply);

        if (strcmp(server_reply, "Correct") == 0 || guess == -1) {
            break;
        }
    }

    closesocket(sock);
    WSACleanup();

    return 0;
}
