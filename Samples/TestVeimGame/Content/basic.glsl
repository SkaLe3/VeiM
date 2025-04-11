#type vertex
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec2 TexCoord;

void main() {
    gl_Position = u_ViewProjection * u_Transform * vec4(aPos, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}

#type fragment
#version 460 core
in vec2 TexCoord;
out vec4 FragColor;

uniform vec4 tintColor;
layout (binding = 0) uniform sampler2D u_Texture;


void main() {
    vec4 texColor = tintColor;
    texColor *= texture(u_Texture, TexCoord);
    FragColor = texColor;
}