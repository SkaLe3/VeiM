#type vertex
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_View;
uniform mat4 u_Transform;

out vec2 TexCoord;
out vec3 FragPos;

void main() {
    gl_Position = u_ViewProjection * u_View * u_Transform * vec4(aPos, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
    FragPos = vec3(u_Transform * vec4(aPos, 1.0));
    float temp = u_ViewProjection[0][0] + u_View[0][0] + u_Transform[0][0];
}

#type fragment
#version 460 core
in vec2 TexCoord;
out vec4 FragColor;

uniform vec4 tintColor;
layout (binding = 0) uniform sampler2D u_Texture;

in vec3 FragPos; // You'll need to pass world-space position in vertex shader
uniform vec3 cameraPos; // Camera position

void main() {
    vec4 texColor = tintColor * texture(u_Texture, TexCoord);

    // Simulate glow based on view angle
    // FragPos is in local model space here
    float glowFactor = 1.0 - abs(dot(normalize(FragPos), vec3(0.0, 0.0, 1.0))); 
    glowFactor = pow(glowFactor, 2.5);

    vec3 glowColor = vec3(2.5, 1.4, 0.5); // Strong orange for testing
    texColor.rgb += glowColor * glowFactor;

    FragColor = texColor;
}
