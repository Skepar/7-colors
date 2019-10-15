#ifndef GAME_H
#define GAME_H

/**list of gamemodes :
*0 : player VS player
*1 : player VS AI random
*2 : player VS AI semi-random
*3 : player VS AI greedy */

typedef struct Game Game;
typedef enum Status Status;

enum Status { A_PLAYING, B_PLAYING, A_WON, B_WON, DRAW };

struct Game {
	char** board;
	Status current;
    int a_score;
    int b_score;
    double a_rate;
	double b_rate;
	char game_mode;
};

#endif
