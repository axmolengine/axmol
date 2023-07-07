#version 310 es
precision highp float;
precision highp int;
#ifdef GL_ES
layout (location = 0) in lowp vec4 v_fragmentColor;
layout (location = 1) in mediump vec2 v_texCoord;
#else
layout (location = 2) in vec4 v_fragmentColor;
layout (location = 3) in vec2 v_texCoord;
#endif
layout(location = 4, binding = 0) uniform sampler2D u_tex0; // Y sample: LumaTexture
layout(location = 5, binding = 0) uniform sampler2D u_tex1; // UV sample: ChromaTexture
layout(std140, binding = 0) uniform Block_0 {
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
layout (location = 0) out vec4 FragColor;
void main()
{
    vec3 YUV;
    
    YUV.x = texture(u_tex0, v_texCoord).w; // Y
    YUV.yz = texture(u_tex1, v_texCoord).xy; // CbCr
	
    /* Convert YUV to RGB */
    vec4 OutColor;
    OutColor.xyz = trasnformYUV(YUV);
    OutColor.w = 1.0;
    FragColor = v_fragmentColor * OutColor;
}
