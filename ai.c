#include <stdio.h>
#include <stdlib.h>
#include "ai.h"
#include "utils.h"
#include "game.h"
#define SIZE 30

enum Cell_status {UNEXPLORED, PLAYER, COLOR, NOT_COLOR, EXPLORED_TWO_TIMES};
typedef enum Cell_status CellStatus;

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


/**updates the array neighbour_colors, which is filled with 0 before the call of the function, and contains a 1 in the cells corresponding to
a color which is adjacent to the player's territory. Cell 0 corresponds to color A, 1 corresponds to B, 2 to C, etc. */
void update_possible_colors(Game* game_ptr, char (*neighbour_colors)[7]) {
    for (int i = 0; i < SIZE; i++) {
  		for (int j = 0; j < SIZE; j++) {
            if (game_ptr->board[i][j] != '*' && game_ptr->board[i][j] != '^' && player_adjacent(game_ptr,i,j) == 1) {
                (*neighbour_colors)[game_ptr->board[i][j]-65] = 1;
            }
        }
    }
}

char semi_random_strategy(Game* game_ptr) {
    char neighbour_colors[7] = {0}; //At the end of the function, neighbour_colors[i] contains 1 if the ith color is adjacent to the player zone, 0 otherwise
    update_possible_colors(game_ptr,&neighbour_colors);
    int n = sum(neighbour_colors,7);
    if (n==0) {
        return 'A';
    } else {
        return nth_occurence(rand()%n, 1, neighbour_colors, 7)+65; // The +65 is there to "convert" the int into a valid color
    }
}

/**Updates the value of the case (i,j) in the array explored during an exploration. The effect of the function depends
on which type of exploration is happening. The arguments is_second_turn and is_perimeter_based indicate the type of the exploration  */
void update_cell(Game* game_ptr,char color,int i,int j,CellStatus (*explored_ptr)[SIZE][SIZE],int* score_ptr, int* perimeter_ptr, int is_second_turn, int is_perimeter_based) {
    char player_symbol = get_symbol(game_ptr);
    if (game_ptr->board[i][j]==color) {
        if (!is_perimeter_based) {
            *score_ptr += 1; //If we're not during the perimeter based algorithm, we're in the greedy algorithm, so we count the score gained if we explore a cell of the right color
        }
        (*explored_ptr)[i][j] = COLOR;
    } else if (game_ptr->board[i][j]==player_symbol) {
        (*explored_ptr)[i][j] = PLAYER;
    } else {
        (*explored_ptr)[i][j] = NOT_COLOR;
        if (is_perimeter_based) {
            *perimeter_ptr += 1; //If we're during the perimeter based algorithm, we increase the perimeter counter each time we see a cell adjacent to the territory that the player would have if he had played color
        }
    }
    if (is_second_turn) {
        (*explored_ptr)[i][j] = EXPLORED_TWO_TIMES;
    }
}

/** During an exploration, if the cell (i,j) is adjacent to a cell of status origin which is being explored, this function will be called. It will return 1 if
the cell needs to be explored and 0 otherwise */
int must_be_explored(Game* game_ptr, char color, int i, int j, CellStatus (*explored_ptr)[SIZE][SIZE], int is_second_turn, int is_perimeter_based, CellStatus origin) {
    char player_symbol = get_symbol(game_ptr);
    if (is_second_turn) { //During the second turn of exploration (for forseeing greedy strategy), we'll explore cells that are either of the second color tested, or that are in the territory that the player would have if he had played the first color tested AND that aren't explored yet during the second exploration
        return ((game_ptr->board[i][j]==color || (*explored_ptr)[i][j]==COLOR || (*explored_ptr)[i][j]==PLAYER) && (*explored_ptr)[i][j] != EXPLORED_TWO_TIMES);
    } else if (is_perimeter_based){ //If we're perimeter based, we only want to explore cells that are unexplored and next to the territory that the player would have if he had played the color tested
        return ((*explored_ptr)[i][j] == UNEXPLORED && origin != NOT_COLOR);
    } else { //In the greedy exploration, we just want to explore cells with the color tested or cells of the player
        return ((game_ptr->board[i][j] == color || game_ptr->board[i][j] == player_symbol) && (*explored_ptr)[i][j] == UNEXPLORED);
    }
}

