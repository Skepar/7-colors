#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define SIZE 30

void print_board(char board [SIZE][SIZE], int size) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%c", board[i][j]);
        }
        printf("\n");
    }
}

void init_board(char board[SIZE][SIZE], int size) {
	for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
			      board[i][j]=rand()%7+65;
        }
    }
    board[SIZE-1][0]='v';
    board[0][SIZE-1]='^';
}

/** Returns 1 if the cell (i,j) is adjacent to a cell possessed by player*/
int player_adjacent(char player_symbol,int i,int j, char board[SIZE][SIZE]) {
    if (i-1 >= 0) {
      if (board[i-1][j]==player_symbol) {
        return 1;
      }
    }
    if (i+1 < SIZE) {
      if (board[i+1][j]==player_symbol) {
        return 1;
      }
    }
    if (j-1 >= 0) {
      if (board[i][j-1]==player_symbol) {
        return 1;
      }
    }
    if (j+1 < SIZE) {
      if (board[i][j+1]==player_symbol) {
        return 1;
      }
    }
    return 0;
  }

char symbol(int player) {
  if (player == 0) {
    return 'v';
  } else {
    return '^';
  }
}

/**Updates the board when a player has played and chosen a color
*Note : The player is coded with an int. 0 is the bottom left player.
*1 is the upper right player.
*/
void world_update(int player, char color,char board[SIZE][SIZE])
{
    int modified = 0;
    char player_symbol = symbol(player);
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
          if (board[i][j] == color && player_adjacent(player_symbol,i,j, board)) {
            board[i][j]=player_symbol;
            modified = 1;
          }
        }
    }
    if (modified == 1) {
      world_update(player, color, board);
    }
}

double rate_symbol(char symbol, char board[SIZE][SIZE]) {
  int number_symbol = 0;
  double rate;
  for (int i = 0; i < SIZE; i++) {
      for (int j = 0; j < SIZE; j++) {
        if (board[i][j] == symbol) {
          number_symbol++;
        }
      }
    }
  rate = ( (double) number_symbol/ (double) (SIZE * SIZE) ) * 100;
  return rate;
}

int play_turn(int* player_pointer, char board[SIZE][SIZE]) {
  char c;
  char color;
  char player_symbol = symbol(*player_pointer);
  double rate_player_0;
  double rate_player_1;
  printf("\nWhich color ? (%c turn)\n",player_symbol);
  scanf("%c",&color);
  while((c = getchar()) != '\n' && c != EOF) {}
  for (int i = 0; i< 50;i++) {
    printf("\n");
  }
  world_update(*player_pointer,color,board);
  print_board(board, SIZE);
  *player_pointer = (*player_pointer + 1) % 2;
  rate_player_0 =rate_symbol('v',board);
  rate_player_1 =rate_symbol('^',board);
  printf("\nPlayer v owns %f %% of the map", rate_player_0);
  printf("\nPlayer ^ owns %f %% of the map", rate_player_1);
  if (rate_player_0 > 50) {
    return 0;
  }
  if (rate_player_1 > 50) {
    return 1;
  }
  if (rate_player_0 == rate_player_1 && rate_player_0 >= 50) {
    return 2;
  }
  play_turn(player_pointer,board);

}


/** Program entry point */
int main(void) {
	char board[SIZE][SIZE];
	srand(time(NULL));
	init_board(board, SIZE);
  print_board(board, SIZE);
  int player = 0;
  int* player_pointer = &player;
  int winner = play_turn(player_pointer,board);
  if (winner == 2) {
    printf("\n\nDraw\n");
  } else {
    printf("\n\nPlayer %c wins\n",symbol(winner));
  }
    return 0;
}
