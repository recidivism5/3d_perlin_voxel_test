#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>

char* read_shader_src(const char* path);
GLuint load_and_compile_shader(const char* path, GLenum shaderType);
GLuint create_program(const char* vert_shader_path, const char* frag_shader_path);