#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{ 
    FragColor = texture(screenTexture, TexCoords); // original 
    // FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0); // inversion

    // float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
    float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b; // weighted channels for more accurate results

    FragColor = vec4(average, average, average, 1.0); // greyscale
}