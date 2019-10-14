#ifndef UTILS_H
#define UTILS_H

typedef struct Coord Coord;
typedef struct DynArray DynArray;

struct Coord {
	char x;
	char y;
};

struct DynArray {
	Coord** array;
	int size;
	int last;
};

DynArray* new_dynarray(int init_size);
void add_coord(DynArray* dyn, char x, char y);
void remove_coord(DynArray* dyn, int n);
void free_dynarray(DynArray* dyn);
char get_x(DynArray* dyn, int n);
char get_y(DynArray* dyn, int n);
DynArray* get_adjacent_coords(char** board, char i, char j, char size);

int sum(char* array, int size);
int nth_occurence(int n, char c, char* array, int size);

#endif
