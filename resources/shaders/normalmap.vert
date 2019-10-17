#version 430
	 
in vec4 in_Position;
in vec3 in_Normal;

out vec3 normal;
	 
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
	 
void main(void)
{
    normal = in_Normal;
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * in_Position;

}
