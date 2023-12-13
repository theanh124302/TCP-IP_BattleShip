#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 9000

int sock = 0;

// Hàm xử lý nhận dữ liệu từ server
void *receiveThread(void *arg) {
    char message[1024] = {0};
    while (1) {
        // Nhận dữ liệu từ server
        memset(message, 0, sizeof(message));
        recv(sock, message, sizeof(message), 0);
        printf("Server: %s\n", message);
    }
}

int main() {
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

    // Tạo luồng để xử lý việc nhận dữ liệu từ server
    pthread_t receive_thread;
    if (pthread_create(&receive_thread, NULL, receiveThread, NULL) < 0) {
        perror("Thread creation failed");
        return -1;
    }

    // Gửi và nhận thông điệp với server
    while (1) {
        fgets(message, sizeof(message), stdin);

        // Gửi thông điệp
        send(sock, message, strlen(message), 0);
    }

    return 0;
}
