#version 430
#define pi 3.14159265

//Gaussian Blur with Standard Deviation = 1

uniform sampler2D blurTexture;
uniform int blurRange;

out vec4 out_Color;

void main (void)
{
    vec3 result = vec3(0.0);
    ivec2 texCoord = ivec2(gl_FragCoord.xy);

    //Weights for Gaussian Blur:
    float weight;
    float weightSum = 0.0;

    vec4 pixel = texelFetch(blurTexture, texCoord, 0);
    if (pixel == vec4(0.0))
        discard;

    ivec2 offset;
    for(int i = -(blurRange-1)/2 ; i <= (blurRange-1)/2 ; i++)
    {
        for(int j = -(blurRange-1)/2 ; j <= (blurRange-1)/2 ; j++)
        {
            offset = ivec2(i, j);
            weight = (1.0/2.0*pi) * exp(-0.5*(offset.x*offset.x + offset.y*offset.y));
            pixel = texelFetch(blurTexture, texCoord + offset, 0);

            //weight *= pixel.a;
            result += pixel.rgb * weight;
            weightSum += weight;
        }
    }

    result /= weightSum;

    //result = texelFetch(blurTexture, texCoord, 0).xyz;
    out_Color = vec4(result, 1.0);

}
