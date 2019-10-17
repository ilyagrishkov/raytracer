#version 430

in vec4 in_Position;

out vec4 worldcoords;
out vec4 gl_Position;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 cropMatrix;

void main(void)
{
    worldcoords = viewMatrix * modelMatrix * in_Position;
	gl_Position =  cropMatrix * projectionMatrix * viewMatrix * modelMatrix * in_Position;
}