#version 310 es

layout(location = 0) in vec2 a_localPosition;

#if !defined(AXSLC_TARGET_MSL)
layout(location = 1) in vec4 a_instanceColor;
// pos: xy, radius: z, due to metal alignment is 16
layout(location = 2) in vec4 a_instancePosAndRadius;
#endif

layout(location = 0) out vec2 v_position;
layout(location = 1) out vec4 v_color;
layout(location = 2) out float v_thickness;

layout(std140, set = 0, binding = 0) uniform vs_ub {
    float u_pixelScale;
    mat4 u_MVPMatrix;
};

#if defined(AXSLC_TARGET_MSL)
struct instance_data_st {
    vec4 color;
    vec2 pos;
    float radius;
};
layout(std140, binding = 1) readonly buffer vs_inst {
    instance_data_st instances[];
};
#endif

void main()
{
    v_position = a_localPosition;

#if !defined(AXSLC_TARGET_MSL)
    v_color = a_instanceColor;
    vec2 instancePos = a_instancePosAndRadius.xy;
    float radius = a_instancePosAndRadius.z;
#else
    v_color = instances[gl_InstanceIndex].color;
    vec2 instancePos = instances[gl_InstanceIndex].pos;
    float radius = instances[gl_InstanceIndex].radius;
#endif

    // resolution.y = pixelScale * radius
    v_thickness = 3.0f / (u_pixelScale * radius);

    vec2 p = vec2(radius * a_localPosition.x, radius * a_localPosition.y) + instancePos;
    gl_Position = u_MVPMatrix * vec4(p, 0.0f, 1.0f);
}
