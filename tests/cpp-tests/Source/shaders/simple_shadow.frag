#version 310 es
precision highp float;
precision highp int;
layout(location = TEXCOORD0) in vec2 v_texCoord;
layout(location = POSITION) in vec4 v_position;
layout(set = 1, binding = 0) uniform sampler2D u_sampler0;
layout(set = 1, binding = 1) uniform sampler2D u_sampler1;
layout(std140, set = 0, binding = 1) uniform fs_ub {
    vec3 u_target_pos;
    vec4 u_color;
};

layout(location = SV_Target0) out vec4 FragColor;

void main(void)
{

	float Radius = 4.0;//project range
	vec3 UVector = vec3(1.0, 0.0, 0.0)/(2.0 * Radius);
	vec3 VVector = vec3(0.0, 0.0, -1.0)/(-2.0 * Radius);
	vec2 coord;  
	coord.x = dot(v_position.xyz - u_target_pos, UVector) + 0.5;
	coord.y = dot(v_position.xyz - u_target_pos, VVector) + 0.5;

	FragColor = u_color*texture(u_sampler0,v_texCoord)*texture(u_sampler1,coord);
}
