#ifndef GAME_H
#define GAME_H

/**list of gamemodes :
*0 : player VS player
*1 : player VS AI random
*2 : player VS AI semi-random
*3 : player VS AI greedy */

enum Status { A_PLAYING, B_PLAYING, A_WON, B_WON, DRAW };

struct Game {
	char** board;
	enum Status current;
    int a_score;
    int b_score;
	char game_mode;
	double a_rate;
	double b_rate;     // set to 1 if the second player is a computer, 0 otherwise
};

#endif
