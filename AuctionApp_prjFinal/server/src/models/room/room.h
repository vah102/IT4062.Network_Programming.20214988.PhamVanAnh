#ifndef ROOM_H
#define ROOM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 20

#define ROOM_DATA "data/rooms.dat"
#define MAX_ROOMS 100

// Cấu trúc thông tin phòng đấu giá
typedef struct {
    int room_id;                    // ID của phòng
    char roomName[MAX_LENGTH]; // Tên phòng đấu giá
    int numUsers;                  // Số người tham gia
    int numItems;                  // Số vật phẩm trong phòng
    char username[MAX_LENGTH];
} Room;

// Khai báo các hàm thao tác với phòng đấu giá
int checkRoomExists(Room room);
int getNextRoomId();
int createRoom(const char *roomName, const char *username);
int deleteRoom(int room_id);
int loadRooms(Room *rooms, const char *username);
int updateRoomById(int room_id, const char *newRoomName, int newNumUsers, int newNumItems);
int getRoomById(int room_id, Room *room);

#endif // ROOM_H
