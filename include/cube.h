#include <GL/glew.h>

const GLfloat cube_vertices[108];
const GLfloat cube_normals[108];
const GLfloat cube_color_data[108];
const GLfloat cube_uv_data[72];

void translate_cube_vertices(float trans[3], float output[108]);