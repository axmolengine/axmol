#version 310 es


layout(location = 0) in vec3 a_position;
layout(location = 0) out vec3 v_reflect;

layout(std140) uniform vs_ub {
    mat4  u_cameraRot;
};

void main(void)
{
    vec4 reflect =  u_cameraRot * vec4(a_position, 1.0);
    v_reflect = reflect.xyz;
    gl_Position = vec4(a_position.xy, 1.0 , 1.0);
}
