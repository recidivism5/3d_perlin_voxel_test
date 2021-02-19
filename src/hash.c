#include <hash.h>
#include <math.h>

int meme_hash(unsigned int x) {
    if (sin(sin(x % 3)+x) > 0.5f && cos(x%2) > 0.3f) return 1;
    else return 0;
}