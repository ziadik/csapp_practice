#include "csapp.h"

int main(int argc, char **argv)
{
    struct addrinfo *p, *listp, hints;
    char buf[MAXLINE];
    int rc, flags;

    /* Проверка аргументов командной строки */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <domain name>\n", argv[0]);
        exit(0);
    }

    /* Инициализация подсказок для getaddrinfo */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;       /* Только IPv4 */
    hints.ai_socktype = SOCK_STREAM; /* Только TCP */

    /* Получение списка структур addrinfo */
    if ((rc = getaddrinfo(argv[1], NULL, &hints, &listp)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(rc));
        exit(1);
    }

    /* Обход списка и вывод IP-адресов */
    flags = NI_NUMERICHOST; /* Числовой формат адреса */
    for (p = listp; p; p = p->ai_next) {
        Getnameinfo(p->ai_addr, p->ai_addrlen, 
                   buf, MAXLINE, NULL, 0, flags);
        printf("%s\n", buf);
    }

    /* Освобождение ресурсов */
    Freeaddrinfo(listp);
    exit(0);
}