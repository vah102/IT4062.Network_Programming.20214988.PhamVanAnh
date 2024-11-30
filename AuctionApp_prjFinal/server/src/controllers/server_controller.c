#include <gtk/gtk.h>
#include <sys/socket.h>
#include <string.h>
#include "user.h"
#include "message_type.h"
#include "room.h"
#include "item.h"

#define BUFFER_SIZE 100

int handle_login(int client_socket, char buffer[BUFFER_SIZE])
{
    User user;
    memcpy(&user, &buffer[1], sizeof(user)); // Deserialize dữ liệu từ buffer

    printf("Nhận thông tin đăng nhập từ client: %s\n", user.username);

    // Lưu thông tin người dùng
    int result = authenticateUser(user);

    if (result)
    {
        char *success = "Đăng nhập thành công!";
        send(client_socket, success, strlen(success), 0);
    }
    else
    {
        char *error = "Sai thông tin đăng nhập!";
        send(client_socket, error, strlen(error), 0);
    }
    return 0;
}

int handle_register(int client_socket, char buffer[BUFFER_SIZE])
{
    User user;
    memcpy(&user, &buffer[1], sizeof(user)); // Deserialize dữ liệu từ buffer

    printf("Nhận thông tin đăng ký từ client: %s\n", user.username);
    int result = saveUser(user);

    if (result == 1)
    {
        char *success = "Đăng ký thành công!";
        send(client_socket, success, strlen(success), 0);
    }
    else if (result == 0)
    {
        char *exists = "Người dùng đã tồn tại!";
        send(client_socket, exists, strlen(exists), 0);
    }
    else
    {
        char *error = "Lỗi lưu thông tin!";
        send(client_socket, error, strlen(error), 0);
    }
    return 0;
}

void handleCreateRoom(int sockfd, const char *roomName) {
    // Kiểm tra nếu tên phòng hợp lệ
    if (roomName == NULL || strlen(roomName) == 0) {
        send(sockfd, "Tên phòng không hợp lệ.", 23, 0);
        return;
    }

    // Tạo phòng đấu giá mới
    if (createRoom(roomName)) {
        send(sockfd, "Phòng đấu giá đã được tạo thành công.", 38, 0);
    } else {
        send(sockfd, "Lỗi khi tạo phòng đấu giá.", 26, 0);
    }
}

// // Hàm liệt kê các phòng đã tồn tại
// void handleListRooms() {
//     Room rooms[100];
//     int numRooms = 0;

//     if (loadRooms(rooms, &numRooms)) {
//         if (numRooms == 0) {
//             printf("Không có phòng đấu giá nào.\n");
//         } else {
//             printf("Danh sách các phòng đấu giá:\n");
//             for (int i = 0; i < numRooms; i++) {
//                 printf("ID: %d, Tên: %s\n", rooms[i].roomId, rooms[i].roomName);
//             }
//         }
//     } else {
//         printf("Không thể tải phòng đấu giá.\n");
//     }
// }

// Hàm xóa phòng đấu giá
void handleDeleteRoom(int sockfd, int roomId) {
    // Xử lý việc xóa phòng
    if (roomId >= 0) {
        deleteRoom(roomId);  // Gọi hàm deleteRoom để xóa phòng
        send(sockfd, "Phòng đấu giá đã được xóa.", 28, 0);
    } else {
        send(sockfd, "ID phòng không hợp lệ.", 24, 0);
    }
}


// Hàm xử lý tạo vật phẩm
void handleCreateItem(int sockfd, const char *itemData) {
    // Giả sử itemData chứa thông tin vật phẩm dưới dạng chuỗi, cần phân tích để tạo Item
    Item newItem;
    sscanf(itemData, "%d %s %d %d %d", &newItem.itemId, newItem.name, &newItem.startingPrice, &newItem.buyNowPrice, &newItem.auctionTime);

    if (saveItem(newItem) > 0) {
        send(sockfd, "Vật phẩm đã được tạo thành công.", 30, 0);
    } else {
        send(sockfd, "Lỗi khi tạo vật phẩm.", 20, 0);
    }
}

// Hàm xử lý liệt kê các vật phẩm trong phòng
// void handleListItems(int sockfd, int roomId) {
//     Item items[10];  // Giới hạn 10 vật phẩm trong một lần yêu cầu
//     int count = listItems(roomId, items, 10);

//     if (count > 0) {
//         for (int i = 0; i < count; i++) {
//             char buffer[200];
//             sprintf(buffer, "ID: %d, Tên: %s, Giá khởi điểm: %d, Giá bán ngay: %d, Thời gian: %d\n",
//                     items[i].itemId, items[i].name, items[i].startingPrice, items[i].buyNowPrice, items[i].auctionTime);
//             send(sockfd, buffer, strlen(buffer), 0);
//         }
//     } else {
//         send(sockfd, "Không có vật phẩm nào trong phòng đấu giá.", 40, 0);
//     }
// }

// Hàm xử lý xóa vật phẩm
void handleDeleteItem(int sockfd, int itemId) {
    if (deleteItem(itemId) > 0) {
        send(sockfd, "Vật phẩm đã được xóa thành công.", 30, 0);
    } else {
        send(sockfd, "Lỗi khi xóa vật phẩm.", 20, 0);
    }
}