#version 430
#define pi 3.14159265

out vec4 out_Color;

uniform mat4 lightViewMatrix;

uniform sampler2D coordsTexture;
uniform sampler2D normalTexture;
uniform sampler2D colorTexture;
uniform sampler2D ssaoTexture;

uniform int blurRange;


float blurredSSAO (void)
{
    float result = 0.0;
    ivec2 texCoord = ivec2(gl_FragCoord.xy);

    //Weights for Gaussian Blur:
    float weight;
    float weightSum = 0.0;

    vec4 pixel = texelFetch(ssaoTexture, texCoord, 0);
    if (pixel == vec4(0.0))
        discard;

    ivec2 offset;
    for(int i = -(blurRange-1)/2 ; i <= (blurRange-1)/2 ; i++)
    {
        for(int j = -(blurRange-1)/2 ; j <= (blurRange-1)/2 ; j++)
        {
            offset = ivec2(i, j);
            weight = 1.0;// (1.0/2.0*pi) * exp(-0.5*(offset.x*offset.x + offset.y*offset.y));
            pixel = texelFetch(ssaoTexture, texCoord + offset, 0);

            result += pixel.r * weight * pixel.w; 
            weightSum += weight * pixel.w;
        }
    }

    result /= weightSum;
    return result;

}

void main (void)
{
    vec4 vert = texelFetch(coordsTexture, ivec2(gl_FragCoord.xy), 0);

    // if background pixel, discard
    if (vert.w == 0.0)
        discard;

    vec3 normal = texelFetch(normalTexture, ivec2(gl_FragCoord.xy), 0).xyz;
    vec4 color = texelFetch(colorTexture, ivec2(gl_FragCoord.xy), 0);

    // ambient occlusion
    //float occlusion = texelFetch(ssaoTexture, ivec2(gl_FragCoord.xy), 0).x;
    float occlusion = blurredSSAO();

    // from now on, compute Phong Shader
    vec3 lightDirection = (lightViewMatrix * vec4(0.0, 0.0, 1.0, 0.0)).xyz;
    lightDirection = normalize(lightDirection);

    vec3 lightReflection = reflect(-lightDirection, normal);
    vec3 eyeDirection = -normalize(vert.xyz);
    float shininess = 100.0;

    vec4 ambientLight = vec4(0.5) * color;

    vec4 diffuseLight = color * 0.4 * max(dot(lightDirection, normal),0.0);
    vec4 specularLight = vec4(1.0) *  max(pow(dot(lightReflection, eyeDirection), shininess),0.0);

    //out_Color = vec4 (vec3(1.0)*occlusion, 1.0);
    out_Color = occlusion*vec4(ambientLight.xyz + diffuseLight.xyz + specularLight.xyz, 1.0);
}
