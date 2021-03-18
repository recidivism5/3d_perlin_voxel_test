#include <GL/glew.h>
#include <mtwister.h>

int world_seed;
MTRand rng;

float* world_vert_buffer;
float* world_uv_buffer;
float* world_normal_buffer;

GLuint world_uv_buffer_id;
int world_uv_array_size;

GLuint world_normal_buffer_id;
int world_normal_array_size;

void SeedWorld(int seed);