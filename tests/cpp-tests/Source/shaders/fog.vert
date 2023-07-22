#version 310 es
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texCoord;
layout(location = 0) out float v_fogFactor;              //weight for fog
layout(location = 1) out vec2 v_texture_coord;



layout(std140, binding = 0) uniform vs_ub {
    float u_fogDensity;// For exp and exp2 equation
    float u_fogStart; // This is only for linear fog
    float u_fogEnd; // This is only for linear fog
    int u_fogEquation; // 0 = linear, 1 = exp, 2 = exp2
    mat4 u_MVPMatrix;
};

void main(void)
{
    gl_Position = u_MVPMatrix * a_position;
    v_texture_coord = a_texCoord; 

    float fogFragCoord = abs(gl_Position.z);                                       //get fog distance


    if(u_fogEquation == 0)
            v_fogFactor = (u_fogEnd-fogFragCoord )/(u_fogEnd-u_fogStart);                //linear fog
    else if(u_fogEquation == 1)
            v_fogFactor = exp(-u_fogDensity*fogFragCoord  );                         //exp fog
    else if(u_fogEquation == 2)
	    v_fogFactor = exp(-pow(u_fogDensity*fogFragCoord , 2.0));                //exp2 fog


    v_fogFactor = clamp(v_fogFactor, 0.0, 1.0);                                     //clamp 0 to 1
    
}
