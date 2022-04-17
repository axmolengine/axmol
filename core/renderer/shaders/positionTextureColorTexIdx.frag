const char* positionTextureColorTexIdx_frag = R"(
#ifdef GL_ES
varying lowp vec4 v_fragmentColor;
varying mediump vec2 v_texCoord;
varying lowp float v_texIdx;
#else
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
varying float v_texIdx;
#endif

//#define numTextures 16

//uniform sampler2D u_textures[numTextures];

uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform sampler2D u_texture2;
uniform sampler2D u_texture3;

vec4 getSampleFromArray(int ndx, vec2 uv) {
	if(ndx == 0){
		return texture2D(u_texture0, uv);
	}
	
	if(ndx == 1){
		return texture2D(u_texture1, uv);
	}
	
	if(ndx == 2){
		return texture2D(u_texture2, uv);
	}
	
	if(ndx == 3){
		return texture2D(u_texture3, uv);
	}
	
	return texture2D(u_texture0, uv);
}

void main()
{
	gl_FragColor = v_fragmentColor * getSampleFromArray(int(v_texIdx), v_texCoord);
}
)";
