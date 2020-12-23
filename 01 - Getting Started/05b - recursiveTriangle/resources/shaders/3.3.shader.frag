#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;

uniform vec4 u_clearColor;
uniform float u_mixPct;
uniform sampler2D u_texture1;
uniform sampler2D u_texture2;

void main()
{   
    vec4 container = texture(u_texture1, TexCoord);
    vec4 smiley = texture(u_texture2, TexCoord);
    FragColor = mix(u_clearColor, container, u_mixPct);
    FragColor = mix(FragColor, smiley, smiley.a);
}