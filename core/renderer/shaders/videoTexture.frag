#include <string_view>

/* mat4 to mat3:
mat3 coeff = mat3(
    colorTransform[0].x, colorTransform[0].y, colorTransform[0].z,
    colorTransform[1].x, colorTransform[1].y, colorTransform[1].z,
    colorTransform[2].x, colorTransform[2].y, colorTransform[2].z
);
mat3 coeff = mat3(
    colorTransform[0].xyz,
    colorTransform[1].xyz,
    colorTransform[2].xyz
);
mat3 coeff = mat3(colorTransform); // require GLES3
*/

// refer to:
// https://docs.microsoft.com/en-us/windows/win32/medfound/recommended-8-bit-yuv-formats-for-video-rendering#yuy2
const std::string_view videoTextureYUY2_frag = R"(

#ifdef GL_ES
varying lowp vec4 v_fragmentColor;
varying mediump vec2 v_texCoord;
#else
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
#endif

uniform sampler2D u_tex0; // Y sample
uniform sampler2D u_tex1; // UV sample
uniform mat4 colorTransform; 

vec3 trasnformYUV(vec3 YUV)
{
    YUV -= vec3(colorTransform[0].w, colorTransform[1].w, colorTransform[2].w);
    return mat3(
        colorTransform[0].xyz,
        colorTransform[1].xyz,
        colorTransform[2].xyz
    ) * YUV;
}

void main()
{
    vec3 YUV;
    
    /* For dual sampler */
    YUV.yz = texture2D(u_tex1, v_texCoord).yw;
    YUV.x = texture2D(u_tex0, v_texCoord).x;
	
    /* Convert YUV to RGB */
    vec4 OutColor;
    OutColor.xyz = trasnformYUV(YUV);
    OutColor.w = 1.0;

    gl_FragColor = v_fragmentColor * OutColor;
}
)"sv;

/*
The NV12 pixel format render shader: 
    - Windows: 
      - video format: HEVC(H256)
      - support codec id: 'hev1', 'hvc1'
    - Apple(macOS,iOS,tvOS): 
      - video format: H264,HEVC(H256)
      - support codec id: 'hvc1'
      - sub pixel formats:
        - 'y420v'(kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange)
        - 'y420f'(kCVPixelFormatType_420YpCbCr8BiPlanarFullRange)
Consider test videos:
  - HEVC(H265): 1912x1080.mp4, 1920x1080.mp4, 1912x1080_hvc1.mp4
  - H264: 1912x1080.mp4, 1920x1080.mp4, 1280x720.mp4, 432x240.mp4
refer to: 
  - https://docs.microsoft.com/en-us/windows/win32/medfound/recommended-8-bit-yuv-formats-for-video-rendering#nv12
  - https://github.com/doyoulikerock/D3D11NV12Rendering/blob/master/D3D11NV12Rendering/PixelShader.hlsl
*/
const std::string_view videoTextureNV12_frag = R"(

#ifdef GL_ES
varying lowp vec4 v_fragmentColor;
varying mediump vec2 v_texCoord;
#else
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
#endif

uniform sampler2D u_tex0; // Y sample: LumaTexture
uniform sampler2D u_tex1; // UV sample: ChromaTexture
uniform mat4 colorTransform; 

vec3 trasnformYUV(vec3 YUV)
{
    YUV -= vec3(colorTransform[0].w, colorTransform[1].w, colorTransform[2].w);
    return mat3(
        colorTransform[0].xyz,
        colorTransform[1].xyz,
        colorTransform[2].xyz
    ) * YUV;
}

void main()
{
    vec3 YUV;
    
    YUV.x = texture2D(u_tex0, v_texCoord).w; // Y
    YUV.yz = texture2D(u_tex1, v_texCoord).xy; // CbCr
	
    /* Convert YUV to RGB */
    vec4 OutColor;
    OutColor.xyz = trasnformYUV(YUV);
    OutColor.w = 1.0;

    gl_FragColor = v_fragmentColor * OutColor;
}
)"sv;

const std::string_view videoTextureBGRA_frag = R"(
#ifdef GL_ES
varying lowp vec4 v_fragmentColor;
varying mediump vec2 v_texCoord;
#else
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
#endif

uniform sampler2D u_tex0;

void main()
{
    gl_FragColor = v_fragmentColor * texture2D(u_tex0, v_texCoord).bgra;
}
)"sv;
