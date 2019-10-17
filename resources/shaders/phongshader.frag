#version 150

in vec4 color;
in vec3 normal;
in vec4 vert;

in vec2 texCoords;
in float depth;

out vec4 out_Color;

uniform mat4 lightViewMatrix;
uniform mat4 viewMatrix;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float shininess;

uniform bool has_texture;
uniform sampler2D model_texture;

void main(void)
{
    vec3 light_intensity = vec3(1.0);

    vec4 model_color = color;
    if (has_texture)
    {
        model_color = texture(model_texture, texCoords);
    }

    vec3 normvec = normalize(normal);
    vec3 lightDirection = (viewMatrix * inverse(lightViewMatrix) * vec4(0.0, 0.0, 1.0, 0.0)).xyz;
    lightDirection = normalize(lightDirection);
    vec3 lightReflection = reflect(-lightDirection, normvec);
    vec3 eyeDirection = normalize(-vert.xyz);

    vec3 ambient = light_intensity * ka;
    vec3 diffuse = light_intensity* kd * max(dot(lightDirection, normvec),0.0);
    vec3 specular = light_intensity * ks *  max(pow(dot(lightReflection, eyeDirection), shininess),0.0);

    out_Color = vec4(ambient.xyz + diffuse.xyz + specular.xyz, model_color.w);
}
