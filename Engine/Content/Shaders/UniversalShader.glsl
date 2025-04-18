#type vertex
#version 460 core
layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec2 a_UV;
layout (location = 2) in vec3 a_Normal;
layout (location = 3) in vec3 a_Tangent;


struct VertexOutput
{
	vec3 FragPos;
	vec3 Normal;
	mat3 TBN;
};

struct Input
{
	vec2 UV;
};


out VertexOutput fs_in;
out Input fs_user_in;

#include <Buffers.glsl>

uniform mat4 u_Transform;



void main()
{
	fs_in.FragPos = vec3(u_Transform * vec4(a_Pos, 1.0));
	fs_in.Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;
	fs_user_in.UV = a_UV;

	vec3 T = normalize(vec3(u_Transform * vec4(a_Tangent, 0.0)));
	vec3 N = normalize(vec3(u_Transform * vec4(a_Normal, 0.0)));
	T = normalize(T - dot(T, N) * N);

	vec3 B = cross(N, T);
	fs_in.TBN = mat3(T, B, N);

	gl_Position = u_ViewProjection * u_Transform * vec4(a_Pos, 1.0); 
}


#type fragment
#version 460 core

// For VMSL:
// User defines function with #pragma surface funcName LightingModelName
// #pragma vertex, geometry and fragment could be add later



// Engine Defined
struct SurfaceOutput 
{
	vec3 Diffuse;
	vec3 Specular;
	vec3 Normal;
	float Shininess;
};

// Engine defined part
struct VertexOutput
{
	vec3 FragPos;
	vec3 Normal;
	mat3 TBN;
};

// User defined part
struct Input
{
	vec2 UV;
};

// Engine Defined
out vec4 FragColor;
in VertexOutput fs_in;
in Input fs_user_in;

// User Defined
uniform sampler2D u_DiffuseMap;
uniform sampler2D u_NormalMap;
uniform sampler2D u_SpecularMap;
uniform float u_Shininess;

#include <Buffers.glsl>

// Engine defined
float AttenuationCalculation(vec3 lightDir, float c, float l, float q);
vec4 LightingBlinnPhong(SurfaceOutput s, vec3 lightDir, vec3 viewDir, float atten, vec4 lightColor, vec4 lightAmibent, vec4 lightSpecular);
float GetShadowFactorPoint(vec3 fragPos, int lightIndex);
float GetShadowFactorDir(vec3 fragPos, vec3 normal, int lightIndex);
vec3 UnpackNormal(vec3 normalValue);

// User defined
void surf(Input IN, inout SurfaceOutput o)
{
	o.Diffuse = texture(u_DiffuseMap, IN.UV).rgb;
	o.Specular = texture(u_SpecularMap, IN.UV).rgb;
	o.Normal = UnpackNormal(texture(u_NormalMap, IN.UV).rgb);
	o.Shininess = u_Shininess;
}

// Engine defined (everythin below)
void main() 
{
	SurfaceOutput o;
	surf(fs_user_in, o);

	vec3 viewDir = normalize(u_CamPos - fs_in.FragPos);

	vec4 result = vec4(0.0);
	for (int i = 0; i < u_LightCount; i++)
	{
		float attenuation;
		vec3 lightDir;
		if (u_LightPositions[i].w == 0)
		{
			lightDir = -u_LightPositions[i].xyz;
			attenuation = 1.0 * (1.0 - GetShadowFactorDir(fs_in.FragPos, o.Normal, i));
		}
		else
		{
			lightDir = u_LightPositions[i].xyz - fs_in.FragPos;
			attenuation = AttenuationCalculation(lightDir, u_LightAttenuation[i].x, u_LightAttenuation[i].y, u_LightAttenuation[i].z)
			*(1.0 - GetShadowFactorPoint(fs_in.FragPos, i)) ;

		}

		result += LightingBlinnPhong(o, lightDir, viewDir, attenuation, i);
	}
	result += u_AmbientColor;
	FragColor = vec4(result.rgb, 1.0);
}

vec4 LightingBlinnPhong(SurfaceOutput s, vec3 lightDir, vec3 viewDir, float atten, int lightIndex)
{
	vec3 lightDirNormalized = normalize(lightDir);
	vec3 normal = normalize(s.Normal);
	vec3 halfwayDir = normalize(lightDirNormalized + viewDir);

	float diffuseFactor = max(dot(normal, lightDirNormalized), 0.0);
	float specularFactor = pow(max(dot(normal, halfwayDir), 0.0), s.Shininess * 128.0);

	vec3 diffuse = s.Diffuse * u_LightsColors[lightIndex].rgb * u_LightsColors[lightIndex].a * diffuseFactor;
	vec3 specular = s.Specular * u_SpecularLightsColors[lightIndex].rgb * specularFactor;

	return vec4((diffuse + specular) * atten, 1.0);

}

float AttenuationCalculation(vec3 lightDir, float c, float l, float q)
{
	float distance = length(lightDir);
	return 1.0 / (c + l * distance + q * distance * distance);
}

float GetShadowFactorPoint(vec3 fragPos, int lightIndex)
{
	if (u_ShadowMapIndices[lightIndex] == -1) return 0.0;

	vec4 lightPos = u_LightPositions[lightIndex];
	float farPlane = u_ShadowFarPlanes[lightIndex];

	vec3 sampleOffsetDirections[20] = vec3[]
    (
        vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
        vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
        vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
        vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
        vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
    );  
    vec3 fragToLight = fragPos - lightPos.xyz;
    float currentDepth = length(fragToLight);
    float shadow = 0.0;
    float bias = 0.05; // Make configurable
    int samples = 20;
    float viewDistance = length(u_CamPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / u_ShadowFarPlanes[lightIndex])) / 25.0; // Make 25.0 configurable
    for (int i = 0; i < samples; ++i)
    {
    	float closestDepth = texture(u_PointLightShadowMaps[lightIndex], fragToLight + sampleOffsetDirections[i] * diskRadius).r;
    	closestDepth *= u_ShadowFarPlanes[lightIndex];
    	if (currentDepth - bias > closestDepth)
    		shadow += 1.0;
    }
    shadow /= float(samples);
    return shadow;
}

float GetShadowFactorDir(vec3 fragPos, vec3 normal, int lightIndex)
{
	if (u_ShadowMapIndices[lightIndex] == -1) return 0.0;

	vec4 lightPos = u_LightPositions[lightIndex];
	vec3 fragToLight = fragPos - lightPos.xyz;
	vec4 fragPos_LightSpace = u_LightSpaceMatrices[lightIndex] * vec4(fragPos, 1.0);

	// Make dependant on far plane value
	float bias = max(0.005 * (1.0 - dot(normal, fragToLight)), 0.0005);

	vec3 projCoords = fragPos_LightSpace.xyz / fragPos_LightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;

	if (projCoords.z > 1.0)
		return 0.0;

	float currentDepth = projCoords.z;

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(u_DirLightShadowMaps[lightIndex], 0);
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(u_DirLightShadowMaps[lightIndex], projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;
	return shadow;
}

vec3 UnpackNormal(vec3 normalValue)
{
	vec3 normal = normalize(normalValue * 2.0 - vec3(1.0, 1.0, 1.0));
	return normalize(fs_in.TBN * normal);
}