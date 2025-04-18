#include <Buffers.glsl>

struct DirectionalLight
{
	vec4 Color;
	vec3 Direction;
	float Intensity;
	int CastShadows;
}

struct PointLight
{
	vec4 Color;
	vec3 Position;
	float Intensity;
	float Radius;
	float CutOff;
}

struct SpotLight
{
	vec4 Color;
	vec3 Position;
	vec3 Direction;
	float Intensity;
	float SpotAngle;
	float InnerCutOff;
	float OuterCutOff;
	float Radius;
}

layout()