#version 430

layout (lines) in;
layout (line_strip, max_vertices=60) out;

in vec4 vert_color[2];
in vec4 control_point_1[2];
in vec4 control_point_2[2];

out vec4 geom_color;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;


void main (void)
{

	mat4 modelviewprojMatrix = projectionMatrix * viewMatrix * modelMatrix;

	// first endpoint P0
	gl_Position = modelviewprojMatrix * gl_in[0].gl_Position;
	geom_color = vert_color[0];
	EmitVertex();

	
	float divs = 50.0;	
	float t = 0.0;
	vec4 pt = vec4(0.0);
	geom_color = vec4 (0.0, 0.0, 1.0, 1.0);
	for (int i = 1; i < int(divs-1); i++)
	{
		t = (i+1)/divs;	
		pt = pow(1-t,3)*gl_in[0].gl_Position + 3.0*pow(1-t,2)*t*control_point_1[0] +
					3.0*(1-t)*pow(t,2)*control_point_2[0] + pow(t, 3)*gl_in[1].gl_Position;
		//pt = t * gl_in[0].gl_Position + (1.0-t) * gl_in[1].gl_Position;
		gl_Position = modelviewprojMatrix * pt;
		EmitVertex();
	}
	

	// second endpoint P3
	gl_Position = modelviewprojMatrix * gl_in[1].gl_Position;
	geom_color = vert_color[1];
	EmitVertex();
/*
	gl_Position = modelviewprojMatrix * control_point_2[0];
	geom_color = vec4 (0.0, 1.0, 0.0, 1.0);
	EmitVertex();

	gl_Position = modelviewprojMatrix * control_point_1[0];
	geom_color = vec4 (0.0, 1.0, 0.0, 1.0);
	EmitVertex();
*/

	EndPrimitive();
}
