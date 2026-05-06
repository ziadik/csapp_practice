#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

#define MAXLINE 8192

int main(int argc, char **argv)
{
    struct addrinfo *p, *listp, hints;
    char buf[MAXLINE];
    int rc, flags;
    
    /* Проверка аргументов */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <domain name>\n", argv[0]);
        return 1;
    }
    
    /* Инициализация Winsock (ОБЯЗАТЕЛЬНО для Windows!) */
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed\n");
        return 1;
    }
    
    /* Настройка подсказок */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;     /* IPv4 и IPv6 */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;
    
    /* Получение списка адресов */
    if ((rc = getaddrinfo(argv[1], NULL, &hints, &listp)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(rc));
        WSACleanup();
        return 1;
    }
    
    /* Вывод IP-адресов */
    flags = NI_NUMERICHOST;
    printf("IP addresses for %s:\n", argv[1]);
    printf("------------------------\n");
    
    for (p = listp; p != NULL; p = p->ai_next) {
        if (getnameinfo(p->ai_addr, p->ai_addrlen,
                        buf, MAXLINE, NULL, 0, flags) == 0) {
            printf("%s\n", buf);
        }
    }
    
    /* Освобождение ресурсов */
    freeaddrinfo(listp);
    WSACleanup();
    
    return 0;
}