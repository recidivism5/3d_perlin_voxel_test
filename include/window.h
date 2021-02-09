#include <SDL2/SDL.h>
#include <GL/glew.h>

#include <stdio.h>

#include "shader.h"
#include "cube.h"
#include "matrices.h"

SDL_Window    *m_window;
SDL_GLContext  m_context;
GLuint         vertex_array_id, vertex_buffer_id, color_buffer_id;
GLuint         m_vert_shader;
GLuint         m_frag_shader;
GLuint         m_shader_prog;
GLuint         matrix_id;

extern const GLfloat verts[6][4];

extern const GLint indicies[];

int Initialize();
int Update();
int Cleanup();
int InitOpenGL();
int InitShaders();
void InitMatrices();
int InitTextures();