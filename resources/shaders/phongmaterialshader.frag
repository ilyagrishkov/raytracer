#version 150

in vec3 normal;
in vec4 vert;
in vec2 texCoords;

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

    vec3 albedo = kd;
    if (has_texture)
    {
        albedo = texture(model_texture, texCoords).xyz;
    }

    vec3 normvec = normalize(normal);
    vec3 light_position = (inverse(lightViewMatrix) * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
    vec3 light_direction = normalize(light_position-vert.xyz);
    vec3 light_reflection = normalize(reflect(-light_direction, normvec));

    vec3 camera_position = (inverse(viewMatrix) * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
    vec3 eye_direction = normalize(camera_position-vert.xyz);

    vec3 ambient = light_intensity * ka;
    vec3 diffuse = light_intensity* albedo * max(dot(light_direction, normvec), 0.0);
    vec3 specular = light_intensity * ks *  pow(max(dot(light_reflection, eye_direction), 0.0), shininess);

    out_Color = vec4(min(ambient.xyz + diffuse.xyz + specular.xyz, vec3(1.0)), 1.0);
}

