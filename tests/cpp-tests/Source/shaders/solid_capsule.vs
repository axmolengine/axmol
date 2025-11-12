#version 310 es

layout(location=0) in vec2 a_localPosition;

#if !defined(AXSLC_TARGET_MSL)
layout(location=1) in vec4 a_instanceTransform;
layout(location=2) in vec4 a_instanceColor;
// radius: x, length: y, due to metal alignment is 16
layout(location=3) in vec4 a_instanceRadiusAndLength;
#endif

layout(location=0) out vec2 v_position;
layout(location=1) out vec4 v_color;
layout(location=2) out float v_length;
layout(location=3) out float v_thickness;

layout(std140, set = 0, binding = 0) uniform vs_ub {
    float u_pixelScale;
    mat4 u_MVPMatrix;
};

#if defined(AXSLC_TARGET_MSL)
struct instance_data_st {
    vec4 trans;
    vec4 color;
    float radius;
    float len;
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

    float radius = a_instanceRadiusAndLength.x;
    float length = a_instanceRadiusAndLength.y;
    float x = a_instanceTransform.x;
    float y = a_instanceTransform.y;
    float c = a_instanceTransform.z;
    float s = a_instanceTransform.w;
#else
    v_color = instances[gl_InstanceIndex].color;

    float radius = instances[gl_InstanceIndex].radius;
    float length = instances[gl_InstanceIndex].len;
    float x = instances[gl_InstanceIndex].trans.x;
    float y = instances[gl_InstanceIndex].trans.y;
    float c = instances[gl_InstanceIndex].trans.z;
    float s = instances[gl_InstanceIndex].trans.w;
#endif

    // scale quad large enough to hold capsule
    float scale = radius + 0.5 * length;

    // quad range of [-1, 1] implies normalize radius and length
    v_length = length / scale;

    // resolution.y = pixelScale * scale
    v_thickness = 3.0f / (u_pixelScale * scale);

    vec2 p = vec2(scale * a_localPosition.x, scale * a_localPosition.y);
    p = vec2((c * p.x - s * p.y) + x, (s * p.x + c * p.y) + y);
    gl_Position = u_MVPMatrix * vec4(p, 0.0, 1.0);
}
