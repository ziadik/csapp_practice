#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#define MAX_HOSTNAME 256

int main() {
    WSADATA wsaData;
    char hostname[MAX_HOSTNAME];
    char username[MAX_HOSTNAME];
    char domain[MAX_HOSTNAME];
    DWORD size = MAX_HOSTNAME;
    
    system("chcp 65001 > nul");
    
    printf("====================================\n");
    printf("  SYSTEM INFORMATION\n");
    printf("====================================\n\n");
    
    // 1. Windows username
    if (GetUserNameA(username, &size)) {
        printf("Username: %s\n", username);
    } else {
        printf("Username: (failed to get)\n");
    }
    
    // 2. Computer name and domain
    size = MAX_HOSTNAME;
    if (GetComputerNameA(hostname, &size)) {
        printf("Computer name: %s\n", hostname);
    }
    
    // Get domain from environment variable
    char *user_domain = getenv("USERDOMAIN");
    if (user_domain != NULL) {
        printf("Domain/Workgroup: %s\n", user_domain);
    } else {
        printf("Domain/Workgroup: (not defined)\n");
    }
    
    // 3. IP addresses
    printf("\n--- IP Addresses ---\n");
    
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0) {
        struct hostent *host;
        struct in_addr addr;
        
        if (gethostname(hostname, sizeof(hostname)) == 0) {
            host = gethostbyname(hostname);
            if (host != NULL) {
                for (int i = 0; host->h_addr_list[i] != NULL; i++) {
                    memcpy(&addr, host->h_addr_list[i], sizeof(addr));
                    printf("  • %s\n", inet_ntoa(addr));
                }
            }
        }
        WSACleanup();
    } else {
        printf("  • Failed to get IP addresses\n");
    }
    
    printf("\n====================================\n");
    system("pause");
    
    return 0;
}