#version 430

in vec4 in_Position;

out vec4 worldcoords;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main(void)
{
    worldcoords = in_Position;
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * in_Position;
}
