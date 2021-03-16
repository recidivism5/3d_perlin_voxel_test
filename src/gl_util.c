#include "gl_util.h"

int util_setVec3(GLuint shader_program_id, const char* name, float* vec)
{
    glUniform3fv(glGetUniformLocation(shader_program_id, name), 1, vec);
}