#include <string.h>

void hamilton_product(const float* q, const float* r, float *output);
void make_quaternion(const float* axis, const float theta, float *output);
void rotate(const float* point, const float* axis, const float theta, float *output);