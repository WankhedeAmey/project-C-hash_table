#include <math.h>

#include "prime.h"

/*
    * Checking if a nummber is prime or not. Also a function implemented to get the next prime greate than or equal to x 
*/
int is_prime(const int x) {
    if (x < 2) return -1;
    if (x < 4) return 1;
    if ((x % 2) == 0) return 0;

    for (int i = 3; i <= floor(sqrt((double) x)); i ++) {
        if ((x % i) == 0)
            return 0;
    }

    return 1;
}

int next_prime(int x) {
    while (is_prime(x) != 1) {
        x ++;
    }
    return x;
}