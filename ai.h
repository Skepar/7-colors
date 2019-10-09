#ifndef AI_H
#define AI_H

typedef struct Game Game;

char random_strategy(Game* game_ptr);
char semi_random_strategy(Game* game_ptr);
char greedy_strategy(Game* game_ptr);
char ai_strategy(Game* game_ptr);
char player_adjacent(Game* game_ptr,int i,int j);
char get_symbol(Game* game_ptr);

#endif
