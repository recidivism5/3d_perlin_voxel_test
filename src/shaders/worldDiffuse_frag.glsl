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
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float invlen1 = clamp(20.0f/len1, 0.0, 3.23);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * invlen1;
    vec3 ambient = 0.2f * lightColor;
    color = (ambient + diffuse) * texture(myTextureSampler, UV).rgb;
    
}