#include "window.h"

float cam_pos[3] = {0.0f, 0.0f, -3.0f};
float cam_rot_x = 0.0f;
float cam_rot_y = 0.0f;

int mouse_x = 0;
int mouse_y = 0;
const float MOUSE_MOTION_TO_THETA_RATE = 0.001;
const float SPEED = 0.1;
void update_mouse()
{
    static int prev_mouse_x = 0;
    static int prev_mouse_y = 0;
    static int delta_mouse_x = 0;
    static int delta_mouse_y = 0;
    static int start_flag = 0;

    SDL_GetRelativeMouseState(&mouse_x, &mouse_y);
    if (start_flag)
    {
        delta_mouse_x = mouse_x - prev_mouse_x;
        delta_mouse_y = mouse_y - prev_mouse_y;
    }
    else
    {
        start_flag = 1;
    }
    
    SDL_GetRelativeMouseState(&prev_mouse_x, &prev_mouse_y);

    //update camera angle:
    cam_rot_y -= MOUSE_MOTION_TO_THETA_RATE * delta_mouse_x;
    cam_rot_x -= MOUSE_MOTION_TO_THETA_RATE * delta_mouse_y;

}

void move()
{
    if (move_direction_z == 1)
    {
        cam_pos[0] += SPEED * final_wtc[8];
        cam_pos[1] += SPEED * final_wtc[9];
        cam_pos[2] += SPEED * final_wtc[10];
    }
    else if (move_direction_z == -1)
    {
        cam_pos[0] -= SPEED * final_wtc[8];
        cam_pos[1] -= SPEED * final_wtc[9];
        cam_pos[2] -= SPEED * final_wtc[10];
    }
    if (move_direction_x == 1)
    {
        cam_pos[0] -= SPEED * final_wtc[0];
        cam_pos[1] -= SPEED * final_wtc[1];
        cam_pos[2] -= SPEED * final_wtc[2];
    }
    else if (move_direction_x == -1)
    {
        cam_pos[0] += SPEED * final_wtc[0];
        cam_pos[1] += SPEED * final_wtc[1];
        cam_pos[2] += SPEED * final_wtc[2];
    }
}

int Initialize()
{
    // Initialize SDL Video
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Failed to initialize SDL video\n");
        return 1;
    }

    // Create main window
    m_window = SDL_CreateWindow(
        "SDL App",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        960, 540,
        SDL_WINDOW_OPENGL);
    if (m_window == NULL) {
        fprintf(stderr, "Failed to create main window\n");
        SDL_Quit();
        return 1;
    }

    // Initialize rendering context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK,
        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    m_context = SDL_GL_CreateContext(m_window);
    if (m_context == NULL) {
        fprintf(stderr, "Failed to create GL context\n");
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return 1;
    }

    SDL_GL_SetSwapInterval(1); // Use VSYNC

    // Initialize GL Extension Wrangler (GLEW)
    GLenum err;
    glewExperimental = GL_TRUE; // Please expose OpenGL 3.x+ interfaces
    err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Failed to init GLEW\n");
        SDL_GL_DeleteContext(m_context);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return 1;
    }

    InitOpenGL();

    InitTextures();

    InitMatrices();

    SeedWorld(4);

    world_init();

    pthread_t client_thread;
    pthread_create(&client_thread, NULL, start_client, NULL);

    return 0;
}

int InitOpenGL()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    InitShaderProgram(&shader_program_ids[0], "tutorial4");
    matrix_ids[0] = glGetUniformLocation(shader_program_ids[0], "MVP");

    InitShaderProgram(&shader_program_ids[1], "tutorial5");
    matrix_ids[1] = glGetUniformLocation(shader_program_ids[1], "MVP");

    InitShaderProgram(&shader_program_ids[SHADER_DIFFUSE], "diffuse");
    matrix_ids[SHADER_DIFFUSE] = glGetUniformLocation(shader_program_ids[SHADER_DIFFUSE], "MVP");

    InitShaderProgram(&shader_program_ids[SHADER_WD], "worldDiffuse");
    matrix_ids[SHADER_WD] = glGetUniformLocation(shader_program_ids[SHADER_WD], "MVP");

    lightPos_id = glGetUniformLocation(shader_program_ids[SHADER_DIFFUSE], "lightPos");
    
    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &color_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_color_data), cube_color_data, GL_STATIC_DRAW);

    glGenBuffers(1, &triNormals_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, triNormals_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_normals), cube_normals, GL_STATIC_DRAW);
}

