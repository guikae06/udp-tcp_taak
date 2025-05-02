#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")
DWORD WINAPI client_handler(LPVOID client_socket_ptr) { //DWORD staat voor double word = 32-bit unsigned integer en WINAPI = _stdcall
    SOCKET client_socket = *(SOCKET*)client_socket_ptr;
    free(client_socket_ptr);

    struct sockaddr_in client;
    int c = sizeof(struct sockaddr_in);
    getpeername(client_socket, (struct sockaddr*)&client, &c);
    printf("Client connected from %s\n", inet_ntoa(client.sin_addr));

    int guess;
    int recv_size;

    while (1) {
	srand(time(NULL));
        int random_number = rand() % 100 + 1;
        printf("New round for %s, number = %d\n", inet_ntoa(client.sin_addr), random_number);
 
		while(1){
			recv_size = recv(client_socket, (char *)&guess, sizeof(guess), 0);
        		if (recv_size <= 0) {
            			printf("Client %s disconnected.\n", inet_ntoa(client.sin_addr));
            			closesocket(client_socket);
						return 0;
						break;
						}
            guess = ntohl(guess);
            printf("Client %s guessed: %d\n", inet_ntoa(client.sin_addr), guess);
	
        
            if (guess == -1) {
                send(client_socket, "Einde", 6, 0);
                closesocket(client_socket);
                printf("Client %s exited the game.\n", inet_ntoa(client.sin_addr));
                return 0;
            } else if (guess > random_number) {
                send(client_socket, "Lager", 6, 0);
            } else if (guess < random_number) {
                send(client_socket, "Hoger", 6, 0);
            } else {
                send(client_socket, "Correct", 8, 0);
		closesocket(client_socket);
		break;
		
                 // nieuwe ronde
            }
		}

        // nieuwe ronde start automatisch
    }

    closesocket(client_socket);
    return 0;
}

int main() {
    WSADATA wsa;
    SOCKET server_socket, *new_sock;
    struct sockaddr_in server, client;
    int c;

    srand((unsigned int)time(NULL));

    printf("Initialising Winsock...\n");
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("Failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Initialised.\n");

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Socket created.\n");

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Bind done.\n");

    listen(server_socket, 5);
    printf("Waiting for incoming connections...\n");

    c = sizeof(struct sockaddr_in);
    while (1) {
        SOCKET client_socket = accept(server_socket, (struct sockaddr *)&client, &c);
        if (client_socket == INVALID_SOCKET) {
            printf("Accept failed: %d\n", WSAGetLastError());
            continue;
        }

        new_sock = malloc(sizeof(SOCKET));
        *new_sock = client_socket;

        HANDLE thread = CreateThread(NULL, 0, client_handler, (void*)new_sock, 0, NULL);
        if (thread == NULL) {
            printf("Could not create thread: %d\n", GetLastError());
            closesocket(client_socket);
            free(new_sock);
        } else {
            CloseHandle(thread);
        }
    }

    closesocket(server_socket);
    WSACleanup();

    return 0;
}
