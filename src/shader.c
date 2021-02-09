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