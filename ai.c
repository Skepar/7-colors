#include <stdlib.h>
#include "ai.h"
#include "utils.h"
#include "game.h"
#define SIZE 30

char get_symbol(Game* game_ptr) {
    if (game_ptr->current == A_PLAYING) {
        return '*';
    } else {
        return '^';
    }
}

char random_strategy(Game* game_ptr) {
    return rand()%7+65;
}

char semi_random_strategy(Game* game_ptr) {
    char neighbour_colors[7] = {0}; //At the end of the function, neighbour_colors[i] contains 1 if the ith color is adjacent to the player zone, 0 otherwise
    for (int i = 0; i < SIZE; i++) {
  		for (int j = 0; j < SIZE; j++) {
            if (game_ptr->board[i][j] != '*' && game_ptr->board[i][j] != '^' && player_adjacent(game_ptr,i,j) == 1) {
                neighbour_colors[game_ptr->board[i][j]-65] = 1;
            }
        }
    }
    int n = sum(neighbour_colors,7);
    return nth_occurence(rand()%n, 1, neighbour_colors, 7)+65;
}

char greedy_strategy(Game* game_ptr) {
	return 'A'; //TO DO
}

char ai_strategy(Game* game_ptr) {
  if (game_ptr->game_mode == 1) {
    return random_strategy(game_ptr);
  } else if (game_ptr->game_mode == 2) {
    return semi_random_strategy(game_ptr);
  } else if (game_ptr->game_mode == 3) {
		return greedy_strategy(game_ptr);
	}
  return 'A';
}

/** Returns 1 if the cell (i,j) is adjacent to a cell possessed by the current_player*/
char player_adjacent(Game* game_ptr,int i,int j) {
    char player_symbol = get_symbol(game_ptr);
    
	if (i-1 >= 0) {
        if (game_ptr->board[i-1][j]==player_symbol) {
            return 1;
		}
	}
	if (i+1 < SIZE) {
		if (game_ptr->board[i+1][j]==player_symbol) {
            return 1;
		}
	}
	if (j-1 >= 0) {
		if (game_ptr->board[i][j-1]==player_symbol) {
            return 1;
		}
	}
	if (j+1 < SIZE) {
		if (game_ptr->board[i][j+1]==player_symbol) {
            return 1;
		}
	}
	return 0;
}
