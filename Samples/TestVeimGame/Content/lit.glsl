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
    vec4 FragPosLightSpace;
} vs_out;

layout (std140) uniform Matrices
{
    uniform mat4 u_ViewProjection;    
};

uniform mat4 u_Transform;
uniform mat4 u_LightSpaceMatrix;


void main() {
    vs_out.FragPos = vec3(u_Transform * vec4(aPos, 1.0));
    vs_out.Normal = mat3(transpose(inverse(u_Transform))) * aNormal; 
    vs_out.UV = aUV;
    vs_out.FragPosLightSpace = u_LightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    gl_Position = u_ViewProjection * u_Transform * vec4(aPos, 1.0);
    
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
    vec4 FragPosLightSpace;
}gs_in[];

out GS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 UV;
    vec4 FragPosLightSpace;
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
            gs_out.FragPosLightSpace = gs_in[i].FragPosLightSpace;

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
        gs_out.FragPosLightSpace = gs_in[0].FragPosLightSpace;
        EmitVertex();
        gl_Position = Explode(gl_in[1].gl_Position, normal);
        gs_out.UV = gs_in[1].UV;
        gs_out.FragPos = Explode(vec4(gs_in[1].FragPos, 1.0), normal).xyz;
        gs_out.Normal   = gs_in[1].Normal;
        gs_out.FragPosLightSpace = gs_in[1].FragPosLightSpace;
        EmitVertex();
        gl_Position = Explode(gl_in[2].gl_Position, normal);
        gs_out.UV = gs_in[2].UV;
        gs_out.FragPos = Explode(vec4(gs_in[2].FragPos, 1.0), normal).xyz;
        gs_out.Normal   = gs_in[2].Normal;
        gs_out.FragPosLightSpace = gs_in[2].FragPosLightSpace;
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

    bool castShadows;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float farPlane;
    bool castShadows;
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
    vec4 FragPosLightSpace;
}fs_in;

in vec3 gs_FragPos;
in vec3 gs_Normal;
in vec2 gs_UV;

uniform vec3 u_ViewPos;
uniform Material u_Material;
uniform sampler2D u_ShadowMap;
uniform samplerCube u_ShadowCubeMap;

uniform DirLight u_DirLight;
uniform PointLight u_PointLights[NR_POINT_LIGHTS];
uniform SpotLight u_SpotLights[NR_SPOT_LIGHTS];

uniform bool u_UseDepth;
uniform float u_BiasBase;

uniform float u_NearZ;
uniform float u_FarZ;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir, vec3 normal );
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float LinearizeDepth(float depth);
float ShadowCalculationPoint(PointLight light,vec3 lightDir);


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
        float depth = LinearizeDepth(gl_FragCoord.z) / u_FarZ; // divide by far for demonstration
        FragColor = vec4(vec3(depth), 1.0);
    }
    else
    {
            //Debug shadow cube map
            //float closestDepth = texture(u_ShadowCubeMap, - u_PointLights[0].position + fs_in.FragPos ).r;
            //FragColor = vec4(vec3(closestDepth), 1.0); 

            FragColor = vec4(result, 1.0); 


        
    }  
}
float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir, vec3 normal )
{
    if (!u_DirLight.castShadows) return 0.0;

        // Consider using u_BiasBase calculated with farClip - nearClip expression
        float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
        // Perform perspective divide manually. gl_Position does this automatically
        // Needed only for perspective projection
        vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
        projCoords = projCoords * 0.5 + 0.5; // Transform NDC coords to the [0, 1]
        float currentDepth = projCoords.z;
        // PCF
        float shadow = 0.0;
        vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
        for (int x = -1; x <= 1; ++x)
        {
            for (int y = -1; y <= 1; ++y)
            {
                float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
                shadow += currentDepth- bias > pcfDepth ? 1.0 : 0.0;
            }
        }
        shadow /= 9.0;
        if (projCoords.z > 1.0) // Dont cast shadow outside the frustum
            shadow == 0.0;
        return shadow;

}

float ShadowCalculationPoint(PointLight light, vec3 fragToLight)
{
    if (!light.castShadows) return 0.0;

        float closestDepth = texture(u_ShadowCubeMap, fragToLight).r;
        closestDepth *= light.farPlane; // Transform back to [0, u_FarZ] from [0, 1]
        float currentDepth = length(fragToLight);
        float bias = 0.05;
        float shadow = (currentDepth - bias) > closestDepth ? 1.0 : 0.0;
        return shadow;

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

vec3 CombineLight(float shadow, vec3 lAmbient, vec3 lDiffuse, vec3 lSpecular, float diffFactor, float specFactor, float attenuation, float intensity)
{
    vec3 cAmbient = lAmbient * texture(u_Material.diffuse, fs_in.UV).rgb;
    vec3 cDiffuse = lDiffuse * diffFactor * texture(u_Material.diffuse, fs_in.UV).rgb;
    vec3 cSpecular = lSpecular * specFactor * texture(u_Material.specular, fs_in.UV).rgb;
    cAmbient *= attenuation * intensity;
    cDiffuse *= attenuation * intensity;
    cSpecular *= attenuation * intensity;
    return (cAmbient + (1.0 - shadow) * (cDiffuse + cSpecular));
}

vec3 CombineLight(float shadow, vec3 lAmbient, vec3 lDiffuse, vec3 lSpecular, float diffFactor, float specFactor, float attenuation)
{
    return CombineLight(shadow, lAmbient, lDiffuse, lSpecular, diffFactor, specFactor, attenuation, 1.0);
}

vec3 CombineLight(float shadow, vec3 lAmbient, vec3 lDiffuse, vec3 lSpecular, float diffFactor, float specFactor)
{
    return CombineLight(shadow, lAmbient, lDiffuse, lSpecular, diffFactor, specFactor, 1.0, 1.0);
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

    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, lightDir, normal);
    return CombineLight(shadow, light.ambient, light.diffuse, light.specular, diff, spec);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 fragToLight = fragPos - light.position;

    float diff = CalcDiffuse(lightDir, normal);
    float spec = CalcSpecular(lightDir, normal, viewDir);
    float attenuation = CalcAttenuation(light.position, fragPos, light.constant, light.linear, light.quadratic);

    float shadow = ShadowCalculationPoint(light, fragToLight);
    return CombineLight(shadow, light.ambient, light.diffuse, light.specular, diff, spec, attenuation);
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

    return CombineLight(0, light.ambient, light.diffuse, light.specular, diff, spec, attenuation, intensity);
}


float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * u_NearZ * u_FarZ) / (u_FarZ + u_NearZ - z * (u_FarZ - u_NearZ));    
}