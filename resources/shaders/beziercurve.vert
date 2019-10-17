#version 430

in vec4 in_Position;
in vec4 in_ControlPoint1;
in vec4 in_ControlPoint2;

out vec4 vert_color;
out vec4 control_point_1;
out vec4 control_point_2;

uniform vec4 in_Color;

void main(void)
{
	vert_color = in_Color;
	control_point_1 = in_ControlPoint1;
	control_point_2 = in_ControlPoint2;
	gl_Position = in_Position;
}
