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
*3 : player VS AI greedy
*4 : AI semi-random VS AI greedy */

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

char** init_board(char fair) {
    char** board = malloc(SIZE * sizeof(char*));

	for (int i = 0; i < SIZE; i++) {
		board[i] = malloc(SIZE * sizeof(char));
        if (fair) {
            for (int j = 0; j <= i; j++) {
                char rand_ch = rand()%7+65;
                board[i][j] = rand_ch;
                board[j][i] = rand_ch;
            }
		} else {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = rand()%7+65;
            }
        }
	}
	board[SIZE-1][0] = '*';
	board[0][SIZE-1] = '^';

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
    if (game_ptr->game_mode == 0 || (game_ptr->game_mode != 4 && game_ptr->current == A_PLAYING)) {
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
    game_ptr->a_rate = ((double) game_ptr->a_score/(double) (SIZE * SIZE)) * 100;
    game_ptr->b_rate = ((double) game_ptr->b_score/(double) (SIZE * SIZE)) * 100;
  	if (game_ptr->a_rate > 50) {
          game_ptr->current = A_WON;
  	}
  	if (game_ptr->b_rate > 50) {
          game_ptr->current = B_WON;
  	}
  	if (game_ptr->a_rate == game_ptr->b_rate && game_ptr->a_rate >= 50) {
          game_ptr->current = DRAW;
  	}
}

Status run(Game* game_ptr, char verbose) {
    while (game_ptr->current == A_PLAYING || game_ptr->current == B_PLAYING) {
        if (verbose) {
            print_board(game_ptr->board);
            printf("\nPlayer * owns %.2f %% of the map", game_ptr->a_rate);
            printf("\nPlayer ^ owns %.2f %% of the map", game_ptr->b_rate);
        }
        play_turn(game_ptr);
    }
    if (verbose) {
        print_board(game_ptr->board);

    	if (game_ptr->current == DRAW) {
            printf("\n\nDraw\n");
    	} else if (game_ptr->current == A_WON) {
            printf("\n\nPlayer * wins\n");
    	} else {
            printf("\n\nPlayer ^ wins\n");
        }
    }
    free_board(game_ptr->board);
    return game_ptr->current;
}

void run_n_times(int n) {
    int a_victories = 0;
    int b_victories = 0;
    Status result;

    for (int i = 0; i < n; i++) {
        Game game = { init_board(1), A_PLAYING, 1, 1, 3, 0, 0};
        result = run(&game, 0);
        if (result == A_WON) {
            a_victories++;
        } else {
            b_victories++;
        }
    }
    printf("\nA : %d\nB : %d\n", a_victories, b_victories);
}

/** Program entry point */
int main(void) {
    srand(time(NULL));
    //run_n_times(2000);
    Game game = { init_board(1), A_PLAYING, 1, 1, 5, 0, 0};
    run(&game,1);

	return 0;
}
