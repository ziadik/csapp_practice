#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define MAXLINE 8192

typedef struct sockaddr SA;

int main(int argc, char **argv) {
    WSADATA wsaData;
    SOCKET clientfd;
    struct addrinfo hints, *result, *ptr;
    char buf[MAXLINE];
    int iResult;
    
    // Проверка аргументов
    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        return 1;
    }
    
    // Инициализация Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        fprintf(stderr, "WSAStartup failed: %d\n", iResult);
        return 1;
    }
    
    // Настройка подсказок для getaddrinfo
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    
    // Разрешение имени хоста и порта
    iResult = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (iResult != 0) {
        fprintf(stderr, "getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }
    
    // Попытка подключиться к первому доступному адресу
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        clientfd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (clientfd == INVALID_SOCKET) {
            continue;
        }
        
        iResult = connect(clientfd, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(clientfd);
            continue;
        }
        break;
    }
    
    freeaddrinfo(result);
    
    if (ptr == NULL) {
        fprintf(stderr, "Unable to connect to server\n");
        WSACleanup();
        return 1;
    }
    
    printf("Connected to %s:%s\n", argv[1], argv[2]);
    printf("Enter messages (Ctrl+Z then Enter to exit):\n");
    printf("----------------------------------------\n");
    
    // Цикл отправки и получения сообщений
    while (fgets(buf, MAXLINE, stdin) != NULL) {
        // Отправка сообщения
        iResult = send(clientfd, buf, (int)strlen(buf), 0);
        if (iResult == SOCKET_ERROR) {
            printf("send failed: %d\n", WSAGetLastError());
            break;
        }
        
        // Получение ответа
        iResult = recv(clientfd, buf, MAXLINE - 1, 0);
        if (iResult > 0) {
            buf[iResult] = '\0';
            printf("Server: %s", buf);
        } else if (iResult == 0) {
            printf("Connection closed by server\n");
            break;
        } else {
            printf("recv failed: %d\n", WSAGetLastError());
            break;
        }
        printf("----------------------------------------\n");
    }
    
    closesocket(clientfd);
    WSACleanup();
    printf("\nConnection closed\n");
    return 0;
}