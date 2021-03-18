#include <GL/glew.h>
#include <mtwister.h>

int world_seed;
MTRand rng;

float* world_vert_buffer;

void SeedWorld(int seed);