#version 310 es
precision highp float;
precision highp int;

layout(location = COLOR0) in vec4 v_color;
layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(set = 1, binding = 0) uniform sampler2D u_tex0;

const float blurSize = 1.0/512.0;
const float intensity = 0.35;
layout(std140, set = 0, binding = 1) uniform fs_ub {
    vec2 resolution;
};

layout(location = SV_Target0) out vec4 FragColor;

void main()
{
   vec4 sum = vec4(0);
   vec2 texcoord = v_texCoord.xy;
   int j;
   int i;

   //thank you! http://www.gamerendering.com/2008/10/11/gaussian-blur-filter-shader/ for the 
   //blur tutorial
   // blur in y (vertical)
   // take nine samples, with the distance blurSize between them
   sum += texture(u_tex0, vec2(texcoord.x - 4.0*blurSize, texcoord.y)) * 0.05;
   sum += texture(u_tex0, vec2(texcoord.x - 3.0*blurSize, texcoord.y)) * 0.09;
   sum += texture(u_tex0, vec2(texcoord.x - 2.0*blurSize, texcoord.y)) * 0.12;
   sum += texture(u_tex0, vec2(texcoord.x - blurSize, texcoord.y)) * 0.15;
   sum += texture(u_tex0, vec2(texcoord.x, texcoord.y)) * 0.16;
   sum += texture(u_tex0, vec2(texcoord.x + blurSize, texcoord.y)) * 0.15;
   sum += texture(u_tex0, vec2(texcoord.x + 2.0*blurSize, texcoord.y)) * 0.12;
   sum += texture(u_tex0, vec2(texcoord.x + 3.0*blurSize, texcoord.y)) * 0.09;
   sum += texture(u_tex0, vec2(texcoord.x + 4.0*blurSize, texcoord.y)) * 0.05;
	
	// blur in y (vertical)
   // take nine samples, with the distance blurSize between them
   sum += texture(u_tex0, vec2(texcoord.x, texcoord.y - 4.0*blurSize)) * 0.05;
   sum += texture(u_tex0, vec2(texcoord.x, texcoord.y - 3.0*blurSize)) * 0.09;
   sum += texture(u_tex0, vec2(texcoord.x, texcoord.y - 2.0*blurSize)) * 0.12;
   sum += texture(u_tex0, vec2(texcoord.x, texcoord.y - blurSize)) * 0.15;
   sum += texture(u_tex0, vec2(texcoord.x, texcoord.y)) * 0.16;
   sum += texture(u_tex0, vec2(texcoord.x, texcoord.y + blurSize)) * 0.15;
   sum += texture(u_tex0, vec2(texcoord.x, texcoord.y + 2.0*blurSize)) * 0.12;
   sum += texture(u_tex0, vec2(texcoord.x, texcoord.y + 3.0*blurSize)) * 0.09;
   sum += texture(u_tex0, vec2(texcoord.x, texcoord.y + 4.0*blurSize)) * 0.05;

   //increase blur with intensity!
   FragColor = sum*intensity + texture(u_tex0, texcoord); 
   return;
   /*if(sin(iGlobalTime) > 0.0)
       FragColor = sum * sin(iGlobalTime)+ texture(iChannel0, texcoord);
   else
	   FragColor = sum * -sin(iGlobalTime)+ texture(iChannel0, texcoord);
   */
}
