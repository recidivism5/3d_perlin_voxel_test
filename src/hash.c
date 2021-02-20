#include <hash.h>
#include <math.h>
#include <stdio.h>

int hash_array[512] = {151,160,137,91,90,15,
    131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
    190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
    88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
    77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
    102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
    135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
    5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
    223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
    129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
    251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
    49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
    138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,

    151,160,137,91,90,15,
    131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
    190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
    88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
    77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
    102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
    135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
    5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
    223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
    129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
    251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
    49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
    138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

float perlin_gradient_vector_table[72] = {1,1,0,      -1,1,0,
                                        1,-1,0,     -1,-1,0,
                                        1,0,1,      -1,0,1,
                                        1,0,-1,     -1,0,-1,
                                        0,1,1,       0,-1,1,
                                        0,1,-1,      0,-1,-1,

                                        1,1,0,      -1,1,0,
                                        1,-1,0,     -1,-1,0,
                                        1,0,1,      -1,0,1,
                                        1,0,-1,     -1,0,-1,
                                        0,1,1,       0,-1,1,
                                        0,1,-1,      0,-1,-1};

float dot_product_3d(float* v1, float* v2)
{
    return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

void subtract_vec3(float* v1, float* v2, float* o)
{
    static float v1_copy[3];
    memcpy(&v1_copy, v1, sizeof(v1_copy)); 
    static float v2_copy[3];
    memcpy(&v2_copy, v2, sizeof(v2_copy));

    o[0] = v1_copy[0] - v2_copy[0];
    o[1] = v1_copy[1] - v2_copy[1];
    o[2] = v1_copy[2] - v2_copy[2];
}

float cube_corners[24] = {0,0,0,  1,0,0,  1,1,0,  0,1,0,
                        0,0,1,  1,0,1,  1,1,1,  0,1,1};

float fade(float a)
{
    return 6*a*a*a*a*a - 15*a*a*a*a + 10*a*a*a;
}

float gen3DPerlinValue(int i, int j, int k, float cube_size)
{
    static float n_i;
    static float n_j;
    static float n_k;
    
    static int cs_i;
    cs_i = (int)cube_size;

    n_i = (i % cs_i)/cube_size;
    n_j = (j % cs_i)/cube_size;
    n_k = (k % cs_i)/cube_size;

    static int vec_table_index;
    vec_table_index = (meme_hash((unsigned int)(i+j+k)) % 12) * 3;
    static float gradient_vectors[3*8];
    memcpy(gradient_vectors, &perlin_gradient_vector_table[vec_table_index], sizeof(gradient_vectors));
    
    static float normalized_position[3];
    normalized_position[0] = n_i;
    normalized_position[1] = n_j;
    normalized_position[2] = n_k;

    static float distance_vectors[3*8];
    for (int p = 0; p < 8; p++)
    {
        subtract_vec3(normalized_position, &cube_corners[p*3], &distance_vectors[p*3]);
    }

    static float dots[8];
    for (int p = 0; p < 8; p++)
    {
        dots[p] = dot_product_3d(&gradient_vectors[p*3], &distance_vectors[p*3]);
    }

    static float AB;
    static float CD;
    static float EF;
    static float GH;
    static float M;
    static float N;
    static float fn_i;
    static float fn_j;
    fn_i = fade(n_i);
    fn_j = fade(n_j);

    AB = dots[0] + fn_i*(dots[1]-dots[0]);
    CD = dots[2] + fn_i*(dots[3]-dots[2]);
    EF = dots[4] + fn_i*(dots[5]-dots[4]);
    GH = dots[6] + fn_i*(dots[7]-dots[6]);

    M = AB + fn_j*(CD-AB);
    N = EF + fn_j*(GH-EF);

    return (M + fade(n_k)*(N-M)) * 0.936f;
    

}

int meme_hash(unsigned int x) {
    if (x % 255 > 0) x = x % 255;
    return hash_array[x];
}