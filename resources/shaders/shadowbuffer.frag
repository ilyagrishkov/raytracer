#version 430

in vec4 worldcoords;
//in vec4 gl_FragCoord;

out vec4 out_Color;

void main(void)
{
//	if(gl_FragCoord.z > 1.0 || gl_FragCoord.z < 0.0)
//	{
//		discard;
//	}
    out_Color = worldcoords;
}
