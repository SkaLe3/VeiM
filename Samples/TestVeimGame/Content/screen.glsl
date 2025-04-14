#type vertex
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;

out vec2 UV;

void main() {
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
	UV = aUV;
}

#type fragment
#version 460 core
out vec4 FragColor;
in vec2 UV;

uniform sampler2D u_ScreenTexture;



void main() {
    // Default
    FragColor = texture(u_ScreenTexture, UV);

    // Invert
    //FragColor = vec4(1 - texture(u_ScreenTexture, UV).rgb, 1.0);
    
    // Grayscale
    //FragColor = texture(u_ScreenTexture, UV);
    //float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    //FragColor = vec4(average, average, average, 1.0);


    /*
    // Kernel
    vec2 offset = 1.0 / vec2(textureSize(u_ScreenTexture, 0));
    vec2 offsets[9] = vec2[](
        vec2(-offset.x,  offset.y), // top-left
       vec2( 0.0f,    offset.y), // top-center
        vec2( offset.x,  offset.y), // top-right
        vec2(-offset.x,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset.x,  0.0f),   // center-right
        vec2(-offset.x, -offset.y), // bottom-left
        vec2( 0.0f,   -offset.y), // bottom-center
        vec2( offset.x, -offset.y)  // bottom-right    
    );

    // Sharpen Kernel
    //float kernel[9] = float[](
    //    -1, -1, -1,
    //    -1,  9, -1,
    //    -1, -1, -1
    //);
    // Blur Kernel
    //float kernel[9] = float[](
    //1.0 / 16, 2.0 / 16, 1.0 / 16,
    //2.0 / 16, 4.0 / 16, 2.0 / 16,
    //1.0 / 16, 2.0 / 16, 1.0 / 16  
    //);


    // Edge detection
    
    float kernel[9] = float[](
    1, 1, 1,
    1, -8, 1,
    1, 1, 1 
    );
    
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(u_ScreenTexture, UV.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    
    FragColor = vec4(col, 1.0);
    */
}