int InitTextures()
{
    textures[0] = loadDDS("../res/tex/results/sand_.DDS");
	
	texture_ids[0] = glGetUniformLocation(shader_program_ids[1], "myTextureSampler");
    texture_ids[1] = glGetUniformLocation(shader_program_ids[SHADER_DIFFUSE], "myTextureSampler");
    texture_ids[2] = glGetUniformLocation(shader_program_ids[SHADER_WD], "myTextureSampler");

    glGenBuffers(1, &uv_buffers[0]);
    glBindBuffer(GL_ARRAY_BUFFER, uv_buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_uv_data), cube_uv_data, GL_STATIC_DRAW);
}

void InitMatrices()
{
    float translation_temp[3] = {0.0, 0.0, 0.0};

    rot_matrix(0.0f, 0.0f, model_to_world);

    make_perspective_projection_matrix(default_fov_radians, default_aspect_ratio, 0.1f, 500.0f, perspective_proj);

    float target[3] = {1.0, 0.0, 0.0};
    float temp_y_axis[3] = {0.0, 1.0, 0.0};
    view = lookAt(cam_pos, target, temp_y_axis);

}

void UpdateMatrices()
{
    
    static float camera_rotation_matrix[16];
    rot_matrix(cam_rot_x, cam_rot_y, camera_rotation_matrix);
    static float camera_column_trans[16];
    column_major_trans_matrix(cam_pos, camera_column_trans);
    
    f_mult_mat44s(camera_rotation_matrix, camera_column_trans, final_wtc);
    f_mult_mat44s(perspective_proj, final_wtc, final_matrix);

    static float scale_matrix[16];
    static float scale_factor;
    scale_factor = 1.0f;
    memcpy(scale_matrix, identity44, sizeof(identity44));
    scale_matrix[0] *= scale_factor;
    scale_matrix[5] *= scale_factor;
    scale_matrix[10] *= scale_factor;

    static float test_trans[16];
    cmt_matrix(0.0f, 0.0f, 0.0f, test_trans);
    f_mult_mat44s(final_wtc, scale_matrix, final_matrix_2);
    f_mult_mat44s(perspective_proj, final_matrix_2, final_matrix_2);
    f_mult_mat44s(final_matrix_2, test_trans, final_matrix_2);

}

void update_client_position()
{
    memcpy(&own_position, &cam_pos, sizeof(own_position));
}

void friend_draw()
{
    glUseProgram(shader_program_ids[SHADER_DIFFUSE]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glUniform1i(texture_ids[1], 0);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uv_buffers[0]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
    
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, triNormals_buffer_id);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

    static float lightPos[3] = {2,3,5};
    glUniform3fv(lightPos_id, 1, lightPos);
    static float lightColor[3] = {1.0f, 0.5f, 0.31f};
    glUniform3fv(glGetUniformLocation(shader_program_ids[SHADER_DIFFUSE], "lightColor"), 1, lightColor);
    
    static float friend_translation_mat[16];
    static float friend_rotation_mat[16];
    static float friend_rot_x = 2.0f;
    static float friend_rot_y = 1.5f;
    if (friend_rot_x < 2*3.14)
    {
        friend_rot_x += 0.01f;
        friend_position[2] += 0.01f;
    }
    else
    {
        friend_rot_x = 0.0f;
        friend_position[2] = 0.0f;
    }
    if (friend_rot_y < 2*3.14)
    {
        friend_rot_y += 0.01f;
    }
    else
    {
        friend_rot_y = 0.0f;
    }
    static float friend_model_to_world[16];
    cmt_matrix(-friend_position[0], -friend_position[1], -friend_position[2], friend_translation_mat);
    rot_matrix(friend_rot_x, friend_rot_y, friend_rotation_mat);
    f_mult_mat44s(friend_translation_mat, friend_rotation_mat, friend_model_to_world);

    static float friend_draw_matrix[16];
    f_mult_mat44s(final_matrix, friend_model_to_world, friend_draw_matrix);
    
    glUniformMatrix4fv(glGetUniformLocation(shader_program_ids[SHADER_DIFFUSE], "model_to_world"), 1, GL_TRUE, friend_model_to_world);
    glUniformMatrix4fv(matrix_ids[SHADER_DIFFUSE], 1, GL_TRUE, friend_draw_matrix);
    lightPos[0] = -cam_pos[0];
    lightPos[1] = -cam_pos[1];
    lightPos[2] = -cam_pos[2];
    glDrawArrays(GL_TRIANGLES, 0, 12*3);

    glDisableVertexAttribArray(2);
}

