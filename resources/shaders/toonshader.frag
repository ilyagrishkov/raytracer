#version 430
	 
in vec4 color;
in vec3 normal;
in vec4 vert;

out vec4 out_Color;

uniform mat4 lightViewMatrix;
uniform mat4 viewMatrix;
uniform float quantizationLevel;
	 
void main(void)
{

    vec3 lightDirection = (viewMatrix * inverse(lightViewMatrix) * vec4(0.0, 0.0, 1.0, 0.0)).xyz;
    lightDirection = normalize(lightDirection);

    vec3 lightReflection = reflect(-lightDirection, normal);
    vec3 eyeDirection = normalize(-vert.xyz);
    float shininess = 100.0;

    vec4 diffuseLight = color * max(dot(lightDirection, normal), 0.0);
    vec4 specularLight = vec4(1.0) *  max(pow(dot(lightReflection, eyeDirection), shininess), 0.0);

    float intensity = dot(lightDirection, normal);

    vec3 currentColor = vec3(diffuseLight.xyz + specularLight.xyz);

    // color quantization for toon effect:
    currentColor = floor(0.5 + (quantizationLevel * currentColor)) / quantizationLevel;

    out_Color = vec4(currentColor, 1.0);

}


