#include <gui.h>

float* font_gen_UVs(char* input, int size)
{
    static int i;
    static int twi;
    float* output = malloc(size*12*sizeof(float));
    static float x;
    static float y;
    const float od16 = 1.0f/16.0f;
    for (i = 0; i < size; i++)
    {
        twi = i*12;
        x = (input[i] % 16) / 16.0;
        y = (input[i] / 16) / 16.0;

        output[twi] = x + od16;
        output[twi+1] = y;
        output[twi+2] = x;
        output[twi+3] = y;
        output[twi+4] = x;
        output[twi+5] = y + od16;

        output[twi+6] = x;
        output[twi+7] = y + od16;
        output[twi+8] = x + od16;
        output[twi+9] = y + od16;
        output[twi+10] = x + od16;
        output[twi+11] = y;
    }
    return output;
}

UI_Element* element_init(float screen_pos_x, float screen_pos_y, float font_size, char* text, int text_length)
{
    UI_Element* e = malloc(sizeof(UI_Element));
    e->UVs = font_gen_UVs(text, text_length);
    e->verts = malloc(text_length * 18 * sizeof(float));
    e->verts_size = text_length * 18;
    e->UVs_size = text_length * 12;
    e->tri_count = text_length * 2;

    float og_screen_pos_x = screen_pos_x;
    int step;
    for (int i = 0; i < text_length; i++)
    {
        step = i * 18;
        //tri1
        e->verts[step]   = screen_pos_x + font_size;
        e->verts[step+1] = screen_pos_y;
        e->verts[step+2] = 0;

        e->verts[step+3] = screen_pos_x;
        e->verts[step+4] = screen_pos_y;
        e->verts[step+5] = 0;

        e->verts[step+6] = screen_pos_x;
        e->verts[step+7] = screen_pos_y - font_size;
        e->verts[step+8] = 0;

        //tri2
        e->verts[step+9]  = screen_pos_x;
        e->verts[step+10] = screen_pos_y - font_size;
        e->verts[step+11] = 0;

        e->verts[step+12] = screen_pos_x + font_size;
        e->verts[step+13] = screen_pos_y - font_size;
        e->verts[step+14] = 0;

        e->verts[step+15] = screen_pos_x + font_size;
        e->verts[step+16] = screen_pos_y;
        e->verts[step+17] = 0;

        screen_pos_x += font_size;
        if (screen_pos_x > 1.0f)
        {
            screen_pos_x = og_screen_pos_x;
            screen_pos_y -= font_size;
        }
    }
    return e;
}

void gui_init()
{
    char* text = "bruh";
    test_element = element_init(0.5, 0.5, 0.1, text, 4);

    glGenVertexArrays(1, &font_vao_id);
    glBindVertexArray(font_vao_id);

    glGenBuffers(1, &gui_test_vert_buffer_id);
    glGenBuffers(1, &gui_test_uv_buffer_id);

    glBindBuffer(GL_ARRAY_BUFFER, gui_test_vert_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, test_element->verts_size*sizeof(float), test_element->verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, gui_test_uv_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, test_element->UVs_size*sizeof(float), test_element->UVs, GL_STATIC_DRAW);

    //InitShaderProgram(&font_shader_program_id, "font");

}