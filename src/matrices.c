#include "matrices.h"

void f_mult_mat44s(float* mat1_in, float* mat2_in, float* output){

    static float mat1[16];
    memcpy(mat1, mat1_in, sizeof(mat1));
    static float mat2[16];
    memcpy(mat2, mat2_in, sizeof(mat2));

    output[0] = mat1[0] * mat2[0] + mat1[1] * mat2[4] + mat1[2] * mat2[8] + mat1[3] * mat2[12];
    output[1] = mat1[0] * mat2[1] + mat1[1] * mat2[5] + mat1[2] * mat2[9] + mat1[3] * mat2[13];
    output[2] = mat1[0] * mat2[2] + mat1[1] * mat2[6] + mat1[2] * mat2[10] + mat1[3] * mat2[14];
    output[3] = mat1[0] * mat2[3] + mat1[1] * mat2[7] + mat1[2] * mat2[11] + mat1[3] * mat2[15];
    output[4] = mat1[4] * mat2[0] + mat1[5] * mat2[4] + mat1[6] * mat2[8] + mat1[7] * mat2[12];
    output[5] = mat1[4] * mat2[1] + mat1[5] * mat2[5] + mat1[6] * mat2[9] + mat1[7] * mat2[13];
    output[6] = mat1[4] * mat2[2] + mat1[5] * mat2[6] + mat1[6] * mat2[10] + mat1[7] * mat2[14];
    output[7] = mat1[4] * mat2[3] + mat1[5] * mat2[7] + mat1[6] * mat2[11] + mat1[7] * mat2[15];
    output[8] = mat1[8] * mat2[0] + mat1[9] * mat2[4] + mat1[10] * mat2[8] + mat1[11] * mat2[12];
    output[9] = mat1[8] * mat2[1] + mat1[9] * mat2[5] + mat1[10] * mat2[9] + mat1[11] * mat2[13];
    output[10] = mat1[8] * mat2[2] + mat1[9] * mat2[6] + mat1[10] * mat2[10] + mat1[11] * mat2[14];
    output[11] = mat1[8] * mat2[3] + mat1[9] * mat2[7] + mat1[10] * mat2[11] + mat1[11] * mat2[15];
    output[12] = mat1[12] * mat2[0] + mat1[13] * mat2[4] + mat1[14] * mat2[8] + mat1[15] * mat2[12];
    output[13] = mat1[12] * mat2[1] + mat1[13] * mat2[5] + mat1[14] * mat2[9] + mat1[15] * mat2[13];
    output[14] = mat1[12] * mat2[2] + mat1[13] * mat2[6] + mat1[14] * mat2[10] + mat1[15] * mat2[14];
    output[15] = mat1[12] * mat2[3] + mat1[13] * mat2[7] + mat1[14] * mat2[11] + mat1[15] * mat2[15];
}

float default_aspect_ratio = 16.0f/9.0f;
float default_fov_radians = 1.745329;

const float x_axis[3] = {1.0f, 0.0f, 0.0f};
const float y_axis[3] = {0.0f, 1.0f, 0.0f};
const float z_axis[3] = {0.0f, 0.0f, 1.0f};

float identity44[16] = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

void add_vec3(float vec1[3], float vec2[3], float output[3])
{
    output[0] = vec1[0] + vec2[0];
    output[1] = vec1[1] + vec2[1];
    output[2] = vec1[2] + vec2[2];
}

void cross_product(float vec1[3], float vec2[3], float* output)
{
    output[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
    output[1] = vec1[2] * vec2[0] - vec1[0] * vec2[2];
    output[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];
}

float* lookAt(float cam_pos[3], float center[3], float up[3])
{

    static float F[3];
    F[0] = center[0] - cam_pos[0];
    F[1] = center[1] - cam_pos[1];
    F[2] = center[2] - cam_pos[2];

    float F_magnitude = sqrt(F[0]*F[0] + F[1]*F[1] + F[2]*F[2]);
    F[0] = F[0] / F_magnitude;
    F[1] = F[1] / F_magnitude;
    F[2] = F[2] / F_magnitude;

    static float s[3];
    static float s_normalized[3];
    static float u[3];

    cross_product(F, up, s);
    float s_magnitude = sqrt(s[0]*s[0] + s[1]*s[1] + s[2]*s[2]);
    s_normalized[0] = s[0] / s_magnitude;
    s_normalized[1] = s[1] / s_magnitude;
    s_normalized[2] = s[2] / s_magnitude;

    cross_product(s_normalized, F, u);
    float u_magnitude = sqrt(u[0]*u[0] + u[1]*u[1] + u[2]*u[2]);

    //negate F
    F[0] = -F[0];
    F[1] = -F[1];
    F[2] = -F[2];

    static float output[16] = {0.0f};
    memcpy(&output[0], s_normalized, 3 * sizeof(float));
    memcpy(&output[4], u, 3 * sizeof(float));
    memcpy(&output[8], F, 3 * sizeof(float));
    output[15] = 1.0f;

    float translation_matrix[16];
    memcpy(translation_matrix, identity44, sizeof(identity44));
    translation_matrix[3] = -cam_pos[0];
    translation_matrix[7] = -cam_pos[1];
    translation_matrix[11] = -cam_pos[2];

    f_mult_mat44s(output, translation_matrix, output);

    return output;
}

void rt_matrix(float object_rot_about_x, float object_rot_about_y, float object_translation[3], float* output)
{

    rotate(y_axis, x_axis, object_rot_about_x, &output[4]);
    rotate(z_axis, x_axis, object_rot_about_x, &output[8]);
    
    rotate(x_axis, y_axis, object_rot_about_y, &output[0]);
    rotate(&output[4], y_axis, object_rot_about_y, &output[4]);
    rotate(&output[8], y_axis, object_rot_about_y, &output[8]);

    memcpy(&output[12], object_translation, 3 * sizeof(float));

    output[3] = 0.0f;
    output[7] = 0.0f;
    output[11] = 0.0f;
    output[15] = 1.0f;

}

void cmt_matrix(float i_offset, float j_offset, float k_offset, float* output)
{
    memcpy(output, identity44, sizeof(identity44));
    output[3] = i_offset;
    output[7] = j_offset;
    output[11] = k_offset;
}

void rot_matrix(float rot_about_x, float rot_about_y, float* output)
{
    memcpy(output, identity44, sizeof(identity44));
    rotate(y_axis, x_axis, rot_about_x, &output[4]);
    rotate(z_axis, x_axis, rot_about_x, &output[8]);
    
    rotate(x_axis, y_axis, rot_about_y, &output[0]);
    rotate(&output[4], y_axis, rot_about_y, &output[4]);
    rotate(&output[8], y_axis, rot_about_y, &output[8]);
}

void column_major_trans_matrix(float translation[3], float* output)
{
    memcpy(output, identity44, sizeof(identity44));
    output[3] = translation[0];
    output[7] = translation[1];
    output[11] = translation[2];
}

void make_perspective_projection_matrix(float fov_radians, float aspect_ratio, float near, float far, float* output)
{
    float S = 1/tan(fov_radians/2);
    output[0] = S/aspect_ratio;
    output[5] = S;

    output[10] = -far/(far-near);
    output[14] = -1.0f;

    output[11] = -(far*near)/(far-near) - near;
}