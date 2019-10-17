#version 430

in vec4 in_Position;

out vec4 cameracoords;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main(void)
{
    cameracoords = viewMatrix * modelMatrix * in_Position;
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * in_Position;
}
