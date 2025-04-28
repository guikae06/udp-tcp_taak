// udp_client.c
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    int server_len = sizeof(server);
    char message[2000], server_reply[2000];
    int recv_size;
    char server_ip[100];
    DWORD timeout = 16000; // 16 sec timeout

    printf("Enter server IP address: ");
    scanf("%s", server_ip);

    printf("Initialising Winsock...\n");
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("Failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Initialised.\n");

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Socket created.\n");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(server_ip);
    server.sin_port = htons(8888);

    while (1) {
        printf("Enter your guess: ");
        scanf("%s", message);

        sendto(sock, message, strlen(message), 0, (struct sockaddr *)&server, server_len);

        // Set receive timeout
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));

        recv_size = recvfrom(sock, server_reply, sizeof(server_reply), 0, (struct sockaddr *)&server, &server_len);

        if (recv_size == SOCKET_ERROR) {
            printf("You lost ?\n");
        } else {
            server_reply[recv_size] = '\0';
            printf("Server reply: %s\n", server_reply);
        }
    }

    closesocket(sock);
    WSACleanup();

    return 0;
}
