#include <GL/glew.h>

#include "shader.h"

typedef struct UI_Element {
    float* verts;
    float* UVs;
    int verts_size;
    int UVs_size;
} UI_Element;

typedef struct Paper {
    float* page_verts;
    float* text_verts;
    float* text_UVs;
} Paper;

UI_Element* test_element;
GLuint gui_test_vert_buffer_id;
GLuint gui_test_uv_buffer_id;

GLuint font_shader_program_id;
GLuint font_vao_id;

UI_Element* element_init(float screen_pos_x, float screen_pos_y, float font_size, char* text, int text_length);
float* font_gen_UVs(char* input, int size);