#include <stdio.h>
#include <string.h>
#include "room.h"

// Hàm để lấy ID phòng tiếp theo (tăng dần)
int getNextRoomId()
{
    int next_room_id = 1;
    FILE *file = fopen("data/rooms.txt", "r");
    if (file == NULL)
    {
        return next_room_id; // Nếu file chưa tồn tại, bắt đầu với ID 1
    }

    Room room;
    while (fscanf(file, "%d %s %s %d %d\n", &room.room_id, room.roomName, room.username, &room.numUsers, &room.numItems) == 5)
    {
        if (room.room_id >= next_room_id)
        {
            next_room_id = room.room_id + 1;
        }
    }
    fclose(file);
    return next_room_id;
}

// Hàm tạo phòng đấu giá
int createRoom(const char *roomName, const char *username)
{
    int room_id = getNextRoomId();
    Room room;
    room.room_id = room_id;
    strncpy(room.roomName, roomName, sizeof(room.roomName));
    strncpy(room.username, username, sizeof(room.username));
    room.numUsers = 0; // Bắt đầu với số người dùng là 0
    room.numItems = 0; // Bắt đầu với số vật phẩm là 0

    if (checkRoomExists(room))
    {
        return 0;
    }

    FILE *file = fopen("data/rooms.txt", "a");
    if (file == NULL)
    {
        return 0; // Lỗi khi mở file
    }

    fprintf(file, "%d %s %s %d %d\n", room.room_id, room.roomName, room.username, room.numUsers, room.numItems);
    fclose(file);
    return 1; // Thành công
}

// Hàm xóa phòng đấu giá
int deleteRoom(int room_id)
{
    FILE *file = fopen("data/rooms.txt", "r");
    if (file == NULL)
    {
        return 0; // Không thể mở file
    }

    Room rooms[100];
    int numRooms = 0;

    // Đọc tất cả phòng và lưu vào mảng trừ phòng cần xóa
    Room room;
    while (fscanf(file, "%d %s %s %d %d\n", &room.room_id, room.roomName, room.username, &room.numUsers, &room.numItems) == 5)
    {
        if (room.room_id != room_id)
        {
            rooms[numRooms++] = room;
        }
    }
    fclose(file);

    // Nếu không tìm thấy phòng để xóa
    if (numRooms == 0)
    {
        return 0;
    }

    // Ghi lại các phòng còn lại vào file mới
    file = fopen("data/rooms.txt", "w");
    if (file == NULL)
    {
        return 0; // Lỗi khi mở file để ghi lại
    }

    for (int i = 0; i < numRooms; i++)
    {
        fprintf(file, "%d %s %s %d %d\n", rooms[i].room_id, rooms[i].roomName, rooms[i].username, rooms[i].numUsers, rooms[i].numItems);
    }

    fclose(file);
    return 1; // Thành công
}

// Hàm kiểm tra phòng đấu giá đã tồn tại hay chưa
int checkRoomExists(Room room)
{
    FILE *file = fopen("data/rooms.txt", "r");
    if (file == NULL)
    {
        return 0; // Nếu không thể mở file, coi như không có phòng nào
    }

    Room existedRoom;
    while (fscanf(file, "%d %s %s %d %d\n", &existedRoom.room_id, existedRoom.roomName, existedRoom.username, &existedRoom.numUsers, &existedRoom.numItems) == 5)
    {
        if (strcmp(existedRoom.username, room.username) == 0 && strcmp(existedRoom.roomName, room.roomName) == 0)
        {
            fclose(file);
            return room.room_id;
        }
    }

    fclose(file);
    return 0; // Phòng không tồn tại
}

// Hàm tải danh sách phòng đấu giá
int loadRooms(Room *rooms, const char *username)
{
    FILE *file = fopen("data/rooms.txt", "r");
    if (file == NULL)
    {
        perror("Error opening room file");
        return -1;
    }

    int room_count = 0;

    if (username != NULL)
    {
        Room existedRoom;
        while (fscanf(file, "%d %s %s %d %d\n", &existedRoom.room_id, existedRoom.roomName, existedRoom.username, &existedRoom.numUsers, &existedRoom.numItems) == 5)
        {
            if (strcmp(existedRoom.username, username) == 0)
            {
                rooms[room_count++] = existedRoom;
            }
        }
    }
    else
    {
        Room existedRoom;
        while (fscanf(file, "%d %s %s %d %d\n", &existedRoom.room_id, existedRoom.roomName, existedRoom.username, &existedRoom.numUsers, &existedRoom.numItems) == 5)
        {
            rooms[room_count++] = existedRoom;
        }
    }

    fclose(file);    
    return room_count;
}

// Hàm lấy thông tin phòng đấu giá theo id
int getRoomById(int room_id, Room *room)
{
    FILE *file = fopen("data/rooms.txt", "r");
    if (file == NULL)
    {
        return 0;
    }

    Room existedRoom;
    while (fscanf(file, "%d %s %s %d %d\n", &existedRoom.room_id, existedRoom.roomName, existedRoom.username, &existedRoom.numUsers, &existedRoom.numItems) == 5)
    {
        if (existedRoom.room_id == room_id)
        {
            *room = existedRoom;
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0; // Phòng không tồn tại
}

// Hàm cập nhật thông tin phòng đấu giá
int updateRoomById(int room_id, const char *newRoomName, int newNumUsers, int newNumItems)
{
    FILE *file = fopen("data/rooms.txt", "r");
    if (file == NULL)
    {
        return 0; // Không thể mở file
    }

    Room rooms[100];
    int numRooms = 0;
    int found = 0;

    Room room;
    while (fscanf(file, "%d %s %s %d %d\n", &room.room_id, room.roomName, room.username, &room.numUsers, &room.numItems) == 5)
    {
        if (room.room_id == room_id)
        {
            // Cập nhật thông tin
            strncpy(room.roomName, newRoomName, sizeof(room.roomName));
            room.numUsers = newNumUsers;
            room.numItems = newNumItems;
            found = 1;
        }
        rooms[numRooms++] = room;
    }
    fclose(file);

    if (!found)
    {
        return 0; // Không tìm thấy phòng để cập nhật
    }

    file = fopen("data/rooms.txt", "w");
    if (file == NULL)
    {
        return 0; // Không thể mở file để ghi lại
    }

    for (int i = 0; i < numRooms; i++)
    {
        fprintf(file, "%d %s %s %d %d\n", rooms[i].room_id, rooms[i].roomName, rooms[i].username, rooms[i].numUsers, rooms[i].numItems);
    }

    fclose(file);
    return 1; // Thành công
}