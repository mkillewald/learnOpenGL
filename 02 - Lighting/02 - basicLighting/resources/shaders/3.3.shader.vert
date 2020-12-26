#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform vec3 lightPos; 

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;
out vec3 LightPos;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(view * model * vec4(aPos, 1.0));

    /*
     * Inversing matrices is a costly operation for shaders, so wherever possible try to avoid doing inverse operations 
     * since they have to be done on each vertex of your scene. For learning purposes this is fine, but for an efficient 
     * application you'll likely want to calculate the normal matrix on the CPU and send it to the shaders via a uniform 
     * before drawing (just like the model matrix).
     */

    // create normal matrix by taking the transpose and inverse of the upper-left 3x3 part of the model matrix 
    Normal = mat3(transpose(inverse(view *  model))) * aNormal;

    LightPos = vec3(view * vec4(lightPos, 1.0f)); // transform lightPos from world space to view space 
} 
