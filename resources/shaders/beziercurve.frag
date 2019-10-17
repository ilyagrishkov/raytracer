#version 430

out vec4 out_Color;

in vec4 geom_color;

void main(void)
{
	out_Color = geom_color;
}

