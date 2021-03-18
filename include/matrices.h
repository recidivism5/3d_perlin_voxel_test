#include "quaternion.h"
#include <math.h>
#include <stdio.h>

float default_aspect_ratio;
float default_fov_radians;

const float x_axis[3];
const float y_axis[3];
const float z_axis[3];

float identity44[16];

void f_mult_mat44s(float* mat1, float* mat2, float* output);
void add_vec3(float vec1[3], float vec2[3], float output[3]);
void cross_product(float vec1[3], float vec2[3], float* output);

float* lookAt(float cam_pos[3], float center[3], float up[3]);

void rt_matrix(float object_rot_about_x, float object_rot_about_y, float object_translation[3], float* output);
void cmt_matrix(float i_offset, float j_offset, float k_offset, float* output);

void rot_matrix(float rot_about_x, float rot_about_y, float* output);
void column_major_trans_matrix(float translation[3], float* output);

void make_perspective_projection_matrix(float fov_radians, float aspect_ratio, float near, float far, float* output);