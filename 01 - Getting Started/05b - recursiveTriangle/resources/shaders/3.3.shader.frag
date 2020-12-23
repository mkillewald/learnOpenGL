#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;

uniform vec4 clearColor;
uniform float mixPct;
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{   vec4 container = texture(texture1, TexCoord);
    vec4 smiley = texture(texture2, TexCoord);
    FragColor = mix(clearColor, container, mixPct);
    FragColor = mix(FragColor, smiley, smiley.a);
}