#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

DynArray* new_dynarray(int init_size) {
	DynArray* res = malloc(sizeof(DynArray));
	
	res->size = init_size;
	res->last = 0;
	res->array = malloc(init_size*sizeof(Coord*));
	
	return res;
}

void add_coord(DynArray* dyn, char x, char y) {
	if (dyn->last < dyn->size-1) {
		Coord* c = malloc(sizeof(Coord));
		c->x = x;
		c->y = y;
		dyn->array[dyn->last] = c;
		dyn->last++;
	}
}

void remove_coord(DynArray* dyn, int n) {
	free(dyn->array[n]);
	dyn->array[n] = dyn->array[dyn->last-1];
	dyn->last--;
}

void free_dynarray(DynArray* dyn) {
	for (int i = 0; i < dyn->last; i++) {
		free(dyn->array[i]);
	}
	free(dyn->array);
	free(dyn);
}

DynArray* get_adjacent_coords(char** board, char i, char j, char size) {
	DynArray* res = new_dynarray(4);
	
	if (i-1 >= 0) add_coord(res, i-1, j);
	if (j-1 >= 0) add_coord(res, i, j-1);
	if (i+1 < size) add_coord(res, i+1, j);
	if (j+1 < size) add_coord(res, i, j+1);
	
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
