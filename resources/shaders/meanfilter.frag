#version 430

uniform sampler2D imageTexture;
uniform int kernelsize;

out vec4 out_Color;

void main ()
{
  vec3 result = vec3(0.0);
  ivec2 texCoord = ivec2(gl_FragCoord.xy);
  
  for(int i = -kernelsize; i <= kernelsize; i++)
  {
    for(int j = -kernelsize; j <= kernelsize; j++)
    {
      vec3 pixelColor = texelFetch(imageTexture, texCoord + ivec2(i, j), 0).rgb;
      result += pixelColor.rgb;
    }
  }

  result /= float((2*kernelsize+1) * (2*kernelsize+1));

  out_Color = vec4(result, 1.0);
}
