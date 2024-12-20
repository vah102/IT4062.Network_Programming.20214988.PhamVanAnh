#include <gtk/gtk.h>
#include <sys/socket.h>
#include <string.h>
#include "user.h"
#include "message_type.h"
#include "session_manager.h"
#include "server_controller.h"
#include "room.h"
#include "item.h"

void handle_login(int client_socket, char buffer[BUFFER_SIZE])
{
    User user;
    memcpy(&user, &buffer[1], sizeof(user)); // Deserialize dữ liệu từ buffer

    printf("Nhận thông tin đăng nhập từ client: %s - %s\n", user.username, user.password);

    // Lưu thông tin người dùng
    int user_id = authenticateUser(user);
    ClientSession *session = find_session_by_socket(client_socket);

    if (user_id > 0 && session != NULL)
    {
        session->user_id = user_id;
        strncpy(session->username, user.username, sizeof(session->username));
        printf("User %s logged in with user_id %d\n", user.username, user_id);

        int response = 1; // Đăng nhập thành công
        send(client_socket, &response, 1, 0);
    }
    else
    {
        int response = 0; // Đăng nhập thất bại
        send(client_socket, &response, 1, 0);
    }
}

void handle_register(int client_socket, char buffer[BUFFER_SIZE])
{
    User user;
    memcpy(&user, &buffer[1], sizeof(user)); // Deserialize dữ liệu từ buffer

    printf("Nhận thông tin đăng ký từ client: %s - %s\n", user.username, user.password);

    // Lưu thông tin người dùng
    int user_id = saveUser(user);
    ClientSession *session = find_session_by_socket(client_socket);

    if (user_id > 0 && session != NULL)
    {
        session->user_id = user_id;
        strncpy(session->username, user.username, sizeof(session->username));
        printf("User %s register with user_id %d\n", user.username, user_id);

        int response = 1; // Đăng ký thành công
        send(client_socket, &response, 1, 0);
    }
    else
    {
        int response = 0; // Đăng ký thất bại
        send(client_socket, &response, 1, 0);
    }
}

void handleCreateRoom(int client_socket, char buffer[BUFFER_SIZE])
{
    char roomName[MAX_LENGTH];
    memcpy(roomName, &buffer[1], MAX_LENGTH);
    ClientSession *session = find_session_by_socket(client_socket);

    if (session != NULL)
    {
        int response = createRoom(roomName, session->username);
        send(client_socket, &response, 1, 0);
    }
    else
    {
        int response = 0;
        send(client_socket, &response, 1, 0);
    }
}

void handleFetchAllRoom(int client_socket)
{
    char buffer[BUFFER_SIZE];
    Room rooms[MAX_ROOMS];
    int room_count = loadRooms(rooms, NULL);

    if (room_count < 0)
    {
        int response = 0;
        send(client_socket, &response, 1, 0);
        return;
    }

    memcpy(&buffer[0], &room_count, 1);
    memcpy(&buffer[1], &rooms, room_count * sizeof(Room));

    if (send(client_socket, buffer, (room_count * sizeof(Room)) + 1, 0) < 0)
    {
        perror("Error sending room data");
        return;
    }
}

void handleFetchOwnRoom(int client_socket)
{
    ClientSession *session = find_session_by_socket(client_socket);

    if (session != NULL)
    {
        char buffer[BUFFER_SIZE];
        Room rooms[MAX_ROOMS];
        int room_count = loadRooms(rooms, session->username);

        if (room_count < 0)
        {
            int response = 0;
            send(client_socket, &response, 1, 0);
            return;
        }

        memcpy(&buffer[0], &room_count, 1);
        memcpy(&buffer[1], &rooms, room_count * sizeof(Room));

        if (send(client_socket, buffer, (room_count * sizeof(Room)) + 1, 0) < 0)
        {
            perror("Error sending room data");
            return;
        }
    }
    else
    {
        int response = 0;
        send(client_socket, &response, 1, 0);
    }
}

// Hàm xóa phòng đấu giá
void handleDeleteRoom(int sockfd, int room_id)
{
    // Xử lý việc xóa phòng
    if (room_id >= 0)
    {
        deleteRoom(room_id); // Gọi hàm deleteRoom để xóa phòng
        send(sockfd, "The auction room has been deleted.", 28, 0);
    }
    else
    {
        send(sockfd, "Invalid room ID.", 24, 0);
    }
}

void handleJoinRoom(int client_socket, int room_id)
{

    char buffer[BUFFER_SIZE];
    ClientSession *session = find_session_by_socket(client_socket);
    Room room;
    int result = getRoomById(room_id, &room);
    printf("%d %s \n", room.room_id, session->username);

    if (result == 0 || session == NULL)
    {
        int response = 0; // Error
        send(client_socket, &response, 1, 0);
        return;
    }

    // Kiểm tra user
    int role = 2;
    if (strcmp(room.username, session->username) == 0)
    {
        role = 1; // Owner
    }
    else
    {
        role = 2; // Joiner
    }

    // Đóng gói dữ liệu
    memcpy(&buffer[0], &role, 1);
    memcpy(&buffer[1], &room, sizeof(Room));

    // Gửi dữ liệu cho client
    if (send(client_socket, buffer, sizeof(Room) + 1, 0) < 0)
    {
        perror("Error sending room data");
        return;
    }
}

void handleCreateItem(int sockfd, char buffer[BUFFER_SIZE]) {
    Item item;

    // Kiểm tra nếu buffer đủ dữ liệu
    if (BUFFER_SIZE < sizeof(Item) + 1) {
        int response = 0; // Thất bại
        send(sockfd, &response, sizeof(response), 0);
        return;
    }

    memcpy(&item, &buffer[1], sizeof(Item));

    if (createItem(item.item_id, item.name, item.startingPrice, item.buyNowPrice, item.auctionTime, item.room_id) > 0) {
        int response = 1; // Thành công
        send(sockfd, &response, sizeof(response), 0);
    } else {
        int response = 0; // Thất bại
        send(sockfd, &response, sizeof(response), 0);
    }
}

void handleFetchItems(int client_socket, char buffer[BUFFER_SIZE]) {
    int room_id;
    memcpy(&room_id, &buffer[1], sizeof(int)); // Deserialize room_id từ buffer

    Item items[MAX_ITEM_IN_ROOM];
    int item_count = loadItems(room_id, items);

    char send_buffer[BUFFER_SIZE];
    memcpy(&send_buffer[0], &item_count, sizeof(int));
    memcpy(&send_buffer[4], items, item_count * sizeof(Item));

    if (send(client_socket, send_buffer, sizeof(int) + item_count * sizeof(Item), 0) < 0) {
        perror("Error sending item data");
    }
}



void handleDeleteItem(int sockfd, int item_id) {
    if (deleteItem(item_id) > 0) {
        char response[] = "The item has been successfully deleted.";
        send(sockfd, response, strlen(response), 0);
    } else {
        char response[] = "Error deleting item.";
        send(sockfd, response, strlen(response), 0);
    }
}

