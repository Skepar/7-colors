#include <stdio.h>
#include <stdlib.h>
#include "ai.h"
#include "utils.h"
#include "game.h"
#define SIZE 30

enum Cell_status {UNEXPLORED, PLAYER, COLOR, NOT_COLOR, EXPLORED_TWO_TIMES};

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
    if (n==0) {
        return 'A';
    } else {
        return nth_occurence(rand()%n, 1, neighbour_colors, 7)+65;
    }
}

void update_cell(Game* game_ptr,char color,int i,int j,enum Cell_status (*explored_ptr)[SIZE][SIZE],int* score_ptr, int* perimeter_ptr, int is_second_turn, int is_perimeter_based) {
    char player_symbol = get_symbol(game_ptr);
    if (game_ptr->board[i][j]==color) {
        if (!is_perimeter_based) {
            *score_ptr += 1;
        }
        (*explored_ptr)[i][j] = COLOR;
    } else if (game_ptr->board[i][j]==player_symbol) {
        (*explored_ptr)[i][j] = PLAYER;
    } else {
        (*explored_ptr)[i][j] = NOT_COLOR;
        if (is_perimeter_based) {
            *perimeter_ptr += 1;
        }
    }
    if (is_second_turn) {
        (*explored_ptr)[i][j] = EXPLORED_TWO_TIMES;
    }
}

int must_be_explored(Game* game_ptr,char color,int i,int j,enum Cell_status (*explored_ptr)[SIZE][SIZE], int is_second_turn, int is_perimeter_based, enum Cell_status origin) {
    char player_symbol = get_symbol(game_ptr);
    if (is_second_turn) {
        return ((game_ptr->board[i][j]==color || (*explored_ptr)[i][j]==COLOR || (*explored_ptr)[i][j]==PLAYER) && (*explored_ptr)[i][j] != EXPLORED_TWO_TIMES);
    } else if (is_perimeter_based){
        return ((*explored_ptr)[i][j] == UNEXPLORED && origin != NOT_COLOR);
    }{
        return ((game_ptr->board[i][j]==color || game_ptr->board[i][j]==player_symbol) && (*explored_ptr)[i][j] == UNEXPLORED);
    }
}

void count_greedy(Game* game_ptr,char color,int i,int j,enum Cell_status (*explored_ptr)[SIZE][SIZE],int* score_ptr,int* perimeter_ptr, int is_second_turn, int is_perimeter_based) {
  	update_cell(game_ptr, color, i, j, explored_ptr, score_ptr, perimeter_ptr, is_second_turn, is_perimeter_based);
    enum Cell_status origin = (*explored_ptr)[i][j];
    if ((i-1 >= 0) && must_be_explored(game_ptr,color,i-1,j,explored_ptr,is_second_turn, is_perimeter_based,origin)) {
        count_greedy(game_ptr,color,i-1,j,explored_ptr,score_ptr, perimeter_ptr,is_second_turn, is_perimeter_based);
    }
    if ((i+1 < SIZE) && must_be_explored(game_ptr,color,i+1,j,explored_ptr,is_second_turn, is_perimeter_based,origin)) {
        count_greedy(game_ptr,color,i+1,j,explored_ptr,score_ptr, perimeter_ptr,is_second_turn, is_perimeter_based);
    }
    if ((j-1 >= 0) && must_be_explored(game_ptr,color,i,j-1,explored_ptr,is_second_turn, is_perimeter_based,origin)) {
        count_greedy(game_ptr,color,i,j-1,explored_ptr,score_ptr, perimeter_ptr,is_second_turn, is_perimeter_based);
    }
    if ((j+1 < SIZE) && must_be_explored(game_ptr,color,i,j+1,explored_ptr,is_second_turn, is_perimeter_based,origin)) {
        count_greedy(game_ptr,color,i,j+1,explored_ptr,score_ptr, perimeter_ptr,is_second_turn, is_perimeter_based);
    }
}

