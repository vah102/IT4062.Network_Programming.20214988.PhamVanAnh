#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <time.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define BASE_XOR_KEY 'K'
#define TIMEOUT_SEC 10  // Timeout 10 giây

void xor_cipher(char *data, char key) {
    for (int i = 0; data[i] != '\0'; i++) {
        data[i] ^= key;
    }
}

int main() {
    int client_fd;
    struct sockaddr_in server_addr, response_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(response_addr);
    fd_set read_fds;
    struct timeval timeout;

    // Địa chỉ IP server mà client mong muốn kết nối tới
    char *expected_ip = "127.0.0.1";  // IP của server mong đợi
    struct in_addr expected_server_addr;
    inet_pton(AF_INET, expected_ip, &expected_server_addr);

    // Tạo socket UDP cho client
    if ((client_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(expected_ip);

    while (1) {
        // Nhập thông điệp từ người dùng
        printf("Enter message to send to server: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;  // Loại bỏ ký tự newline

        // Gửi thông điệp đến server
        if (sendto(client_fd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
            perror("Failed to send data");
            continue;  // Tiếp tục vòng lặp nếu gặp lỗi
        }

        printf("Sent message to server: %s\n", buffer);

        // Đặt timeout để nhận phản hồi từ server
        FD_ZERO(&read_fds);
        FD_SET(client_fd, &read_fds);
        timeout.tv_sec = TIMEOUT_SEC;
        timeout.tv_usec = 0;

        // Chờ dữ liệu từ server với timeout
        int activity = select(client_fd + 1, &read_fds, NULL, NULL, &timeout);
        if (activity == -1) {
            perror("Select error");
            continue;  // Tiếp tục vòng lặp nếu gặp lỗi
        } else if (activity == 0) {
            printf("Timeout, no data received from server.\n");
            continue;  // Tiếp tục vòng lặp nếu không nhận được phản hồi
        }

        // Nhận phản hồi từ server
        int recv_len = recvfrom(client_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&response_addr, &addr_len);
        if (recv_len == -1) {
            perror("Failed to receive data");
            continue;  // Tiếp tục vòng lặp nếu gặp lỗi
        }

        buffer[recv_len] = '\0';  // Kết thúc chuỗi

        // So sánh địa chỉ IP của server với địa chỉ mong đợi
        if (memcmp(&response_addr.sin_addr, &expected_server_addr, sizeof(expected_server_addr)) != 0) {
            printf("Error: Received message from unknown IP address: %s\n", inet_ntoa(response_addr.sin_addr));
            continue;  // Tiếp tục vòng lặp nếu IP không khớp
        }

        printf("Received message from verified server IP: %s\n", inet_ntoa(response_addr.sin_addr));

        // Giải mã thông điệp nhận được
        char xor_key = BASE_XOR_KEY + (time(NULL) % 26); // Sử dụng khóa mã hóa giống server
        xor_cipher(buffer, xor_key);
        printf("Decrypted message from server: %s\n", buffer);
    }

    close(client_fd);
    return 0;
}
