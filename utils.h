#ifndef UTILS_H
#define UTILS_H

typedef struct Coord Coord;
typedef struct AdjArray AdjArray;

struct Coord {
	char x;
	char y;
};

struct AdjArray {
	Coord** array;
	int size;
	int last;
};

AdjArray* new_array(int init_size);
void add_coord(AdjArray* adj, char x, char y);
void remove_coord(AdjArray* adj, int n);
void free_array(AdjArray* adj);
int get_x(AdjArray* adj, int i);
int get_y(AdjArray* adj, int i);
AdjArray* get_adjacent_coords(char** board, char i, char j, char size);

int sum(char* array, int size);
int nth_occurence(int n, char c, char* array, int size);

#endif
