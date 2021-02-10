#include <SDL2/SDL.h>
#include <GL/glew.h>

#include <stdio.h>

#include "shader.h"
#include "cube.h"
#include "matrices.h"

SDL_Window    *m_window;
SDL_GLContext  m_context;
GLuint         vertex_array_id, vertex_buffer_id, color_buffer_id;
GLuint         vertex_shader_id;
GLuint         fragment_shader_id;
GLuint         shader_program_id;
GLuint         matrix_id;

float cam_pos[3];
float cam_rot_x;
float cam_rot_y;

float model_to_world[16];
float world_to_camera[16];
float perspective_proj[16];
float final_matrix[16];

int Initialize();
int Update();
int Cleanup();
int InitOpenGL();
int InitShaders();
void InitMatrices();
void UpdateMatrices();
int InitTextures();