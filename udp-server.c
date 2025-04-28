#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")

int main()
{
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server, client;
    int client_len = sizeof(client);
    char buffer[1024];
    int random_number, closest_guess = -1, closest_diff = 9999;
    int timeout = 8000; // 8 seconds

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
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(5000);

    if (bind(sock ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed with error code : %d" , WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    printf("Server started. Waiting for data...\n");

    srand(time(0));
    random_number = rand() % 100;
    printf("Random number: %d\n", random_number);

    while(1)
    {
        // Set timeout
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

        int recv_len = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *) &client, &client_len);
        if (recv_len == SOCKET_ERROR)
        {
            int err = WSAGetLastError();
            if (err == WSAETIMEDOUT) {
                printf("Timeout occurred.\n");
                // Send "You won!"
                char *msg = "You won!";
                sendto(sock, msg, strlen(msg), 0, (struct sockaddr *) &client, client_len);

                // Now start 16 sec timeout
                timeout = 16000;
                setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

                recv_len = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *) &client, &client_len);
                if (recv_len == SOCKET_ERROR)
                {
                    printf("No message within 16 seconds. Sending 'You won!'\n");
                    sendto(sock, msg, strlen(msg), 0, (struct sockaddr *) &client, client_len);
                }
                else
                {
                    printf("Received after 16 sec: %s\n", buffer);
                }

                // New round
                random_number = rand() % 100;
                printf("New random number: %d\n", random_number);
                timeout = 8000;
                closest_diff = 9999;
                closest_guess = -1;
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
            int guess = atoi(buffer);
            printf("Received guess: %d\n", guess);

            int diff = abs(random_number - guess);
            if (diff < closest_diff)
            {
                closest_diff = diff;
                closest_guess = guess;
            }

            timeout /= 2;
            if (timeout < 1000) timeout = 1000; // minimum 1 second
        }
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
