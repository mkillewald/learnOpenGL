#version 330 core
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float     shininess;
};

struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
  
uniform Material material;  
uniform Light light; 
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float time;

in vec3 FragPos;
in vec3 Normal; 
in vec2 TexCoords;

out vec4 FragColor;

float band(float start, float end, float t) {  
    return step(start, t)*step(t, end);
}

float rect(vec2 uv, float start) {
    return band(start, 1.0f-start, TexCoords.x)*band(start, 1.0f-start, TexCoords.y);
}

void main()
{    
    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
  	
    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuseMap = vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * diffuseMap;  
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    vec3 specularMap = vec3(texture(material.specular, TexCoords));
    vec3 specular = light.specular * spec * specularMap;

    // emission
    vec2 myTexCoords = TexCoords;
    myTexCoords.x = myTexCoords.x + 0.045f; // slightly shift texture on x for better alignment
    vec3 emissionMap = vec3(texture(material.emission, myTexCoords + vec2(0.0,time*0.75)));
    vec3 emission = emissionMap * (sin(time)*0.5f+0.5f)*2.0;

    // emission mask
    float emissionMask = rect(TexCoords, specularMap.r);
    emission = emission * emissionMask;
    
    vec3 result = ambient + diffuse + specular + emission;
    FragColor = vec4(result, 1.0f);
}