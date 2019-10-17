#version 430

uniform sampler2D shadowmap;

out vec4 out_Color;

void main()
{
    vec4 value = texelFetch( shadowmap, ivec2( gl_FragCoord.xy ), 0 );
    
    out_Color = vec4(-value.zzz/100.0, 1.0);
}
