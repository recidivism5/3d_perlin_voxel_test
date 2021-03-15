#include "shader.h"

char* read_shader_src(const char* path)
{
  static FILE* stream;
  static long filelen;
  stream = fopen(path, "rb");
  fseek(stream, 0, SEEK_END);
  filelen = ftell(stream);
  rewind(stream);
  char* shader_buffer = (char*)malloc((filelen + 1) * sizeof(char));
  shader_buffer[filelen] = '\0';
  fread(shader_buffer, filelen, 1, stream);
  fclose(stream);
  return shader_buffer;
}

int InitShaderProgram(GLuint* program_id, char* name)
{

    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    static char base_path[] = "../src/shaders/";
    static char end_path_vert[] = "_vert.glsl";
    static char end_path_frag[] = "_frag.glsl";

    char* final_path = malloc(sizeof(base_path)+sizeof(end_path_vert)+sizeof(name));

    // Compile vertex shader
    memcpy(&final_path[0], base_path, sizeof(base_path));
    memcpy(&final_path[15], name, strlen(name));
    memcpy(&final_path[15+strlen(name)], end_path_vert, sizeof(end_path_vert));
    printf("\n building %s", final_path);

    char* vert_shader_src = read_shader_src(final_path);
    glShaderSource(vertex_shader_id, 1, &vert_shader_src, NULL);
    glCompileShader(vertex_shader_id);

    // Compile fragment shader
    memcpy(&final_path[15+strlen(name)], end_path_frag, sizeof(end_path_frag));
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
    //glUseProgram(*program_id);

    return 0;
}