#type vertex
#version 460 core
layout (location = 0) in vec3 aPos;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;


void main() {
    gl_Position = u_ViewProjection * u_Transform * vec4(aPos, 1.0);
}

#type fragment
#version 460 core
out vec4 FragColor;

void main() {
    //FragColor = vec4(1.0, 0.8, 0.05, 1.0f);
    FragColor = vec4(0.23, 0.95, 0.16, 1.0f);
}