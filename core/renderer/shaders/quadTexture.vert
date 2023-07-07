#version 310 es
precision highp float;
precision highp int;
                                              
layout (location = 0) in vec4 a_position;
layout (location = 1) in vec4 a_color;
layout (location = 2) in vec2 a_texCoord;
layout (location = 0) out vec2 TextureCoordOut;
layout (location = 1) out vec4 ColorOut;
layout(std140, binding = 0) uniform Block_0 {
    mat4 u_MVPMatrix;
};
void main()
{
    ColorOut = a_color;
    TextureCoordOut = a_texCoord;
    TextureCoordOut.y = 1.0 - TextureCoordOut.y;
    gl_Position = u_MVPMatrix * a_position;
}
