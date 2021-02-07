#version 150 core
in vec2 Texcoord;
out vec4 out_Color;
uniform sampler2D tex;
void main()
{
    out_Color = texture(tex, Texcoord);
}