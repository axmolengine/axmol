#version 310 es

layout (location = POSITION) in vec4 a_position;
layout (location = TEXCOORD0) in vec2 a_texCoord;
#if !defined(AXSLC_TARGET_MSL)
layout (location = TEXCOORD1) in mat4 a_instance;
#endif
layout (location = TEXCOORD0) out vec2 v_texCoord;

layout(std140, binding = 0) uniform vs_ub {
    mat4 u_MVPMatrix;
};

#if defined(AXSLC_TARGET_MSL)
layout(std140, binding = 1) buffer vs_inst {
    mat4 u_instance[];
};
#endif

void main(void)
{
#if defined(AXSLC_TARGET_MSL)
    gl_Position = u_MVPMatrix * u_instance[gl_InstanceIndex] * a_position;
#else
    gl_Position = u_MVPMatrix * a_instance * a_position;
#endif
    v_texCoord = a_texCoord;
    v_texCoord.y = 1.0 - v_texCoord.y;
}
