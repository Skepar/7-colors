#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "utils.h"
#include "ai.h"
#include "game.h"
#define SIZE 30

/**list of gamemodes :
*0 : player VS player
*1 : player VS AI random
*2 : player VS AI semi-random
*3 : player VS AI greedy
*4 : AI semi-random VS AI greedy
*5 : player VS foreseeing_greedy
*6 : greedy VS foreseeing_greedy
*7 : player VS perimeter based
*8 : greedy VS perimeter based*/

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
        // if fair is set to 1, the generated board is symmetrical
        if (fair) {
            for (int j = 0; j <= i; j++) {
                // A to G are coded in ASCII with the numbers 65 through 71
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

Game* init_game(char game_mode, char fair) {
    Game* res = malloc(sizeof(Game));

    res->board = init_board(fair);
    res->current = A_PLAYING;
    res->a_score = 1;
    res->b_score = 1;
    res->a_rate = 0.11;
    res->b_rate = 0.11;
    res->game_mode = game_mode;

    return res;
}

void free_game(Game* game_ptr) {
    free_board(game_ptr->board);
    free(game_ptr);
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

void update(Game* game_ptr, char color, int i, int j, char explored[SIZE][SIZE]) {
  	char player_symbol = get_symbol(game_ptr);
    explored[i][j] = 1;

    if (game_ptr->board[i][j] == color) {
        game_ptr->board[i][j] = player_symbol;

        if (game_ptr->current == A_PLAYING) {
            game_ptr->a_score++;
        } else {
            game_ptr->b_score++;
        }
    }

    AdjArray* adj_cells = get_adjacent_coords(game_ptr->board, i, j, SIZE);
    int x, y;

    for (int k = 0; k<adj_cells->last; k++) {
        x = get_x(adj_cells, k);
        y = get_y(adj_cells, k);

        if (game_ptr->board[x][y] == color || (game_ptr->board[x][y] == player_symbol && explored[x][y] != 1)) {
            update(game_ptr, color, x, y, explored);
        }
    }
    free_array(adj_cells);
}

void better_world_update(Game* game_ptr, char color) {
    char explored[SIZE][SIZE] = {0};        //explored[i][j]=1 when the cell (i,j) has already been updated

    if (game_ptr->current == A_PLAYING) {
        update(game_ptr, color, SIZE-1, 0, explored);
    } else {
        update(game_ptr, color, 0, SIZE-1, explored);
    }
}

void play_turn(Game* game_ptr) {
    char c;
    char color;
    char player_symbol = get_symbol(game_ptr);

    if (game_ptr->game_mode == 0 || (game_ptr->game_mode != 8 && game_ptr->game_mode != 6 && game_ptr->game_mode != 4 && game_ptr->current == A_PLAYING)) {
        do {
            printf("\nWhich color ? (%c turn)\n",player_symbol);
            scanf("%c",&color);
            while((c = getchar()) != '\n' && c != EOF) {} // buffer emptying magic
        }  while (color < 65 || color > 71);

    } else {
        color = ai_strategy(game_ptr);
    }

    better_world_update(game_ptr, color);

  	game_ptr->current = (game_ptr->current + 1) % 2;    //swaps A_PLAYING and B_PLAYING
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
    return game_ptr->current;
}

void run_n_times(int n, int gamemode) {
    int a_victories = 0;
    int b_victories = 0;
    Status result;

    time_t start = clock();
    for (int i = 0; i < n; i++) {
        Game* game_ptr = init_game(gamemode, 1);
        if (rand()%2 == 0) game_ptr->current = B_PLAYING;

        result = run(game_ptr, 0);
        if (result == A_WON) {
            a_victories++;
        } else if (result == B_WON) {
            b_victories++;
        }
        free_game(game_ptr);
    }
    time_t stop = clock();

    printf("\nA : %d\nB : %d\n", a_victories, b_victories);
    printf("in %f s\n", (double)(stop-start)/CLOCKS_PER_SEC);
}

/** Program entry point */
int main(void) {
    srand(time(NULL));
    printf("list of gamemodes :\n*0 : player VS player\n*1 : player VS AI random\n*2 : player VS AI semi-random\n*3 : player VS AI greedy\n");
    printf("*4 : AI semi-random (A) VS AI greedy (B)\n*5 : player VS foreseeing_greedy\n*6 : greedy (A) VS foreseeing_greedy (B)\n");
    printf("*7 : player VS perimeter based\n*8 : greedy (A) VS perimeter based (B)\n");
    int gamemode;
    char c;
    scanf("%d",&gamemode);
    while((c = getchar()) != '\n' && c != EOF) {}
    if (gamemode == 4 || gamemode == 6 || gamemode == 8) { //Gamemodes where the human don't play
        run_n_times(100,gamemode);
    } else {
        Game* game_ptr = init_game(gamemode, 1);
        run(game_ptr, 1);
        free_game(game_ptr);
    }
	return 0;
}
