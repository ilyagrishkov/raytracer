#version 330

in vec4 color;
in vec3 normal;
in vec4 vert;
in vec4 shadow_vert;

in vec2 texCoords;
in float depth;

out vec4 out_Color;

uniform mat4 lightViewMatrix;
uniform mat4 lightProjectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 cropMatrix;

uniform float ka;
uniform float kd;
uniform float ks;
uniform float shininess;

uniform bool multisampling_enabled;
uniform bool shadowmap_enabled;

uniform sampler2D shadowmap;

void main(void)
{
    vec3 lightDirection = (viewMatrix * inverse(lightViewMatrix) * vec4(0.0, 0.0, 1.0, 0.0)).xyz;
    lightDirection = normalize(lightDirection);

    vec3 normalvec = normalize(normal);

    vec3 lightReflection = reflect(-lightDirection, normalvec);
    vec3 eyeDirection = -normalize(vert.xyz);

    vec4 ambientLight = color * ka;
    vec4 diffuseLight = color * kd * max(dot(lightDirection, normalvec),0.0);
    vec4 specularLight = vec4(vec3(ks), 1.0) *  max(pow(dot(lightReflection, eyeDirection), shininess),0.0);

    if(shadowmap_enabled)
    {
	   	vec4 proj = cropMatrix * lightProjectionMatrix * lightViewMatrix * modelMatrix * shadow_vert;
     	proj /= proj.w;
   		proj.xy = (proj.xy + vec2(1.0)) * 0.5;

      	float cosTheta = dot(normal, lightDirection);
      	float shadow_factor = 1.0;

      //If a given fragment has its back to the light, it is in shadow
      if(cosTheta < 0)
      {
        shadow_factor = 0.0;
      }
      else
      {
        float min_bias = 0.005;
        float bias = min_bias + clamp(tan(acos(cosTheta)), 0.0, 0.1); 
        
        //Shadowmap's depth check

        float dist = (lightViewMatrix * modelMatrix * shadow_vert).z;

        vec4 shadowbuf = texture (shadowmap, proj.xy); 

        if(multisampling_enabled)
        {
          //Multi-sampling

          vec2 neighbours[9] = vec2[](  vec2(-1.0, 1.0), vec2( 0.0, 1.0), vec2( 1.0, 1.0),
                                        vec2(-1.0, 0.0), vec2( 0.0, 0.0), vec2( 1.0, 0.0),
                                        vec2(-1.0,-1.0), vec2( 0.0,-1.0), vec2( 1.0,-1.0));

          ivec2 texture_size = textureSize(shadowmap, 0);
          
          for(int i=0; i<9; i++)
          {
              neighbours[i] = neighbours[i]/texture_size;
          }

          int light_samples = 0;
          for(int i=0; i<9; i++)
          {
            shadowbuf = texture(shadowmap, proj.xy + neighbours[i]);

            if(dist + bias < shadowbuf.z)
              light_samples += 1;
          }
          shadow_factor -= float(light_samples)/9.0;

        }
        else
        {
          //Single sample
          if (dist + bias < shadowbuf.z)
            shadow_factor = 0.0;
        }

        if(shadowbuf.w == 0)
          shadow_factor = 1.0;
      }

      out_Color = vec4(ambientLight.xyz+diffuseLight.xyz+specularLight.xyz,1.0)*shadow_factor;
    }
    else
    {
      out_Color = vec4(ambientLight.xyz+diffuseLight.xyz+specularLight.xyz, color.w);
    }
}
