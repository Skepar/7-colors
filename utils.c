#include "utils.h"

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
