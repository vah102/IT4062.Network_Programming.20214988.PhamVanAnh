// #include <stdio.h>

// #define BOARD_SIZE 3
// #define HLINE "+---+---+---+\n"

// typedef enum {
//     SYMBOL_NULL,
//     NONE,
//     PX,
//     PO
// } role_t;

// typedef enum {
//     WIN = 1,
//     LOSE,
//     DRAW
// } result_t;

// void display_board(role_t board[BOARD_SIZE][BOARD_SIZE]);

// void clear_board(role_t board[BOARD_SIZE][BOARD_SIZE]);

// int place_symbol(role_t board[BOARD_SIZE][BOARD_SIZE], int x, int y, role_t sym);

// role_t determine_winner(role_t board[BOARD_SIZE][BOARD_SIZE]);

// int is_board_full(role_t board[BOARD_SIZE][BOARD_SIZE]);





#ifndef CARO_H
#define CARO_H

#include <stdio.h>

#define BOARD_SIZE 3 // Board size (3x3)
#define PX 'X'
#define PO 'O'
#define NONE ' '
#define HLINE "+---+---+---+\n"
#define WIN 'W'
#define DRAW 'D'
#define LOSE 'L'

// Type definition for roles on the board
typedef char role_t;

// Function prototypes
int place_symbol(role_t board[BOARD_SIZE][BOARD_SIZE], int x, int y, role_t symbol);
void display_board(role_t board[BOARD_SIZE][BOARD_SIZE]);
void clear_board(role_t board[BOARD_SIZE][BOARD_SIZE]);
role_t determine_winner(role_t board[BOARD_SIZE][BOARD_SIZE]);
int is_board_full(role_t board[BOARD_SIZE][BOARD_SIZE]);

#endif // GAME_H