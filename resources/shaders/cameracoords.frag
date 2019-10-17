#version 430

in vec4 cameracoords;

out vec4 out_Color;

void main(void)
{
    out_Color = cameracoords;

}
