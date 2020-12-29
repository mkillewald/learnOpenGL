#version 330 core
uniform vec3 lightColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(lightColor, 1.0f); // set all 4 vector values to 1.0
}