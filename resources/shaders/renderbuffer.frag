#version 430

uniform sampler2D tex;

out vec4 out_Color;
uniform int use_aa_filter;

const mat3 gaussian_filter = mat3(1/16.0, 2/16.0, 1/16.0, 2/16.0, 4/16.0, 2/16.0, 1/16.0, 2/16.0, 1/16.0);

void main()
{
    vec4 result = vec4(0.0);
    if (use_aa_filter == 1)
    {
        for (int j = 0; j < 3; ++j)
        {
            for (int i = 0; i < 3; ++i)
            {
                vec4 t = texelFetch(tex, ivec2(gl_FragCoord.xy + vec2(i-1, j-1)), 0);
                result.rgb += t.rgb * gaussian_filter[i][j];
                result.w = max(result.w, t.w);
            }
        }
    }
    else
    {
        result = texelFetch(tex, ivec2(gl_FragCoord.xy), 0);
    }

    if (result.w > 0.0)
        out_Color = vec4(abs(result.xyz), 1.0);
    else 
        discard;
}
