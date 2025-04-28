#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main()
{
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    int server_len = sizeof(server);
    char buffer[1024];
    char input[100];
    
    printf("Initializing Winsock...\n");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        printf("Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }

    sock = socket(AF_INET , SOCK_DGRAM , 0);
    if (sock == INVALID_SOCKET) {
        printf("Could not create socket : %d" , WSAGetLastError());
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // Localhost
    server.sin_port = htons(5000);

    while (1)
    {
        printf("Enter your guess: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0; // remove newline

        sendto(sock, input, strlen(input), 0, (struct sockaddr *)&server, server_len);

        // Set 16 seconds timeout
        int timeout = 16000;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

        int recv_len = recvfrom(sock, buffer, sizeof(buffer), 0, NULL, NULL);
        if (recv_len == SOCKET_ERROR)
        {
            int err = WSAGetLastError();
            if (err == WSAETIMEDOUT)
            {
                printf("You lost?\n");
            }
            else
            {
                printf("recvfrom() failed: %d\n", err);
                break;
            }
        }
        else
        {
            buffer[recv_len] = '\0';
            printf("Server says: %s\n", buffer);
        }
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