//per frame
int Update()
{
    update_client_position();

    update_mouse();
    move();

    UpdateMatrices();

    glClearColor(0.0f, 0.224f, 0.124f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //JOJ region:

    world_draw();
    friend_draw();

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    SDL_GL_SwapWindow(m_window);
    return 0;
}

int perlin_period = 4;
float perlin_cutoff = 0.2f;

int perlinTest(float i, float j, float k, float gi, float gj, float gk)
{
    if (i < 0 || j < 0 || k < 0 || i > 31 || j > 31 || k > 31)
    {return 0;}

    if (noise3(i/perlin_period + gi*32, j/perlin_period + gj*32, k/perlin_period + gk*32) > perlin_cutoff)
    { return 1;}
    else
    { return 0;}
}

int toggle_full_draw = -1;

int world_size = 1;
int* world;
int world_array_size;
int world_total_blocks = 0;
int world_vert_array_size = 0;

float i_shift = 0;

void world_init()
{
    static int resetFlag = 0;
    if (resetFlag)
    {
        free(world);
        free(world_vert_buffer);
        free(world_uv_buffer);
        free(world_normal_buffer);
    }

    world = malloc(32*32*32*world_size*world_size*world_size*sizeof(int));
    world_array_size = 32*32*32*world_size*world_size*world_size;

    for (float gi = 0; gi < world_size; gi++)
    {
        for (float gj = 0; gj < world_size; gj++)
        {
            for (float gk = 0; gk < world_size; gk++)
            {
                for (float k = 0; k < 32; k++)
                {
                    for (float j = 0; j < 32; j++)
                    {
                        for (float i = 0; i < 32; i++)
                        {   
                            if (noise3(i/perlin_period + gi*32 + i_shift, j/perlin_period + gj*32, k/perlin_period + gk*32) > perlin_cutoff)
                            {
                                world[world_size*world_size*32*32*32*(int)gk + world_size*32*32*32*(int)gj + 32*32*32*(int)gi + 32*32*(int)k + 32*(int)j + (int)i] = 1;
                                world_total_blocks += 1;
                            }
                            else
                            {
                                world[world_size*world_size*32*32*32*(int)gk + world_size*32*32*32*(int)gj + 32*32*32*(int)gi + 32*32*(int)k + 32*(int)j + (int)i] = 0;
                            }
                        }
                    }
                }
            }
        }
    }

    world_fill_vert_buffer();

    if (!resetFlag)
    {
        glGenBuffers(1, &world_vertex_buffer_id);
        glGenBuffers(1, &world_uv_buffer_id);
        glGenBuffers(1, &world_normal_buffer_id);
    }

    glBindBuffer(GL_ARRAY_BUFFER, world_vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, world_vert_array_size*sizeof(float), world_vert_buffer, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, world_uv_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, world_uv_array_size*sizeof(float), world_uv_buffer, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, world_normal_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, world_normal_array_size*sizeof(float), world_normal_buffer, GL_STATIC_DRAW);
    
    resetFlag = 1;
}

int check_cell(int index)
{
    if (index < 0 || index > world_array_size-1)
    {
        return 0;
    }
    else
    {
        return world[index];
    }
}

void world_draw()
{
    glUseProgram(shader_program_ids[SHADER_WD]);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, world_vertex_buffer_id);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, world_uv_buffer_id);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, world_normal_buffer_id);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

    static float lightPos[3] = {2.0f,3.0f,5.0f};
    static float lightColor[3] = {0.31f, 0.5f, 1.0f};
    lightPos[0] = -cam_pos[0];
    lightPos[1] = -cam_pos[1];
    lightPos[2] = -cam_pos[2];

    glUniform3fv(glGetUniformLocation(shader_program_ids[SHADER_WD], "lightColor"), 1, lightColor);
    glUniform3fv(glGetUniformLocation(shader_program_ids[SHADER_WD], "lightPos"), 1, lightPos);

    glUniformMatrix4fv(matrix_ids[SHADER_WD], 1, GL_TRUE, final_matrix_2);
    glDrawArrays(GL_TRIANGLES, 0, world_array_size*3*12);
}

