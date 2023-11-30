#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9000

int main() {
    int sock = 0;
    struct sockaddr_in server;
    char message[1024] = {0};

    // Tạo socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    // Cấu hình địa chỉ và cổng của server
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    // Chuyển đổi địa chỉ IP từ dạng chuỗi sang địa chỉ số học
    if (inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Kết nối đến server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    // Gửi và nhận thông điệp với server
    while (1) {
        fgets(message, sizeof(message), stdin);

        // Gửi thông điệp
        send(sock, message, strlen(message), 0);

        // Nhận và in ra thông điệp từ server
        memset(message, 0, sizeof(message));
        recv(sock, message, sizeof(message), 0);
        printf("%s\n", message);
    }

    return 0;
}