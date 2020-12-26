#version 330 core
out vec4 FragColor;
  
uniform vec3 objectColor;
uniform vec3 lightColor;

in vec3 FragPos;
in vec3 Normal; 
in vec3 LightPos;

void main()
{
    // create ambient lighting effect
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);

    // create diffuse lighting effect
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff) * lightColor;

    // create spcular lighting effect
    float specularStrength = 0.5;
    vec3 viewDir = normalize(-FragPos); // viewer is at 0,0,0 in view space so we can just negate FragPos
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
    vec3 specular = specularStrength * spec * lightColor;  

    // combine all three effects and apply to objectColor
    vec3 result = (ambient + diffuse + specular) * objectColor;;

    FragColor = vec4(result, 1.0);
}