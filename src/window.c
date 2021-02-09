#include "window.h"

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
        640, 480,
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

    InitShaders();

    matrix_id = glGetUniformLocation(shader_program_id, "MVP");
    
    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &color_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_color_data), cube_color_data, GL_STATIC_DRAW);
}

int InitShaders()
{

    vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    // Compile vertex shader
    char* vert_shader_src = read_shader_src("../src/shaders/vertex.glsl");
    glShaderSource(vertex_shader_id, 1, &vert_shader_src, NULL);
    glCompileShader(vertex_shader_id);

    // Compile fragment shader
    char* frag_shader_src = read_shader_src("../src/shaders/frag.glsl");
    glShaderSource(fragment_shader_id, 1, &frag_shader_src, NULL);
    glCompileShader(fragment_shader_id);

    //Link to program
    shader_program_id = glCreateProgram();
    glAttachShader(shader_program_id, vertex_shader_id);
    glAttachShader(shader_program_id, fragment_shader_id);
    glLinkProgram(shader_program_id);

    glDetachShader(shader_program_id, vertex_shader_id);
    glDetachShader(shader_program_id, fragment_shader_id);

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    //Use program
    glUseProgram(shader_program_id);

    return 0;
}

void InitMatrices()
{
    float cube_center[3] = {0.0f, 0.0f, 0.0f};
    float cam_center[3] = {0.0f, 0.0f, -3.0f};

    update_OTW(0.0f, 0.0f, cube_center);
    
    make_perspective_projection_matrix(default_fov_radians, default_aspect_ratio, 0.1f, 100.0f);

    

    float cam_pos[3] = {4.0,3.0,-3.0};
    float origin[3] = {0.0, 0.0, 0.0};
    float temp_y_axis[3] = {0.0, 1.0, 0.0};
    float* view = lookAt(cam_pos, origin, temp_y_axis);
    memcpy(world_to_camera, view, 16 * sizeof(float));

    printf("\n view: \n");
    for (int i = 0; i < 16; i++)
    {
        if (i % 4 == 0)
        {
            putchar('\n');
        }
        printf("%f", view[i]);
        putchar(' ');
    }

    update_FM();

}

/*
 * Render a frame
 */

GLfloat test_matrix[16] = {-0.431713, 0.000000, -0.575618, 0.000000, 
                            -0.345371, 0.719522, 0.259028, 0.000000, 
                            -0.686681, -0.515011, 0.515011, 5.736689, 
                            -0.685994, -0.514496, 0.514496, 5.830952};
int Update()
{
    glClearColor(0.0f, 0.224f, 0.124f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader_program_id);

    glUniformMatrix4fv(matrix_id, 1, GL_TRUE, final_matrix);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_id);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

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
    glDeleteProgram(shader_program_id);
    glDeleteVertexArrays(1, &vertex_array_id);

    SDL_GL_DeleteContext(m_context);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
    return 0;
}