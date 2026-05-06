#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define MAXLINE 8192
#define LISTENQ 1024

#ifdef _WIN32
typedef int ssize_t;
#endif

typedef struct sockaddr SA;

SOCKET Open_listenfd(char *port) {
    SOCKET listenfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int optval = 1;
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;  // Используем IPv4 для простоты
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    printf("Binding to port %s\n", port);
    
    if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerrorA(rv));
        return INVALID_SOCKET;
    }
    
    for (p = servinfo; p != NULL; p = p->ai_next) {
        listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listenfd == INVALID_SOCKET) {
            printf("socket failed: %d\n", WSAGetLastError());
            continue;
        }
        
        if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, 
                       (const char*)&optval, sizeof(optval)) == SOCKET_ERROR) {
            printf("setsockopt failed: %d\n", WSAGetLastError());
            closesocket(listenfd);
            continue;
        }
        
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == SOCKET_ERROR) {
            printf("bind failed on port %s: %d\n", port, WSAGetLastError());
            closesocket(listenfd);
            continue;
        }
        
        if (listen(listenfd, LISTENQ) == SOCKET_ERROR) {
            printf("listen failed: %d\n", WSAGetLastError());
            closesocket(listenfd);
            continue;
        }
        printf("Socket bound and listening\n");
        break;
    }
    
    if (p == NULL) {
        fprintf(stderr, "Failed to bind and listen\n");
        freeaddrinfo(servinfo);
        return INVALID_SOCKET;
    }
    
    freeaddrinfo(servinfo);
    return listenfd;
}

int Rio_readlineb(SOCKET fd, void *buf, size_t maxlen) {
    int n, rc;
    char c;
    char *ptr = (char *)buf;
    
    for (n = 1; n < (int)maxlen; n++) {
        rc = recv(fd, &c, 1, 0);
        if (rc == 1) {
            *ptr++ = c;
            if (c == '\n')
                break;
        } else if (rc == 0) {
            if (n == 1)
                return 0;
            else
                break;
        } else {
            if (WSAGetLastError() == WSAEINTR)
                continue;
            printf("recv error: %d\n", WSAGetLastError());
            return -1;
        }
    }
    *ptr = '\0';
    return n;
}

int Rio_writen(SOCKET fd, void *buf, size_t n) {
    size_t nleft = n;
    ssize_t nwritten;
    char *ptr = (char *)buf;
    
    while (nleft > 0) {
        nwritten = send(fd, ptr, (int)nleft, 0);
        if (nwritten <= 0) {
            if (nwritten < 0 && WSAGetLastError() == WSAEINTR)
                continue;
            else {
                printf("send error: %d\n", WSAGetLastError());
                return -1;
            }
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return (int)n;
}

void echo(SOCKET connfd) {
    ssize_t n;
    char buf[MAXLINE];
    
    while (1) {
        n = Rio_readlineb(connfd, buf, MAXLINE);
        if (n <= 0) {
            if (n == 0) {
                printf("Client disconnected\n");
            }
            break;
        }
        
        printf("Received (%d bytes): %s", (int)n, buf);
        
        if (Rio_writen(connfd, buf, n) != (int)n) {
            fprintf(stderr, "Error writing to client\n");
            break;
        }
        printf("Echoed back\n");
    }
}

int main(int argc, char **argv) {
    SOCKET listenfd, connfd;
    struct sockaddr_storage clientaddr;
    socklen_t clientlen;
    char client_hostname[MAXLINE], client_port[MAXLINE];
    WSADATA wsaData;
    
    // Инициализация Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed\n");
        exit(1);
    }
    
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        WSACleanup();
        exit(0);
    }
    
    listenfd = Open_listenfd(argv[1]);
    if (listenfd == INVALID_SOCKET) {
        fprintf(stderr, "Failed to create listening socket\n");
        WSACleanup();
        exit(1);
    }
    
    printf("\n========================================\n");
    printf("Server listening on port %s\n", argv[1]);
    printf("Press Ctrl+C to stop the server\n");
    printf("========================================\n\n");
    
    while (1) {
        clientlen = sizeof(struct sockaddr_storage);
        printf("Waiting for connection...\n");
        connfd = accept(listenfd, (SA *)&clientaddr, &clientlen);
        
        if (connfd == INVALID_SOCKET) {
            fprintf(stderr, "Accept failed: %d\n", WSAGetLastError());
            continue;
        }
        
        getnameinfo((SA *)&clientaddr, clientlen, client_hostname, MAXLINE, 
                   client_port, MAXLINE, NI_NUMERICHOST | NI_NUMERICSERV);
        printf("========================================\n");
        printf("Connected from %s:%s\n", client_hostname, client_port);
        printf("========================================\n\n");
        
        echo(connfd);
        
        closesocket(connfd);
        printf("Connection closed\n\n");
    }
    
    closesocket(listenfd);
    WSACleanup();
    exit(0);
}