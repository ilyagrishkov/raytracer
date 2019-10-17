#version 430

in vec4 in_Position;
in vec4 in_Color;

out vec4 color;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform vec4 default_color;

// if attribute in_Color exists or not
uniform bool has_color;

void main(void)
{
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * in_Position;

    if (has_color)
        color = in_Color;
    else
        color = default_color;

}