void world_fill_vert_buffer()
{
    world_vert_array_size = world_total_blocks*3*3*12;
    world_uv_array_size = 2*(world_vert_array_size/3);
    world_normal_array_size = world_vert_array_size;

    world_vert_buffer = malloc(world_vert_array_size*sizeof(float));
    world_uv_buffer = malloc(2*(world_vert_array_size/3)*sizeof(float));
    world_normal_buffer = malloc(world_vert_array_size*sizeof(float));
    static long int world_vert_array_index;
    world_vert_array_index = 0;
    static long int world_uv_array_index;
    world_uv_array_index = 0;

    for (float gi = 0; gi < world_size; gi++)
    {
        for (float gj = 0; gj < world_size; gj++)
        {
            for (float gk = 0; gk < world_size; gk++)
            {
                for (float k = 0; k < 32; k++)
                {
                    for (float j = 0; j < 32; j++)
                    {
                        for (float i = 0; i < 32; i++)
                        {   
                            static int index;
                            index = world_size*world_size*32*32*32*(int)gk + world_size*32*32*32*(int)gj + 32*32*32*(int)gi + 32*32*(int)k + 32*(int)j + (int)i;
                            if (world[index])
                            {

                                static float c_trans[3];
                                c_trans[0] = gi*32 + i;
                                c_trans[1] = gj*32 + j;
                                c_trans[2] = gk*32 + k;

                                static float v_translated[108];
                                translate_cube_vertices(c_trans, v_translated);
                                
                                if (!check_cell(index-1) || i == 0)
                                {
                                    memcpy(&world_vert_buffer[world_vert_array_index], &v_translated[90], 18*sizeof(float));
                                    memcpy(&world_normal_buffer[world_vert_array_index], &cube_normals[90], 18*sizeof(float));
                                    memcpy(&world_uv_buffer[world_uv_array_index], &cube_uv_data[60], 12*sizeof(float));

                                    world_vert_array_index += 18;
                                    world_uv_array_index += 12;
                                }

                                if (!check_cell(index+1) || i == 31)
                                {
                                    memcpy(&world_vert_buffer[world_vert_array_index], &v_translated[72], 18*sizeof(float));
                                    memcpy(&world_normal_buffer[world_vert_array_index], &cube_normals[72], 18*sizeof(float));
                                    memcpy(&world_uv_buffer[world_uv_array_index], &cube_uv_data[48], 12*sizeof(float));

                                    world_vert_array_index += 18;
                                    world_uv_array_index += 12;
                                }

                                if (!check_cell(index-32) || j == 0)
                                {
                                    memcpy(&world_vert_buffer[world_vert_array_index], &v_translated[54], 18*sizeof(float));
                                    memcpy(&world_normal_buffer[world_vert_array_index], &cube_normals[54], 18*sizeof(float));
                                    memcpy(&world_uv_buffer[world_uv_array_index], &cube_uv_data[36], 12*sizeof(float));

                                    world_vert_array_index += 18;
                                    world_uv_array_index += 12;
                                }

                                if (!check_cell(index+32) || j == 31)
                                {
                                    memcpy(&world_vert_buffer[world_vert_array_index], &v_translated[18], 18*sizeof(float));
                                    memcpy(&world_normal_buffer[world_vert_array_index], &cube_normals[18], 18*sizeof(float));
                                    memcpy(&world_uv_buffer[world_uv_array_index], &cube_uv_data[12], 12*sizeof(float));

                                    world_vert_array_index += 18;
                                    world_uv_array_index += 12;
                                }

                                if (!check_cell(index-1024) || k == 0)
                                {
                                    memcpy(&world_vert_buffer[world_vert_array_index], &v_translated[0], 18*sizeof(float));
                                    memcpy(&world_normal_buffer[world_vert_array_index], &cube_normals[0], 18*sizeof(float));
                                    memcpy(&world_uv_buffer[world_uv_array_index], &cube_uv_data[0], 12*sizeof(float));

                                    world_vert_array_index += 18;
                                    world_uv_array_index += 12;
                                }

                                if (!check_cell(index+1024) || k == 31)
                                {
                                    memcpy(&world_vert_buffer[world_vert_array_index], &v_translated[36], 18*sizeof(float));
                                    memcpy(&world_normal_buffer[world_vert_array_index], &cube_normals[36], 18*sizeof(float));
                                    memcpy(&world_uv_buffer[world_uv_array_index], &cube_uv_data[24], 12*sizeof(float));

                                    world_vert_array_index += 18;
                                    world_uv_array_index += 12;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

int Cleanup()
{
    glDeleteBuffers(1, &vertex_buffer_id);
    glDeleteBuffers(1, &color_buffer_id);
    glDeleteProgram(shader_program_ids[0]);
    glDeleteVertexArrays(1, &vertex_array_id);

    SDL_GL_DeleteContext(m_context);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
    return 0;
}