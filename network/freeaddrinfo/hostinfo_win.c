// Объединяем необходимые заголовки, заменяя "csapp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// Линкуем библиотеку сокетов Windows
#pragma comment(lib, "ws2_32.lib")

#define MAXLINE 8192

// Заглушки для совместимости с функционалом csapp
void Getnameinfo(const struct sockaddr *sa, socklen_t salen, char *host,
                 size_t hostlen, char *serv, size_t servlen, int flags) {
    int rc = getnameinfo(sa, salen, host, hostlen, serv, servlen, flags);
    if (rc != 0) {
        fprintf(stderr, "getnameinfo error: %s\n", gai_strerror(rc));
        exit(1);
    }
}

void Freeaddrinfo(struct addrinfo *ai) {
    freeaddrinfo(ai);
}

int main(int argc, char **argv) {
    WSADATA wsaData;
    struct addrinfo *p, *listp, hints;
    char buf[MAXLINE];
    int rc, flags;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <domain name>\n", argv[0]);
        exit(0);
    }

    // !!! КРИТИЧЕСКИ ВАЖНО ДЛЯ WINDOWS !!!
    // Инициализация библиотеки сокетов Windows
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed.\n");
        exit(1);
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;       /* Только IPv4 */
    hints.ai_socktype = SOCK_STREAM;

    if ((rc = getaddrinfo(argv[1], NULL, &hints, &listp)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(rc));
        WSACleanup(); // Очистка перед выходом
        exit(1);
    }

    flags = NI_NUMERICHOST;
    for (p = listp; p; p = p->ai_next) {
        Getnameinfo(p->ai_addr, p->ai_addrlen, buf, MAXLINE, NULL, 0, flags);
        printf("%s\n", buf);
    }

    Freeaddrinfo(listp);
    WSACleanup(); // Обязательная очистка
    exit(0);
}