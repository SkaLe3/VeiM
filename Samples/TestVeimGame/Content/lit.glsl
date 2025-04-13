#type vertex
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;


out vec3 FragPos;
out vec3 Normal;
out vec2 UV;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;



void main() {
    gl_Position = u_ViewProjection * u_Transform * vec4(aPos, 1.0);
    FragPos = vec3(u_Transform * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(u_Transform))) * aNormal; 
    UV = aUV;
}

#type fragment
#version 460 core

#define NR_POINT_LIGHTS 4
#define NR_SPOT_LIGHTS 1
struct DirLight{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight{
    vec3 position;
    vec3 direction;

    float innerCutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};


out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 UV;

uniform vec3 u_ViewPos;
uniform Material u_Material;

uniform DirLight u_DirLight;
uniform PointLight u_PointLights[NR_POINT_LIGHTS];
uniform SpotLight u_SpotLights[NR_SPOT_LIGHTS];

uniform bool u_UseDepth;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float LinearizeDepth(float depth);

float nearZ = 0.2;
float farZ = 100.0;

void main() {

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(u_ViewPos - FragPos);

    /* Directional Lighting */
    vec3 result = CalcDirLight(u_DirLight, norm, viewDir);

    /* Point lights */
    for (int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(u_PointLights[i], norm, FragPos, viewDir);

    /* Spot lights */
    for (int i = 0; i < NR_SPOT_LIGHTS; i++)
        result += CalcSpotLight(u_SpotLights[i], norm, FragPos, viewDir);

    if (u_UseDepth)
    {
        float depth = LinearizeDepth(gl_FragCoord.z) / farZ; // divide by far for demonstration
        FragColor = vec4(vec3(depth), 1.0);
    }
    else
    {
       FragColor = vec4(result, 1.0); 
    }  
}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    
    /* Diffuse Shading */
    float diff = max(dot(normal, lightDir), 0.0);

    /* Specular Shading */
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);

    /* Combine */
    vec3 ambient = light.ambient * texture(u_Material.diffuse, UV).rgb;
    vec3 diffuse = light.diffuse * diff * texture(u_Material.diffuse, UV).rgb;
    vec3 specular = light.specular * spec * texture(u_Material.specular, UV).rgb;
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    /* Diffuse Shading */
    float diff = max(dot(normal, lightDir), 0.0);

    /* Specular Shading */
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);

    /* Attenuation */
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance 
        + light.quadratic * distance * distance);

    /* Combine */
    vec3 ambient = light.ambient * texture(u_Material.diffuse, UV).rgb;
    vec3 diffuse = light.diffuse * diff * texture(u_Material.diffuse, UV).rgb;
    vec3 specular = light.specular * spec * texture(u_Material.specular, UV).rgb;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    /* Diffuse Shading */
    float diff = max(dot(normal, lightDir), 0.0);

    /* Specular Shading */
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);

    /* Spot light */
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    /* Attenuation */
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance 
        + light.quadratic * distance * distance);

    /* Combine */
    vec3 ambient = light.ambient * texture(u_Material.diffuse, UV).rgb;
    vec3 diffuse = light.diffuse * diff * texture(u_Material.diffuse, UV).rgb;
    vec3 specular = light.specular * spec * texture(u_Material.specular, UV).rgb;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient  + diffuse + specular);
}


float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * nearZ * farZ) / (farZ + nearZ - z * (farZ - nearZ));    
}