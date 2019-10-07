#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define SIZE 30

typedef struct Game Game;
typedef enum Status Status;

enum Status { A_PLAYING, B_PLAYING, A_WON, B_WON, DRAW };

struct Game {
	char** board;
	Status current;
    int a_score;
    int b_score;
	char game_mode;     // set to 1 if the second player is a computer, 0 otherwise
};  

void print_board(char** board) {
    for (int i = 0; i < 50; i++) {
        printf("\n");
    }
    
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			printf("%c", board[i][j]);
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
	board[SIZE-1][0]='v';
	board[0][SIZE-1]='^';
    
    return board;
}

void free_board(char** board) {
    for (int i = 0; i < SIZE; i++) {
		free(board[i]);
	}
    free(board);
}

/** Returns 1 if the cell (i,j) is adjacent to a cell possessed by player*/
char player_adjacent(Game* game_ptr,int i,int j) {
    char player_symbol;
    
    if (game_ptr->current == A_PLAYING) {
        player_symbol = 'v';
    } else {
        player_symbol = '^';
    }

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

/**Updates the board when a player has played and chosen a color
*Note : The player is coded with an int. 0 is the bottom left player.
*1 is the upper right player.
*/
void world_update(Game* game_ptr, char color) {
	char player_symbol;
    char modified = 0;
    
    if (game_ptr->current == A_PLAYING) {
        player_symbol = 'v';
    } else {
        player_symbol = '^';
    }
    
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

void play_turn(Game* game_ptr) {
    char c;
	char color;
    char player_symbol;
    
    if (game_ptr->current == A_PLAYING) {
        player_symbol = 'v';
    } else {
        player_symbol = '^';
    }
    
    double rate_Aplayer = ((double) game_ptr->a_score/(double) (SIZE * SIZE)) * 100;
	double rate_Bplayer = ((double) game_ptr->b_score/(double) (SIZE * SIZE)) * 100;
	printf("\nPlayer v owns %f %% of the map", rate_Aplayer);
	printf("\nPlayer ^ owns %f %% of the map", rate_Bplayer);
    
	printf("\nWhich color ? (%c turn)\n",player_symbol);
	scanf("%c",&color);
	while((c = getchar()) != '\n' && c != EOF) {}
    
	world_update(game_ptr, color);
	game_ptr->current = (game_ptr->current + 1) % 2;    //swap A_PLAYING and B_PLAYING

	if (rate_Aplayer > 50) {
        game_ptr->current = A_WON;
	}
	if (rate_Bplayer > 50) {
        game_ptr->current = B_WON;
	}
	if (rate_Aplayer == rate_Aplayer && rate_Aplayer >= 50) {
        game_ptr->current = DRAW;
	}
}

/** Program entry point */
int main(void) {
    srand(time(NULL));
    Game game = { init_board(), B_PLAYING, 0, 0, 0 };
    
    while (game.current == A_PLAYING || game.current == B_PLAYING) {
        print_board(game.board);
        play_turn(&game);
    }
    
    print_board(game.board);
    
	if (game.current == DRAW) {
        printf("\n\nDraw\n");
	} else if (game.current == A_WON) {
        printf("\n\nPlayer v wins\n");
	} else {
        printf("\n\nPlayer ^ wins\n");
    }
    
    free_board(game.board);
    
	return 0;
}
