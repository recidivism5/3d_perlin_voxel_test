#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 tNormal;

// Output data ; will be interpolated for each fragment.
out vec2 UV;

out vec3 FragPos;
out vec3 Normal;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 model_to_world;

void main(){

    // Output position of the vertex, in clip space : MVP * position
    gl_Position =  MVP * vec4(vertexPosition_modelspace,1);

    // UV of the vertex. No special space for this one.
    UV = vertexUV;

    FragPos = vec3(model_to_world * vec4(vertexPosition_modelspace,1));
    Normal = vec3(model_to_world) * tNormal;
}