#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define BOARD_SIZE 30

void print_board(char* board, int size) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf("%c", board[i*BOARD_SIZE + j]);
        }
        printf("\n");
    }
}

void init_board(char* board, int size) {
	for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
			board[i*size + j] = rand()%7+65;
        }
    }
}

/** Program entry point */
int main(void) {
	char board[BOARD_SIZE * BOARD_SIZE] = {0};
	
	srand(time(NULL));
	
	init_board(board, BOARD_SIZE);
    print_board(board, BOARD_SIZE);

    return 0;
}
