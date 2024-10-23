#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "caro.h"
#include "flags.h"

#define SERVER_PORT 8080
#define BUFFER_CAPACITY 1024

void request_turn(int socket_fd, char buffer[BUFFER_CAPACITY]) {
    int move_x, move_y;
    printf("Your turn (x,y): ");
    scanf("%d,%d", &move_x, &move_y);

    memset(buffer, 0, BUFFER_CAPACITY);
    sprintf(buffer, "%c%c%c", MOVE, (char) move_x, (char) move_y);
    send(socket_fd, buffer, strlen(buffer), 0);
}

void update_board_from_buffer(role_t game_board[BOARD_SIZE][BOARD_SIZE], char buffer[BUFFER_CAPACITY]) {
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            int index = BOARD_SIZE * row + col + 1;
            game_board[row][col] = (role_t) buffer[index];
        }
    }
}

int setup_connection(struct sockaddr_in *address) {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        printf("\nSocket creation error\n");
        return -1;
    }

    address->sin_family = AF_INET;
    address->sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &address->sin_addr) <= 0) {
        printf("\nInvalid address / Address not supported\n");
        return -1;
    }

    return socket_fd;
}

void handle_server_response(int socket_fd, char buffer[BUFFER_CAPACITY], role_t game_board[BOARD_SIZE][BOARD_SIZE]) {
    int bytes_read = read(socket_fd, buffer, BUFFER_CAPACITY);

    switch (buffer[0]) {
    case NOTIFICATION:
        printf("[Notification] %s", buffer + 1);
        break;

    case TURN_NOTIFICATION:
        request_turn(socket_fd, buffer);
        break;

    case STATE_UPDATE:
        update_board_from_buffer(game_board, buffer);
        display_board(game_board);
        break;

    case RESULT:
        if (buffer[1] == WIN) {
            printf("You win!\n");
        } else if (buffer[1] == DRAW) {
            printf("Draw!\n");
        } else {
            printf("You lose :))\n");
        }
        break;

    default:
        printf("[Error] Invalid message from server\n");
        break;
    }
}

int main() {
    int socket_fd;
    struct sockaddr_in server_address;
    char buffer[BUFFER_CAPACITY];
    role_t game_board[BOARD_SIZE][BOARD_SIZE];

    clear_board(game_board);
    socket_fd = setup_connection(&server_address);
    if (socket_fd < 0) return -1;

    if (connect(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        printf("\nConnection Failed\n");
        return -1;
    }

    while (1) {
        handle_server_response(socket_fd, buffer, game_board);
        
        // Break the loop if RESULT message is received
        if (buffer[0] == RESULT) {
            break;
        }
    }

    // Close socket
    close(socket_fd);
    return 0;
}