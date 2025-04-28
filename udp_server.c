// udp_server.c
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    SOCKET server_socket;
    struct sockaddr_in server, client;
    int client_len = sizeof(client);
    char buffer[2000];
    int random_number;
    int guess, best_guess = -9999, best_diff = 9999;
    int recv_size;
    DWORD timeout = 8000; // 8 seconden
    fd_set readfds;
    struct timeval tv;
    int activity;

    printf("Initialising Winsock...\n");
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("Failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Initialised.\n");

    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Socket created.\n");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Bind done.\n");

    srand(time(NULL));
    random_number = rand() % 100 + 1;
    printf("Generated number: %d\n", random_number);

    while (1) {
        // Timeout instellen
        FD_ZERO(&readfds);
        FD_SET(server_socket, &readfds);

        tv.tv_sec = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;

        printf("Waiting for guesses...\n");
        activity = select(0, &readfds, NULL, NULL, &tv);

        if (activity > 0) {
            recv_size = recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&client, &client_len);
            if (recv_size == SOCKET_ERROR) {
                printf("recvfrom() failed\n");
                continue;
            }

            buffer[recv_size] = '\0';
            guess = atoi(buffer);

            printf("Received guess: %d from %s\n", guess, inet_ntoa(client.sin_addr));

            int diff = abs(guess - random_number);
            if (diff < best_diff) {
                best_diff = diff;
                best_guess = guess;
            }

            // Na elke guess, reset timeout naar 8 sec opnieuw
            timeout = 8000;
        } else {
            // Timeout gebeurd
            printf("Timeout event.\n");

            if (best_diff == 0) {
                sendto(server_socket, "You won !", 9, 0, (struct sockaddr *)&client, client_len);
            } else {
                sendto(server_socket, "You won ?", 9, 0, (struct sockaddr *)&client, client_len);
            }

            // Na 16 seconden volledige stop
            timeout = 16000;
            best_diff = 9999;
            best_guess = -9999;

            // Tweede timeout zonder input → einde
            FD_ZERO(&readfds);
            FD_SET(server_socket, &readfds);

            tv.tv_sec = timeout / 1000;
            tv.tv_usec = (timeout % 1000) * 1000;
            activity = select(0, &readfds, NULL, NULL, &tv);

            if (activity <= 0) {
                printf("Second timeout, server restart...\n");
                random_number = rand() % 100 + 1;
                printf("Generated new number: %d\n", random_number);
                timeout = 8000;
            } else {
                // Eventueel late berichten nog ontvangen
                recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&client, &client_len);
                sendto(server_socket, "You lost !", 10, 0, (struct sockaddr *)&client, client_len);

                random_number = rand() % 100 + 1;
                printf("Generated new number: %d\n", random_number);
                timeout = 8000;
            }
        }
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}
