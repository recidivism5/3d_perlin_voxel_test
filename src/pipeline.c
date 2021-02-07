#include "pipeline.h"

void init_vao(GLuint* vao)
{
  glGenVertexArrays(1, vao);
  glBindVertexArray(*vao);
}