/** explore is called recursively on each cell that needs to be explored (these cells depend on the type of exploration that we're doing), updates the cells Status
in the array explored, and increments the counter of score or perimeter depending on the type of exploration.*/
void explore(Game* game_ptr,char color,int i,int j,CellStatus (*explored_ptr)[SIZE][SIZE],int* score_ptr,int* perimeter_ptr, int is_second_turn, int is_perimeter_based) {
  	update_cell(game_ptr, color, i, j, explored_ptr, score_ptr, perimeter_ptr, is_second_turn, is_perimeter_based);
    CellStatus origin = (*explored_ptr)[i][j];

    AdjArray* adj_cells = get_adjacent_coords(game_ptr->board, i, j, SIZE);
    int x, y;

    for (int k = 0; k<adj_cells->last; k++) {
        x = get_x(adj_cells, k);
        y = get_y(adj_cells, k);

        if (must_be_explored(game_ptr, color, x, y, explored_ptr, is_second_turn, is_perimeter_based, origin)) {
			explore(game_ptr,color, x, y, explored_ptr, score_ptr, perimeter_ptr, is_second_turn, is_perimeter_based);
        }
    }
	free_array(adj_cells);
}

char perimeter_based_strategy(Game* game_ptr) {
    int potential_perimeter = -1;
    char color_played = 'A';
    char neighbour_colors[7] = { 0 };
    //At the end of the function, neighbour_colors[i] contains 1 if the ith color is adjacent to the player zone, 0 otherwise
    update_possible_colors(game_ptr,&neighbour_colors);

    for (int i = 0; i < 7; i++) { //exploration for each color
        if (neighbour_colors[i]) {
            char color = i+65;
            int perimeter = 0;
            CellStatus explored[SIZE][SIZE] = { UNEXPLORED };

            if (game_ptr->current == A_PLAYING) {
				explore(game_ptr, color, SIZE-1, 0, &explored, 0, &perimeter,0,1);
            } else {
				explore(game_ptr, color, 0, SIZE-1, &explored, 0, &perimeter,0,1);
            }

            if (perimeter > potential_perimeter) {
                potential_perimeter = perimeter;
                color_played = color;
            }
        }
    }
    return color_played;
}

char greedy_strategy(Game* game_ptr) {
    int score_gained = 0;
    char color_played = 'A';

	for (int i = 0; i < 7; i++) { //exploration for each color
        char color = i+65;
        int score = 0;
        CellStatus explored[SIZE][SIZE] = { UNEXPLORED };

        if (game_ptr->current == A_PLAYING) {
			explore(game_ptr, color, SIZE-1, 0, &explored, &score, 0,0,0);
        } else {
			explore(game_ptr, color, 0, SIZE-1, &explored, &score, 0,0,0);
        }

        if (score > score_gained) {
            score_gained = score;
            color_played = color;
        }
    }
    return color_played;
}

char foreseeing_greedy_strategy(Game* game_ptr) {
    int score_gained = 0;
    int score1_gained = 0;
    char color_played = 'A';

	for (int i = 0; i < 7; i++){
         for (int j = 0; j < 7; j++) { //exploration for each combination of 2 colors
            char color1 = i+65;
            char color2 = j+65;
            int score = 0;
            int score1 = 0;
            CellStatus explored[SIZE][SIZE] = { UNEXPLORED };

            if (game_ptr->current == A_PLAYING) { // The current player impacts the starting point of the exploration
				explore(game_ptr, color1, SIZE-1, 0, &explored, &score, 0,0,0);
				score1 = score;
				if (i != j) { // It's useless to test the case where the AI plays 2 times the same color
					explore(game_ptr, color2, SIZE-1, 0, &explored, &score, 0,1,0);
				}

            } else {
				explore(game_ptr, color1, 0, SIZE-1, &explored, &score, 0,0,0);
				score1 = score;
				if (i != j) {
					explore(game_ptr,color2,0,SIZE-1,&explored,&score,0,1,0);
				}
            }

            if ((score > score_gained) || (score == score_gained && score1 > score1_gained)) {
                 // In case of equality of scores, we must choose the combination where we gain the more score after the first turn,
                 // to avoid being blocked (if there's only one color available for exemple) 
                score_gained = score;
                score1_gained = score1;
                color_played = color1;
            }
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
    } else if (game_ptr->game_mode == 8) {

		if (game_ptr->current == A_PLAYING) {
			return greedy_strategy(game_ptr);
		} else {
			return perimeter_based_strategy(game_ptr);
		}
	}
    // by default, returns A. Should not happen.
    return 'A';
}

/** Returns 1 if the cell (i,j) is adjacent to a cell possessed by the current_player*/
char player_adjacent(Game* game_ptr, int i, int j) {
    char player_symbol = get_symbol(game_ptr);

    AdjArray* adj_cells = get_adjacent_coords(game_ptr->board, i, j, SIZE);
    int x, y;

    for (int k = 0; k<adj_cells->last; k++) {
        x = get_x(adj_cells, k);
        y = get_y(adj_cells, k);

        if (game_ptr->board[x][y] == player_symbol) {
			free_array(adj_cells);
            return 1;
        }
    }
	free_array(adj_cells);
	return 0;
}
