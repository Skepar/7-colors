#include <stdlib.h>
#include "utils.h"

AdjArray* new_array(int init_size) {
	AdjArray* res = malloc(sizeof(AdjArray));
	
	res->size = init_size;
	res->last = 0;
	res->array = malloc(init_size*sizeof(Coord*));
	
	return res;
}

void add_coord(AdjArray* adj, char x, char y) {
	if (adj->last <= adj->size-1) {
		Coord* c = malloc(sizeof(Coord));
		c->x = x;
		c->y = y;
		adj->array[adj->last] = c;
		adj->last++;
	}
}

void remove_coord(AdjArray* adj, int n) {
	free(adj->array[n]);
	adj->array[n] = adj->array[adj->last-1];
	adj->last--;
}

void free_array(AdjArray* adj) {
	for (int i = 0; i < adj->last; i++) {
		free(adj->array[i]);
	}
	free(adj->array);
	free(adj);
}

int get_x(AdjArray* adj, int i) {
	return (int) adj->array[i]->x;
}

int get_y(AdjArray* adj, int i) {
	return (int) adj->array[i]->y;
}

AdjArray* get_adjacent_coords(char** board, char i, char j, char size) {
	AdjArray* res = new_array(4);
	
	if (i-1 >= 0) { add_coord(res, i-1, j);}
	if (j-1 >= 0) { add_coord(res, i, j-1);}
	if (i+1 < size) { add_coord(res, i+1, j);}
	if (j+1 < size) { add_coord(res, i, j+1);}
	
	return res;
}

int sum(char* array, int size) {
    int sum = 0;
    for (int i=0; i<size; i++) {
        sum += array[i];
    }
    return sum;
}

int nth_occurence(int n, char c, char* array, int size) {
    for (int i=0; i<size; i++) {
        if (array[i] == c && n==0) {
            return i;
        } else if (array[i] == c) {
            n--;
        }
    }
    return -1;
}
