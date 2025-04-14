#type vertex
#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 u_ViewProjection;

void main() {
    TexCoords = aPos;
    vec4 pos =  u_ViewProjection * vec4(aPos, 1.0);
    gl_Position = pos.xyww;

	
}

#type fragment
#version 460 core
out vec4 FragColor;
in vec3 TexCoords;

uniform samplerCube u_SkyBox;



void main() {
    FragColor = texture(u_SkyBox, TexCoords);
}
