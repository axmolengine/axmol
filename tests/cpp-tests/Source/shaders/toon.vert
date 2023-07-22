#version 310 es
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec3 a_normal;
layout(location = 0) out vec2 v_texture_coord;
layout(location = 1) out vec3 v_normal;


layout(std140, binding = 0) uniform vs_ub {
    mat4 u_MVPMatrix;
    mat3 u_NormalMatrix;
};

void main(void)
{
    gl_Position = u_MVPMatrix * a_position;
    v_texture_coord = a_texCoord;
    v_texture_coord.y = (1.0 - v_texture_coord.y);
	v_normal = u_NormalMatrix *a_normal;
}