char perimeter_based_strategy(Game* game_ptr) {
    int potential_perimeter = -1;
    char color_played = 'A';
	for (int i = 0; i < 7; i++)
    {
        char color = i+65;
        int perimeter = 0;
        enum Cell_status explored[SIZE][SIZE] = {UNEXPLORED}; //explored[i][j]=1 when the cell (i,j) have already been updated
        enum Cell_status (*explored_ptr)[SIZE][SIZE] = &explored;
        if (game_ptr->current == A_PLAYING) {
                count_greedy(game_ptr,color,SIZE-1,0,explored_ptr,0,&perimeter,0,1);
        } else {
                count_greedy(game_ptr,color,0,SIZE-1,explored_ptr,0,&perimeter,0,1);
        }
        //printf("score_step1 %d color1 %c\n",perimeter,color);
        if (perimeter > potential_perimeter)
        {
            potential_perimeter = perimeter;
            color_played = color;
        }
    }
    return color_played;
}

char greedy_strategy(Game* game_ptr) {
    int score_gained = 0;
    char color_played = 'A';
	for (int i = 0; i < 7; i++)
    {
        char color = i+65;
        int score = 0;
        enum Cell_status explored[SIZE][SIZE] = {UNEXPLORED}; //explored[i][j]=1 when the cell (i,j) have already been updated
        enum Cell_status (*explored_ptr)[SIZE][SIZE] = &explored;
        if (game_ptr->current == A_PLAYING) {
                count_greedy(game_ptr,color,SIZE-1,0,explored_ptr,&score,0,0,0);
        } else {
                count_greedy(game_ptr,color,0,SIZE-1,explored_ptr,&score,0,0,0);
        }
        //printf("score_step1 %d color1 %c\n",score,color);
        if (score > score_gained)
        {
            score_gained = score;
            color_played = color;
        }
    }
    return color_played;
}

char foreseeing_greedy_strategy(Game* game_ptr) {
    int score_gained = 0;
    char color_played = 'A';
    int score1_gained = 0;
	for (int i = 0; i < 7; i++){
         for (int j = 0;j < 7; j++) {
            char color1 = i+65;
            char color2 =j+65;
            int score = 0;
            int score1 = 0;
            enum Cell_status explored[SIZE][SIZE] = {UNEXPLORED}; //explored[i][j]=1 when the cell (i,j) have already been updated
            enum Cell_status (*explored_ptr)[SIZE][SIZE] = &explored;
            if (game_ptr->current == A_PLAYING) {
                    count_greedy(game_ptr,color1,SIZE-1,0,explored_ptr,&score,0,0,0);
                    score1 = score;
                    if (i != j) {
                        count_greedy(game_ptr,color2,SIZE-1,0,explored_ptr,&score,0,1,0);
                    }

            } else {
                    count_greedy(game_ptr,color1,0,SIZE-1,explored_ptr,&score,0,0,0);
                    score1 = score;
                    //printf("score_step1 %d color1 %c\n",score,color1);
                    if (i != j) {
                        count_greedy(game_ptr,color2,0,SIZE-1,explored_ptr,&score,0,1,0);
                        //printf("score_step2 %d color2 %c\n",score,color2);
                    }
            }
            //printf("score = %d score gained = %d color = %c\n",score,score_gained,color1);
            if ((score > score_gained) || (score == score_gained && score1 > score1_gained)) {
                score_gained = score;
                score1_gained = score1;
                color_played = color1;
            }
        }
    }
    //printf("%c",color_played);
    return color_played;
}

char ai_strategy(Game* game_ptr) {
	if (game_ptr->game_mode == 1) {
		return random_strategy(game_ptr);
	} else if (game_ptr->game_mode == 2) {
		return semi_random_strategy(game_ptr);
	} else if (game_ptr->game_mode == 3) {
		return greedy_strategy(game_ptr);
	} else if (game_ptr->game_mode == 4){
		if (game_ptr->current == A_PLAYING) {
			return semi_random_strategy(game_ptr);
		} else {
			return greedy_strategy(game_ptr);
		}
	} else if (game_ptr->game_mode == 5) {
        return foreseeing_greedy_strategy(game_ptr);
    } else if (game_ptr->game_mode == 6) {
		if (game_ptr->current == A_PLAYING) {
			return greedy_strategy(game_ptr);
		} else {
			return foreseeing_greedy_strategy(game_ptr);
		}
	} else if (game_ptr->game_mode == 7) {
        return perimeter_based_strategy(game_ptr);
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
