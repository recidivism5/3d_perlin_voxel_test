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
        1920, 1080,
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

    InitMatrices();

    return 0;
}

int InitOpenGL()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    InitShaderProgram(&shader_program_ids[0], "tutorial4");

    matrix_id = glGetUniformLocation(shader_program_ids[0], "MVP");
    
    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &color_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_color_data), cube_color_data, GL_STATIC_DRAW);
}

int InitShaderProgram(GLuint* program_id, char* name)
{

    vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    static char base_path[] = "../src/shaders/";
    static char end_path_vert[] = "_vert.glsl";
    static char end_path_frag[] = "_frag.glsl";

    char* final_path = malloc(sizeof(base_path)+sizeof(end_path_vert)+sizeof(name));

    // Compile vertex shader
    memcpy(&final_path[0], base_path, sizeof(base_path));
    memcpy(&final_path[15], name, sizeof(name)+1);
    memcpy(&final_path[16+sizeof(name)], end_path_vert, sizeof(end_path_vert));
    printf("\n building %s", final_path);

    char* vert_shader_src = read_shader_src(final_path);
    glShaderSource(vertex_shader_id, 1, &vert_shader_src, NULL);
    glCompileShader(vertex_shader_id);

    // Compile fragment shader
    memcpy(&final_path[16+sizeof(name)], end_path_frag, sizeof(end_path_frag));
    printf("\n building %s", final_path);
    printf("\n");

    char* frag_shader_src = read_shader_src(final_path);
    glShaderSource(fragment_shader_id, 1, &frag_shader_src, NULL);
    glCompileShader(fragment_shader_id);

    //Link to program
    *program_id = glCreateProgram();
    glAttachShader(*program_id, vertex_shader_id);
    glAttachShader(*program_id, fragment_shader_id);
    glLinkProgram(*program_id);

    glDetachShader(*program_id, vertex_shader_id);
    glDetachShader(*program_id, fragment_shader_id);

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    //Use program
    glUseProgram(*program_id);

    return 0;
}

void InitMatrices()
{
    float translation_temp[3] = {0.0, 0.0, 0.0};
    rot_matrix(1.0f, 0.0f, model_to_world);

    make_perspective_projection_matrix(default_fov_radians, default_aspect_ratio, 0.1f, 100.0f, perspective_proj);

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
    static float scale_factor = 40.0f;
    memcpy(scale_matrix, identity44, sizeof(identity44));
    scale_matrix[0] *= scale_factor;
    scale_matrix[5] *= scale_factor;
    scale_matrix[10] *= scale_factor;

    f_mult_mat44s(final_wtc, scale_matrix, final_matrix_2);
    f_mult_mat44s(perspective_proj, final_matrix_2, final_matrix_2);
    f_mult_mat44s(final_matrix_2, model_to_world, final_matrix_2);

}

/*
 * Render a frame
 */

int Update()
{

    update_mouse();
    move();

    UpdateMatrices();

    glClearColor(0.0f, 0.224f, 0.124f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader_program_ids[0]);

    glUniformMatrix4fv(matrix_id, 1, GL_TRUE, final_matrix);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_id);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glDrawArrays(GL_TRIANGLES, 0, 12*3);

    glUniformMatrix4fv(matrix_id, 1, GL_TRUE, final_matrix_2);

    glDrawArrays(GL_TRIANGLES, 0, 12*3);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    SDL_GL_SwapWindow(m_window);
    return 0;
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