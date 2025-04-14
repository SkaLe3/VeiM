#type vertex
#version 460 core
/* ===========================================================
 *                      VERTEX SHADER
   =========================================================== */
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;

out VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 UV;
} vs_out;

layout (std140) uniform Matrices
{
    uniform mat4 u_ViewProjection;    
};

uniform mat4 u_Transform;



void main() {
    gl_Position = u_ViewProjection * u_Transform * vec4(aPos, 1.0);
    vs_out.FragPos = vec3(u_Transform * vec4(aPos, 1.0));
    vs_out.Normal = mat3(transpose(inverse(u_Transform))) * aNormal; 
    vs_out.UV = aUV;
}

// -----------------------------------------------------------
#type geometry
#version 460 core
/* ===========================================================
 *                      GEOMETRY SHADER
   =========================================================== */
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 UV;
}gs_in[];

out GS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 UV;
}gs_out;

uniform float u_Time;

vec3 GetNormal();
vec4 Explode(vec4 position, vec3 normal);

void main() {    
    
    if (u_Time < 0.0)
    {
        /* Defulat Behavior */
        for (int i = 0; i < 3; ++i)
        {
            gl_Position     = gl_in[i].gl_Position;
            gs_out.FragPos  = gs_in[i].FragPos;
            gs_out.Normal   = gs_in[i].Normal;
            gs_out.UV       = gs_in[i].UV;

            EmitVertex();
        }
        EndPrimitive();
    }
    
    else
    {
        /* Explode */
        
        vec3 normal = GetNormal();

        gl_Position = Explode(gl_in[0].gl_Position, normal);
        gs_out.UV = gs_in[0].UV;
        gs_out.FragPos = Explode(vec4(gs_in[0].FragPos, 1.0), normal).xyz;
        gs_out.Normal   = gs_in[0].Normal;
        EmitVertex();
        gl_Position = Explode(gl_in[1].gl_Position, normal);
        gs_out.UV = gs_in[1].UV;
        gs_out.FragPos = Explode(vec4(gs_in[1].FragPos, 1.0), normal).xyz;
        gs_out.Normal   = gs_in[1].Normal;
        EmitVertex();
        gl_Position = Explode(gl_in[2].gl_Position, normal);
        gs_out.UV = gs_in[2].UV;
        gs_out.FragPos = Explode(vec4(gs_in[2].FragPos, 1.0), normal).xyz;
        gs_out.Normal   = gs_in[2].Normal;
        EmitVertex();
        EndPrimitive();
    }

}  

vec3 GetNormal()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}
vec4 Explode(vec4 position, vec3 normal)
{
    float magnitude = 2.0;
    vec3 direction = normal * ((sin(u_Time) + 1.0) / 2.0) * magnitude; 
    return position + vec4(direction.xy, 0.0, 0.0);
}

// -----------------------------------------------------------
#type fragment
#version 460 core
/* ===========================================================
 *                      FRAGMENT SHADER
   =========================================================== */
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

in GS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 UV;
}fs_in;

in vec3 gs_FragPos;
in vec3 gs_Normal;
in vec2 gs_UV;

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

    vec3 norm = normalize(fs_in.Normal);
    vec3 viewDir = normalize(u_ViewPos - fs_in.FragPos);

    /* Directional Lighting */
    vec3 result = CalcDirLight(u_DirLight, norm, viewDir);

    /* Point lights */
    for (int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(u_PointLights[i], norm, fs_in.FragPos, viewDir);

    /* Spot lights */
    for (int i = 0; i < NR_SPOT_LIGHTS; i++)
        result += CalcSpotLight(u_SpotLights[i], norm, fs_in.FragPos, viewDir);

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

float CalcSpecular(vec3 lightDir, vec3 normal, vec3 viewDir)
{
    vec3 halfwayDir = normalize(lightDir + viewDir);
    return pow(max(dot(normal, halfwayDir), 0.0), u_Material.shininess);
}

float CalcDiffuse(vec3 lightDir, vec3 normal)
{
    return max(dot(normal, lightDir), 0.0);
}

vec3 CombineLight(vec3 lAmbient, vec3 lDiffuse, vec3 lSpecular, float diffFactor, float specFactor, float attenuation, float intensity)
{
    vec3 cAmbient = lAmbient * texture(u_Material.diffuse, fs_in.UV).rgb;
    vec3 cDiffuse = lDiffuse * diffFactor * texture(u_Material.diffuse, fs_in.UV).rgb;
    vec3 cSpecular = lSpecular * specFactor * texture(u_Material.specular, fs_in.UV).rgb;
    cAmbient *= attenuation * intensity;
    cDiffuse *= attenuation * intensity;
    cSpecular *= attenuation * intensity;
    return (cAmbient + cDiffuse + cSpecular);
}

vec3 CombineLight(vec3 lAmbient, vec3 lDiffuse, vec3 lSpecular, float diffFactor, float specFactor, float attenuation)
{
    return CombineLight(lAmbient, lDiffuse, lSpecular, diffFactor, specFactor, attenuation, 1.0);
}

vec3 CombineLight(vec3 lAmbient, vec3 lDiffuse, vec3 lSpecular, float diffFactor, float specFactor)
{
    return CombineLight(lAmbient, lDiffuse, lSpecular, diffFactor, specFactor, 1.0, 1.0);
}


float CalcAttenuation(vec3 lightPos, vec3 fragPos, float c, float l, float q)
{
    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (c + l * distance + q * distance * distance);
    return attenuation;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    
    float diff = CalcDiffuse(lightDir, normal);
    float spec = CalcSpecular(lightDir, normal, viewDir);

    return CombineLight(light.ambient, light.diffuse, light.specular, diff, spec);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = CalcDiffuse(lightDir, normal);
    float spec = CalcSpecular(lightDir, normal, viewDir);
    float attenuation = CalcAttenuation(light.position, fragPos, light.constant, light.linear, light.quadratic);

    return CombineLight(light.ambient, light.diffuse, light.specular, diff, spec, attenuation);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = CalcDiffuse(lightDir, normal);
    float spec = CalcSpecular(lightDir, normal, viewDir);
    float attenuation = CalcAttenuation(light.position, fragPos, light.constant, light.linear, light.quadratic);

    /* Spot light */
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    return CombineLight(light.ambient, light.diffuse, light.specular, diff, spec, attenuation, intensity);
}


float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * nearZ * farZ) / (farZ + nearZ - z * (farZ - nearZ));    
}