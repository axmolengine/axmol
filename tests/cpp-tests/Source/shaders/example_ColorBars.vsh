#version 310 es
// http://www.cocos2d-iphone.org

layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texCoord;

layout(location = 0) out vec2 v_texCoord;


layout(std140, binding = 0) uniform vs_ub {
    mat4 u_PMatrix;
};

void main()
{
    gl_Position = u_PMatrix * a_position;
	v_texCoord = a_texCoord;
}

