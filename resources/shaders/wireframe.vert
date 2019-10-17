#version 150

in vec4 in_Position;
in vec3 in_Normal;
in vec2 in_TexCoords;
in vec4 in_Color;

out vec4 vert_color;
out vec3 vert_normal;
out vec2 vert_texcoords;

uniform vec4 default_color;

// if attribute in_Color exists or not
uniform bool has_color;

void main(void)
{
    if (has_color)
        vert_color = in_Color;
    else
        vert_color = default_color;

    vert_normal = in_Normal;
    vert_texcoords = in_TexCoords;
	gl_Position = in_Position;

}
