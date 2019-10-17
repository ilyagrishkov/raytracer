#version 430
#define numberOfSamples 4

out vec4 out_Color;

uniform mat4 projectionMatrix;

uniform vec3 kernel[numberOfSamples];

uniform sampler2D coordsTexture;
uniform sampler2D normalTexture;
uniform sampler2D noiseTexture;
uniform vec2 viewport;

uniform float radius;
uniform float intensity;
uniform float global_scale;

const float global_bias = 0.01;

float compareSample(in vec2 tcoord, in vec2 uv, in vec3 p, in vec3 n)
{
    vec4 q = texelFetch(coordsTexture, ivec2(tcoord + uv), 0).xyzw;

    // check if not a background pixel
    if (q.w == 0.0)
        return 0.0;

    vec3 diff = q.xyz - p;
    vec3 v = normalize(diff);
    float d = length(diff) * global_scale;

    return max(0.0, dot(n, v) - global_bias) * (1.0/(1.0+d)) * intensity;
}

float ambientOcclusion (in vec3 vert, in vec3 normal)
{

    vec2 samples[4];
    samples[0] = vec2(1.0,0.0);
    samples[1] = vec2(-1.0,0.0);
    samples[2] = vec2(0.0,1.0);
    samples[3] = vec2(0.0,-1.0);

    float occlusion = 0.0;

    vec2 noise_size = textureSize(noiseTexture, 0).xy;
    vec2 noisecoords = vec2(mod(gl_FragCoord.x, noise_size.x), mod(gl_FragCoord.y, noise_size.y));
    vec2 noise = texelFetch(noiseTexture, ivec2(noisecoords).xy, 0).xy;
    noise = noise*2.0 - 1.0;

    float rad = radius / vert.z;

    mat2 rotation = mat2(vec2(0.707, 0.707), vec2(-0.707, 0.707));
    vec2 texcoord = gl_FragCoord.xy;

    for (int i = 0; i < 4; ++i)
    {
        // the original reflects, but the behaviour was strange, so I'm just multiplying, it looks good anyway
      vec2 coord1 = reflect(samples[i], noise)*rad;
//        vec2 coord1 = normalize(samples[i]*noise)*rad;
        vec2 coord2 = rotation * coord1;

        occlusion += compareSample (texcoord, coord1 * 0.25, vert, normal);
        occlusion += compareSample (texcoord, coord1 * 0.75, vert, normal);
        occlusion += compareSample (texcoord, coord2 * 0.50, vert, normal);
        occlusion += compareSample (texcoord, coord2 * 1.00, vert, normal);
    }

    occlusion = 1.0 - (occlusion / float(4.0*4.0));

    return occlusion;
}

void main (void)
{
    // coordinates in view space
    vec4 vert = texelFetch(coordsTexture, ivec2(gl_FragCoord.xy), 0);

    // if background pixel, discard
    if (vert.w == 0.0)
        discard;

    // normal in view space
    vec3 normal = texelFetch(normalTexture, ivec2(gl_FragCoord.xy), 0).xyz;

    // compute ambient occlusion
    float occlusion = ambientOcclusion(vert.xyz, normal);

    out_Color = vec4(vec3(occlusion), 1.0);
}
