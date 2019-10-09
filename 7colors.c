#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ai.h"
#include "game.h"
#define SIZE 30

/**list of gamemodes :
*0 : player VS player
*1 : player VS AI random
*2 : player VS AI semi-random
*3 : player VS AI greedy */

typedef struct Game Game;
typedef enum Status Status;

void print_board(char** board) {
    for (int i = 0; i < 50; i++) {
        printf("\n");
    }

	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			printf("%c ", board[i][j]);
		}
		printf("\n");
	}
}

char** init_board() {
    char** board = malloc(SIZE * sizeof(char*));

	for (int i = 0; i < SIZE; i++) {
		board[i] = malloc(SIZE * sizeof(char));
        for (int j = 0; j < SIZE; j++) {
			board[i][j]=rand()%7+65;
		}
	}
	board[SIZE-1][0]='*';
	board[0][SIZE-1]='^';

    return board;
}

void free_board(char** board) {
    for (int i = 0; i < SIZE; i++) {
		free(board[i]);
	}
    free(board);
}

/**Updates the board when a player has played and chosen a color
*Note : The player is coded with an int. 0 is the bottom left player.
*1 is the upper right player.
*/
void world_update(Game* game_ptr, char color) {
  	char player_symbol = get_symbol(game_ptr);
    char modified = 0;

  	for (int i = 0; i < SIZE; i++) {
  		for (int j = 0; j < SIZE; j++) {
  			if (game_ptr->board[i][j] == color && player_adjacent(game_ptr, i, j)) {
                  game_ptr->board[i][j] = player_symbol;
                  if (game_ptr->current == A_PLAYING) {
                      game_ptr->a_score++;
                  } else {
                      game_ptr->b_score++;
                  }
                  modified = 1;
  			}
  		}
  	}

  	if (modified == 1) {
  		world_update(game_ptr, color);
  	}
}

void update(Game* game_ptr,char color,int i,int j,char explored[SIZE][SIZE]) {
  	char player_symbol = get_symbol(game_ptr);

    explored[i][j] = 1;
    if (game_ptr->board[i][j]==color) {
        game_ptr->board[i][j]=player_symbol;
        if (game_ptr->current == A_PLAYING) {
            game_ptr->a_score++;
        } else {
            game_ptr->b_score++;
        }
    }
    if (i-1 >= 0) {
        if (game_ptr->board[i-1][j]==color || (game_ptr->board[i-1][j]==player_symbol && explored[i-1][j] != 1)) {
            update(game_ptr,color,i-1,j,explored);
        }
    }
    if (i+1 < SIZE) {
        if (game_ptr->board[i+1][j]==color || (game_ptr->board[i+1][j]==player_symbol && explored[i+1][j] != 1)) {
            update(game_ptr,color,i+1,j,explored);
        }
    }
    if (j-1 >= 0) {
        if (game_ptr->board[i][j-1]==color || (game_ptr->board[i][j-1]==player_symbol && explored[i][j-1] != 1)) {
            update(game_ptr,color,i,j-1,explored);
        }
    }
    if (j+1 < SIZE) {
        if (game_ptr->board[i][j+1]==color || (game_ptr->board[i][j+1]==player_symbol && explored[i][j+1] != 1)) {
            update(game_ptr,color,i,j+1,explored);
        }
    }
}

void world_update2(Game* game_ptr, char color) {
    char explored[SIZE][SIZE] = {0}; //explored[i][j]=1 when the cell (i,j) have already been updated
    if (game_ptr->current == A_PLAYING) {
            update(game_ptr,color,SIZE-1,0,explored);
    } else {
            update(game_ptr,color,0,SIZE-1,explored);
    }
}

void play_turn(Game* game_ptr) {
    char c;
    char color;
    char player_symbol = get_symbol(game_ptr);

    double rate_Aplayer = ((double) game_ptr->a_score/(double) (SIZE * SIZE)) * 100;
  	double rate_Bplayer = ((double) game_ptr->b_score/(double) (SIZE * SIZE)) * 100;
  	printf("\nPlayer * owns %.2f %% of the map", rate_Aplayer);
  	printf("\nPlayer ^ owns %.2f %% of the map", rate_Bplayer);
    if (game_ptr->game_mode == 0 || game_ptr->current == A_PLAYING) {
        	printf("\nWhich color ? (%c turn)\n",player_symbol);
          scanf("%c",&color);
          while((c = getchar()) != '\n' && c != EOF) {}
					while (color < 65 || color > 71) {
						printf("\n%c is not a valid color !\nWhich color ? (%c turn)\n",color,player_symbol);
	          scanf("%c",&color);
	          while((c = getchar()) != '\n' && c != EOF) {}
					}
        } else {
          color = ai_strategy(game_ptr);
        }
  	world_update2(game_ptr, color);
  	game_ptr->current = (game_ptr->current + 1) % 2;    //swap A_PLAYING and B_PLAYING

  	if (rate_Aplayer > 50) {
          game_ptr->current = A_WON;
  	}
  	if (rate_Bplayer > 50) {
          game_ptr->current = B_WON;
  	}
  	if (rate_Aplayer == rate_Bplayer && rate_Aplayer >= 50) {
          game_ptr->current = DRAW;
  	}
}

/** Program entry point */
int main(void) {
    srand(time(NULL));
    Game game = { init_board(), A_PLAYING, 1, 1, 3};

    while (game.current == A_PLAYING || game.current == B_PLAYING) {
        print_board(game.board);
        play_turn(&game);
    }

    print_board(game.board);

	if (game.current == DRAW) {
        printf("\n\nDraw\n");
	} else if (game.current == A_WON) {
        printf("\n\nPlayer * wins\n");
	} else {
        printf("\n\nPlayer ^ wins\n");
    }

    free_board(game.board);

	return 0;
}
