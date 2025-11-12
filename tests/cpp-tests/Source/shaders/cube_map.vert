#version 310 es
layout(location = POSITION) in vec4 a_position;
layout(location = NORMAL) in vec3 a_normal;

layout(location = TEXCOORD0) out vec3 v_reflect;
//uniforms

layout(std140, set = 0, binding = 0) uniform vs_ub {
    mat4 u_MVPMatrix;
    mat4 u_MVMatrix;
    mat3 u_NormalMatrix;
};

void main(void)
{
    gl_Position = u_MVPMatrix * a_position;
    
    // compute reflect
    vec4 positionWorldViewSpace = u_MVMatrix * a_position;
    vec3 vEyeVertex     = normalize(positionWorldViewSpace.xyz);
    
    vec3 v_normalVector = u_NormalMatrix * a_normal;
    v_reflect           = normalize(reflect(-vEyeVertex, v_normalVector));
}
