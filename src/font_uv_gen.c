#include <font_uv_gen.h>

float* font_gen_UVs(char* input, int size)
{
    static int i;
    static int twi;
    float* output = malloc(size*12*sizeof(float));
    static float x;
    static float y;
    const float od16 = 1/16.0;
    for (i = 0; i < size; i++)
    {
        twi = i*12;
        x = (input[i] % 16) / 16.0;
        y = (input[i] / 16) / 16.0;
        output[twi] = x;
        output[twi+1] = y;
        output[twi+2] = x;
        output[twi+3] = y + od16;
        output[twi+4] = x + od16;
        output[twi+5] = y + od16;

        output[twi+6] = x;
        output[twi+7] = y;
        output[twi+8] = x + od16;
        output[twi+9] = y;
        output[twi+10] = x + od16;
        output[twi+11] = y + od16;
    }
}