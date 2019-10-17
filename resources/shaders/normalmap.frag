#version 430
	 
in vec3 normal;
out vec4 out_Color;
	 
void main(void)
{
    out_Color = vec4(abs(normal), 1.0);
}
