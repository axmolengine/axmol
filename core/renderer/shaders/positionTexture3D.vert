#version 310 es


layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texCoord;

layout(location = 0) out vec2 TextureCoordOut;


layout(std140, binding = 0) uniform vs_ub {
    mat4 u_MVPMatrix;
};

void main(void)
{
    gl_Position = u_MVPMatrix * a_position;
    TextureCoordOut = a_texCoord;
    TextureCoordOut.y = 1.0 - TextureCoordOut.y;
}
