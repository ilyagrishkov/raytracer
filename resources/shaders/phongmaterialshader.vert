#version 150

in vec4 in_Position;
in vec3 in_Normal;
in vec2 in_TexCoords;

out vec3 normal;
out vec4 vert;
out vec2 texCoords;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main(void)
{	

	mat4 normalMatrix = transpose(inverse(modelMatrix));
	normal = normalize(vec3(normalMatrix * vec4(in_Normal.xyz, 0.0)).xyz);

	vert = modelMatrix * in_Position;

	texCoords = in_TexCoords;

	gl_Position = (projectionMatrix * viewMatrix * modelMatrix) * in_Position;
}
