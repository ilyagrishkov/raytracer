#version 130

uniform sampler2D imageTexture;
uniform ivec4 viewport;
uniform float alpha;

out vec4 out_Color;

void main()
{
  //vec2 texCoord = vec2((gl_FragCoord.xy - viewport.xy)/vec2(viewport.zw));
  vec2 texCoord = vec2((gl_FragCoord.xy - viewport.xy)/vec2(viewport.zw));
  vec3 result = texture(imageTexture, texCoord).rgb;
  out_Color = vec4(result.xyz, alpha);
}
