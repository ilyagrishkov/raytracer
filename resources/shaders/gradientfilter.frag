#version 430

uniform sampler2D imageTexture;
uniform bool hdir;
uniform bool vdir;

out vec4 out_Color;

const mat3 horizontal = mat3
  (-1.0, 0.0, 1.0, 
   -2.0, 0.0, 2.0, 
   -1.0, 0.0, 1.0);

const mat3 vertical = mat3
  (-1.0,-2.0, -1.0,
    0.0, 0.0,  0.0,
    1.0, 2.0,  1.0);
 
void main()
{

  vec3 hresult = vec3(0.0);
  vec3 vresult = vec3(0.0);
  ivec2 texCoord = ivec2(gl_FragCoord.xy);


  for(int i = -1 ; i <= 1 ; i++)
  {
    for(int j = -1 ; j <= 1 ; j++)
    {
      vec3 pixelColor = texelFetch(imageTexture, texCoord + ivec2(i, j), 0).rgb;
      if (hdir)
      {
        hresult += (pixelColor.rgb * horizontal[i+1][j+1]);
      }
      if (vdir)
      {
        vresult += pixelColor.rgb * vertical[i+1][j+1];
      }
    }
  }

  out_Color = vec4(abs(hresult)+abs(vresult), 1.0);
}
