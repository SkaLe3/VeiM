#define MAX_LIGHTS 8

// global uniform buffer for shared common set of uniforms among shaders
layout (std140, binding = 0) uniform CameraData
{
	mat4 View;
	mat4 Projection;
	mat4 ViewProjection;

	vec4 Position;


} u_Camera;

