#include "caro.h"

void display_board(role_t board[BOARD_SIZE][BOARD_SIZE]) {
    printf(HLINE);
    for (int row = 0; row < BOARD_SIZE; row++) {
        printf("|");
        for (int col = 0; col < BOARD_SIZE; col++) {
            if (board[row][col] == PX) {
                printf(" X |");
            } else if (board[row][col] == PO) {
                printf(" O |");
            } else {
                printf("   |");
            }
        }
        printf("\n%s", HLINE);
    }
}

void clear_board(role_t board[BOARD_SIZE][BOARD_SIZE]) {
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            board[row][col] = NONE;  
        }
    }
}

int place_symbol(role_t board[BOARD_SIZE][BOARD_SIZE], int x, int y, role_t symbol) {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
        return -1; // Out of bounds
    }

    if (board[x][y] == NONE) {
        board[x][y] = symbol;
        return 0; // Success
    }
    return -1; // Cell already occupied
}

role_t determine_winner(role_t board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        // Check rows
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != NONE) {
            return board[i][0];
        }
        // Check columns
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != NONE) {
            return board[0][i];
        }
    }

    // Check diagonals
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != NONE) {
        return board[0][0];
    }
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != NONE) {
        return board[0][2];
    }

    return NONE; // No winner
}

int is_board_full(role_t board[BOARD_SIZE][BOARD_SIZE]) {
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            if (board[row][col] == NONE) {
                return 0; // Board is not full
            }
        }
    }
    return 1; // Board is full
}