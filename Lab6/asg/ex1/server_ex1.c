#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define XOR_KEY 'K'
#define TIMEOUT_SEC 10  // Timeout 10 giây

void xor_cipher(char *data, char key) {
    for (int i = 0; data[i] != '\0'; i++) {
        data[i] ^= key;
    }
}

int main() {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);
    fd_set read_fds;
    struct timeval timeout;

    // Tạo socket UDP
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Liên kết socket với địa chỉ IP và cổng
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(server_fd, &read_fds);
        timeout.tv_sec = TIMEOUT_SEC;
        timeout.tv_usec = 0;

        // Kiểm tra xem có dữ liệu nào trên socket không với timeout
        int activity = select(server_fd + 1, &read_fds, NULL, NULL, &timeout);
        if (activity == -1) {
            perror("Select error");
            continue;
        } else if (activity == 0) {
            printf("Timeout, no data received.\n");
            continue;
        }

        // Nhận dữ liệu từ client
        int recv_len = recvfrom(server_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (recv_len == -1) {
            perror("Failed to receive data");
            continue;
        }
        buffer[recv_len] = '\0'; // Kết thúc chuỗi

        printf("Received message from client: %s\n", buffer);

        // Mã hóa thông điệp
        xor_cipher(buffer, XOR_KEY);
        printf("Sending encrypted message to client: %s\n", buffer);

        // Gửi dữ liệu mã hóa đến client
        if (sendto(server_fd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, addr_len) == -1) {
            perror("Failed to send data");
        }
    }

    close(server_fd);
    return 0;
}
