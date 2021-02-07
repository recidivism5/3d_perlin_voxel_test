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
  shader_buffer[filelen+1] = '\0';
  fread(shader_buffer, filelen, 1, stream);
  fclose(stream);
  return shader_buffer;
}

GLuint load_and_compile_shader(const char* path, GLenum shader_type)
{
  char* shader_buffer = read_shader_src(path);
  GLuint shader = glCreateShader(shader_type);
  glShaderSource(shader, 1, &shader_buffer, NULL);
  glCompileShader(shader);

  GLint test;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &test);
  if (!test)
  {
    printf("Shader compilation error: \n");
    char* compilation_log = (char*)malloc(512 * sizeof(char));
    glGetShaderInfoLog(shader, sizeof(compilation_log), NULL, &compilation_log);
    printf("%.*s\n", (int)sizeof(compilation_log), compilation_log);
  }

  return shader;

}

GLuint create_program(const char* path_vert_shader, const char* path_frag_shader)
{
  GLuint vertexShader = load_and_compile_shader(path_vert_shader, GL_VERTEX_SHADER);
  GLuint fragmentShader = load_and_compile_shader(path_frag_shader, GL_FRAGMENT_SHADER);

  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);

  return shaderProgram;
}