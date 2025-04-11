#type vertex
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;

struct Light {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec3 FragPos;
out vec3 Normal;
out vec3 LightDirection;
out vec2 UV;

uniform mat4 u_ViewProjection;
uniform mat4 u_View;
uniform mat4 u_Transform;

uniform Light u_Light;

void main() {
    gl_Position = u_ViewProjection * u_Transform * vec4(aPos, 1.0);
    FragPos = vec3(u_View * u_Transform * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(u_View * u_Transform))) * aNormal; 
    LightDirection = mat3(u_View) * u_Light.direction;
    UV = aUV;
}

#type fragment
#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 LightDirection;
in vec2 UV;

struct Light {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};


uniform Light u_Light;
uniform Material u_Material;

void main() {
    // Ambient
    vec3 ambient = u_Light.ambient * vec3(texture(u_Material.diffuse, UV));

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-LightDirection);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = u_Light.diffuse * diff * vec3(texture(u_Material.diffuse, UV));

    // Specular
    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
    vec3 specular = u_Light.specular * spec * vec3(texture(u_Material.specular, UV));



    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0); 
}
