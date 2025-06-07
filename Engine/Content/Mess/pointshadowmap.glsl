#type vertex
#version 460 core
layout (location = 0) in vec3 aPos;

uniform mat4 u_Transform;

void main() {
    gl_Position = u_Transform * vec4(aPos, 1.0);
}

#type geometry
#version 460 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 u_ShadowMatrices[6];

out vec4 FragPos;

void main()
{
    for (int face = 0; face < 6; ++face)
    {
        gl_Layer = face;
        for (int i = 0; i < 3; ++i)
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = u_ShadowMatrices[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}

#type fragment
#version 460 core
in vec4 FragPos;

uniform vec3 u_LightPos;
uniform float u_FarPlane;

void main() 
{
    float lightDistance = length(FragPos.xyz - u_LightPos);
    lightDistance = lightDistance / u_FarPlane; // map to [0; 1] range
    gl_FragDepth = lightDistance;
}
