#version 310 es
precision highp float;
precision highp int;


layout(location = 0) in vec4 v_fragmentColor;
layout(location = 1) in vec2 v_texCoord;

layout(binding = 0) uniform sampler2D u_tex0; // Y sample
layout(binding = 1) uniform sampler2D u_tex1; // UV sample

layout(std140) uniform fs_ub {
    mat4 colorTransform; 
};

vec3 trasnformYUV(vec3 YUV)
{
    YUV -= vec3(colorTransform[0].w, colorTransform[1].w, colorTransform[2].w);
    return mat3(
        colorTransform[0].xyz,
        colorTransform[1].xyz,
        colorTransform[2].xyz
    ) * YUV;
}

layout(location = 0) out vec4 FragColor;

void main()
{
    vec3 YUV;
    
    /* For dual sampler */
    YUV.yz = texture(u_tex1, v_texCoord).yw;
    YUV.x = texture(u_tex0, v_texCoord).x;
	
    /* Convert YUV to RGB */
    vec4 OutColor;
    OutColor.xyz = trasnformYUV(YUV);
    OutColor.w = 1.0;

    FragColor = v_fragmentColor * OutColor;
}
