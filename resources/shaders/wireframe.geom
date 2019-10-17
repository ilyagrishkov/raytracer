#version 430

layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;

in vec4 vert_color[3];
in vec3 vert_normal[3];
in vec2 vert_texcoords[3];

out vec4 color;
out vec3 normal;
//out vec2 texcoords;
noperspective out vec3 edge_dist; //distance from three vertices
//out vec4 vert;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;


void main (void)
{

    vec3[3] dist;
    dist[0] = vec3(1.0, 0.0, 0.0);
    dist[1] = vec3(0.0, 1.0, 0.0);
    dist[2] = vec3(0.0, 0.0, 1.0);

    mat4 modelViewMatrix = viewMatrix * modelMatrix;
	mat4 normalMatrix = transpose(inverse(modelViewMatrix));

    for (int i = 0; i < 3; ++i)
    {
	    vec4 vert = modelViewMatrix * gl_in[i].gl_Position; 
	    gl_Position = projectionMatrix * vert;
        color = vert_color[i];
        normal = normalize(vec3(normalMatrix * vec4(vert_normal[i],0.0)).xyz);
	    //texcoords = vert_texcoords[i];
        edge_dist = dist[i];
	    EmitVertex();
    }
    
	EndPrimitive();
}
