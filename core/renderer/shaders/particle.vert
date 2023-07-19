#version 310 es

                                              
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec4 a_color;
layout(location = 2) in vec2 a_texCoord;

layout(location = 0) out vec2 TextureCoordOut;
layout(location = 1) out vec4 ColorOut;

layout(std140) uniform vs_ub {
    mat4 u_PMatrix;
};

void main()
{
    ColorOut = a_color;
    TextureCoordOut = a_texCoord;
    TextureCoordOut.y = 1.0 - TextureCoordOut.y;
    gl_Position = u_PMatrix * a_position;
}

