// Server Kodu
#include <winsock2.h>
#include <stdio.h>
#include <string.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

// Öðrenci verisi için struct
typedef struct {
    char name[50];
    int id;
} Student;

// Önceden tanýmlanmýþ Öðrenci Verileri
Student students[20] = {
    {"Ali", 1001}, {"Ayse", 1002}, {"Mehmet", 1003}, {"Baris", 1004},
    {"Can", 1005}, {"Tunc", 1006}, {"Yusuf", 1007}, {"Anýl", 1008},
    {"Mert", 1009}, {"Ece", 1010}, {"Cem", 1011}, {"Kutlu", 1012},
    {"Harun", 1013}, {"Havva", 1014}, {"Burak", 1015}, {"Emine", 1016},
    {"Onur", 1017}, {"Sevval", 1018}, {"Osman", 1019}, {"Ahmet", 1020}
};

DWORD WINAPI handle_client(LPVOID client_socket) {
    SOCKET sock = *(SOCKET *)client_socket;
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    int read_size;

    while ((read_size = recv(sock, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[read_size] = '\0';
        printf("Received: %s\n", buffer);

        // Öðrenci aranýyor...
        int found = 0;
        int i;
        for ( i= 0; i < 20; i++) {
            if (strcmp(buffer, students[i].name) == 0) {
                sprintf(response, "Student ID: %d", students[i].id);
                found = 1;
                break;
            }
        }

        if (!found) {
            strcpy(response, "Student not found");
        }

        // Clienta cevap gönder
        send(sock, response, strlen(response), 0);
    }

    if (read_size == 0) {
        printf("Client disconnected\n");
    } else if (read_size == SOCKET_ERROR) {
        printf("recv failed\n");
    }

    closesocket(sock);
    free(client_socket);
    return 0;
}

int main() {
    WSADATA wsa;
    SOCKET server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Winsocku baþlat
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // Soket Oluþtur
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket: %d\n", WSAGetLastError());
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Soketi porta baðla
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        printf("Bind failed: %d\n", WSAGetLastError());
        return 1;
    }

    // Gelen Baðlantýlarý Dinle
    if (listen(server_fd, MAX_CLIENTS) == SOCKET_ERROR) {
        printf("Listen failed: %d\n", WSAGetLastError());
        return 1;
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (new_socket == INVALID_SOCKET) {
            printf("Accept failed: %d\n", WSAGetLastError());
            continue;
        }

        printf("New connection accepted\n");

        HANDLE thread = CreateThread(NULL, 0, handle_client, (LPVOID)&new_socket, 0, NULL);
        if (thread == NULL) {
            printf("Thread creation failed: %d\n", GetLastError());
            closesocket(new_socket);
        }
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}

