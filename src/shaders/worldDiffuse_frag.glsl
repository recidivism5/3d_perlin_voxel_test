#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;

in vec3 Normal;
in vec3 FragPos;
uniform vec3 lightColor;
uniform vec3 lightPos;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;

void main(){
    float len1 = length(lightPos - FragPos);
    if (len1 < 50)
    {
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float invlen1 = 2.0f/len1;
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor * invlen1;
        vec3 ambient = 0.2f * lightColor;
        color = (ambient + diffuse) * texture(myTextureSampler, UV).rgb;
    }
    else
    {
        color = vec3(0.0,0.0,0.0);
    }
}