#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <assert.h>
#include "caro.h"
#include "flags.h"

#define PORT 8080
#define NUM_PLAYERS 2
#define BUFFER_SIZE 1024
#define STATE_SIZE (BOARD_SIZE * BOARD_SIZE + 1)

typedef struct {
    int socket_fd;
    char player_role;
} player_info_t;

void update_board_buffer(role_t game_board[BOARD_SIZE][BOARD_SIZE], char message_buffer[BUFFER_SIZE]) {
    int index;
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            index = BOARD_SIZE * row + col + 1;
            message_buffer[index] = (char) game_board[row][col];
        }
    }
    message_buffer[0] = STATE_UPDATE;
}

void send_to_all(char message_buffer[BUFFER_SIZE], size_t size, player_info_t players[NUM_PLAYERS]) {
    for (int i = 0; i < NUM_PLAYERS; i++) {
        send(players[i].socket_fd, message_buffer, size, 0);
    }
}

int main() {
    int server_socket;
    player_info_t players[NUM_PLAYERS];
    struct sockaddr_in server_addr;
    int option = 1, address_length = sizeof(server_addr);
    char message_buffer[BUFFER_SIZE];
    role_t game_board[BOARD_SIZE][BOARD_SIZE];
    char current_role = PX;
    int move_x, move_y;
    role_t game_winner;

    clear_board(game_board);

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

    listen(server_socket, 3);

    printf("Server is listening on port %d\n", PORT);

    for (int i = 0; i < NUM_PLAYERS; i++) {
        if ((players[i].socket_fd = accept(server_socket, (struct sockaddr *)&server_addr, (socklen_t*)&address_length)) < 0) {
            perror("Connection acceptance failed");
            exit(EXIT_FAILURE);
        }
        printf("Player %d connected\n", i + 1);

        players[i].player_role = current_role;

        memset(message_buffer, 0, BUFFER_SIZE);
        sprintf(message_buffer, "%cYou're playing as %c\n", NOTIFICATION, current_role);
        send(players[i].socket_fd, message_buffer, strlen(message_buffer), 0);

        current_role = PO;
    }

    while (1) {
        for (int i = 0; i < NUM_PLAYERS; i++) {
            sleep(1);
            memset(message_buffer, 0, BUFFER_SIZE);

            message_buffer[0] = TURN_NOTIFICATION;
            send(players[i].socket_fd, message_buffer, 1, 0);

            while (read(players[i].socket_fd, message_buffer, BUFFER_SIZE) <= 0);

            assert(message_buffer[0] == MOVE);

            move_x = message_buffer[1];
            move_y = message_buffer[2];

            memset(message_buffer, 0, BUFFER_SIZE);

            if (place_symbol(game_board, move_x, move_y, players[i].player_role) == -1) {
                memset(message_buffer, 0, sizeof(message_buffer));
                sprintf(message_buffer, "%cInvalid move\n", NOTIFICATION);
                send(players[i].socket_fd, message_buffer, strlen(message_buffer), 0);

                i -= 1; // Retry the same player
                continue;
            }

            update_board_buffer(game_board, message_buffer);
            send_to_all(message_buffer, STATE_SIZE, players);

            if (is_board_full(game_board) || (game_winner = determine_winner(game_board)) != NONE) {
                goto END_GAME_LOOP;
            }
        }
    }

END_GAME_LOOP:

    sleep(1);

    memset(message_buffer, 0, BUFFER_SIZE);
    message_buffer[0] = RESULT;

    if (game_winner == NONE) {
        message_buffer[1] = DRAW;
        send_to_all(message_buffer, 2, players);
    } else {
        for (int i = 0; i < NUM_PLAYERS; i++) {
            if (players[i].player_role == game_winner) {
                message_buffer[1] = WIN;
            } else {
                message_buffer[1] = LOSE;
            }
            send(players[i].socket_fd, message_buffer, 2, 0);
        }
    }

    for (int i = 0; i < NUM_PLAYERS; i++) {
        close(players[i].socket_fd);
    }
    close(server_socket);
    return 0;
}