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

float default_aspect_ratio = 4.0f/3.0f;
float default_fov_radians = 1.745329;

const float x_axis[3] = {1.0f, 0.0f, 0.0f};
const float y_axis[3] = {0.0f, 1.0f, 0.0f};
const float z_axis[3] = {0.0f, 0.0f, 1.0f};

float identity44[16] = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

float object_to_world[16];
float world_to_camera[16];
float perspective_proj[16] = {0.0f};

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

    //negate F
    F[0] = -F[0];
    F[1] = -F[1];
    F[2] = -F[2];

    static float output[16] = {0.0f};
    memcpy(&output[0], s, 3 * sizeof(float));
    memcpy(&output[4], u, 3 * sizeof(float));
    memcpy(&output[8], F, 3 * sizeof(float));
    output[15] = 1.0f;
    
    output[11] = -F_magnitude;

    return output;
}

void update_OTW(float object_rot_about_x, float object_rot_about_y, float object_translation[3])
{

    rotate(y_axis, x_axis, object_rot_about_x, &object_to_world[4]);
    rotate(z_axis, x_axis, object_rot_about_x, &object_to_world[8]);
    
    rotate(x_axis, y_axis, object_rot_about_y, &object_to_world[0]);
    rotate(&object_to_world[4], y_axis, object_rot_about_y, &object_to_world[4]);
    rotate(&object_to_world[8], y_axis, object_rot_about_y, &object_to_world[8]);

    memcpy(&object_to_world[12], object_translation, 3 * sizeof(float));

    object_to_world[3] = 0.0f;
    object_to_world[7] = 0.0f;
    object_to_world[11] = 0.0f;
    object_to_world[15] = 1.0f;

}

void update_WTC(float camera_rot_about_x, float camera_rot_about_y, float camera_position[3])
{
    rotate(y_axis, x_axis, -camera_rot_about_x, &world_to_camera[4]);
    rotate(z_axis, x_axis, -camera_rot_about_x, &world_to_camera[8]);
    
    rotate(x_axis, y_axis, -camera_rot_about_y, &world_to_camera[0]);
    rotate(&world_to_camera[4], y_axis, -camera_rot_about_y, &world_to_camera[4]);
    rotate(&world_to_camera[8], y_axis, -camera_rot_about_y, &world_to_camera[8]);

    camera_position[0] = -camera_position[0];
    camera_position[1] = -camera_position[1];
    camera_position[2] = -camera_position[2];

    memcpy(&world_to_camera[12], camera_position, 3 * sizeof(float));

    world_to_camera[3] = 0.0f;
    world_to_camera[7] = 0.0f;
    world_to_camera[11] = 0.0f;
    world_to_camera[15] = 1.0f;
}

void make_perspective_projection_matrix(float fov_radians, float aspect_ratio, float near, float far)
{
    float S = 1/tan(fov_radians/2);
    perspective_proj[0] = S;
    perspective_proj[5] = S;

    perspective_proj[10] = -far/(far-near);
    perspective_proj[11] = -1.0f;

    perspective_proj[14] = -(far*near)/(far-near);
}

void update_FM()
{
    f_mult_mat44s(object_to_world, world_to_camera, final_matrix);

    printf("m2w * w2c \n");
    for (int i = 0; i < 16; i++)
    {
        if (i % 4 == 0)
        {
            putchar('\n');
        }
        printf("%f", final_matrix[i]);
        putchar(' ');
    }

    f_mult_mat44s(final_matrix, perspective_proj, final_matrix);

    printf("\n m2w * w2c * persp \n");
    for (int i = 0; i < 16; i++)
    {
        if (i % 4 == 0)
        {
            putchar('\n');
        }
        printf("%f", final_matrix[i]);
        putchar(' ');
    }
}