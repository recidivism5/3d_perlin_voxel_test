#version 150 core
in vec2 in_Position;
in vec2 in_Texcoord;
out vec2 Texcoord;
void main()
{
    Texcoord = in_Texcoord;
    gl_Position = vec4(in_Position, 0.0, 1.0);
}