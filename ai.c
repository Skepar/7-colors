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

void count_greedy(Game* game_ptr,char color,int i,int j,char explored[SIZE][SIZE],int* score_ptr) {
  	char player_symbol = get_symbol(game_ptr);
    explored[i][j] = 1;
    if (game_ptr->board[i][j]==color) {
        *score_ptr += 1;
    }
    if (i-1 >= 0) {
        if ((game_ptr->board[i-1][j]==color || game_ptr->board[i-1][j]==player_symbol) && explored[i-1][j] != 1) {
            count_greedy(game_ptr,color,i-1,j,explored,score_ptr);
        }
    }
    if (i+1 < SIZE) {
        if ((game_ptr->board[i+1][j]==color || game_ptr->board[i+1][j]==player_symbol) && explored[i+1][j] != 1) {
            count_greedy(game_ptr,color,i+1,j,explored,score_ptr);
        }
    }
    if (j-1 >= 0) {
        if ((game_ptr->board[i][j-1]==color || game_ptr->board[i][j-1]==player_symbol) && explored[i][j-1] != 1) {
            count_greedy(game_ptr,color,i,j-1,explored,score_ptr);
        }
    }
    if (j+1 < SIZE) {
        if ((game_ptr->board[i][j+1]==color || game_ptr->board[i][j+1]==player_symbol) && explored[i][j+1] != 1) {
            count_greedy(game_ptr,color,i,j+1,explored,score_ptr);
        }
    }
}

char greedy_strategy(Game* game_ptr) {
    int score_gained = 0;
    char color_played = 'A';
	for (int i = 0; i < 7; i++)
    {
        char color = i+65;
        int score = 0;
        char explored[SIZE][SIZE] = {0}; //explored[i][j]=1 when the cell (i,j) have already been updated
        if (game_ptr->current == A_PLAYING) {
                count_greedy(game_ptr,color,SIZE-1,0,explored,&score);
        } else {
                count_greedy(game_ptr,color,0,SIZE-1,explored,&score);
        }
        if (score > score_gained)
        {
            score_gained = score;
            color_played = color;
        }
    }
    return color_played;
}

char ai_strategy(Game* game_ptr) {
	if (game_ptr->game_mode == 1) {
		return random_strategy(game_ptr);
	} else if (game_ptr->game_mode == 2) {
		return semi_random_strategy(game_ptr);
	} else if (game_ptr->game_mode == 3) {
		return greedy_strategy(game_ptr);
	} else {
		if (game_ptr->current == A_PLAYING) {
			return semi_random_strategy(game_ptr);
		} else {
			return greedy_strategy(game_ptr);
		}
	}
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
