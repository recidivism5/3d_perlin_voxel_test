#include "quaternion.h"
#include <math.h>
#include <stdio.h>

float default_aspect_ratio;
float default_fov_radians;

float final_matrix[16];
float identity44[16];

void f_mult_mat44s(float* mat1, float* mat2, float* output);

float* lookAt(float cam_pos[3], float center[3], float up[3]);

void update_OTW(float object_rot_about_x, float object_rot_about_y, float object_translation[3]);
void update_WTC(float camera_rot_about_x, float camera_rot_about_y, float camera_position[3]);
void make_perspective_projection_matrix(float fov_radians, float aspect_ratio, float near, float far);
void update_FM();