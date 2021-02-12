#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* read_shader_src(const char* path);
int InitShaderProgram(GLuint* program_id, char* name);