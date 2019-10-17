#version 150

in vec4 color;
in vec3 normal;
//in vec4 vert;

//in vec2 texcoords;

noperspective in vec4 edge_dist;

out vec4 out_Color;

uniform mat4 lightViewMatrix;
uniform float thickness;
uniform vec4 line_color;
//const float ka;
//const float kd;
//const float ks;
//const float alpha;

void main(void)
{

    float min_dist = min(min(edge_dist[0], edge_dist[1]), edge_dist[2]);
    //float edge_intensity = exp2(-1.0*min_dist*min_dist);

    vec3 lightDirection = normalize((lightViewMatrix * vec4(0.0, 0.0, 1.0, 0.0)).xyz);

    vec4 diffuse_color = color * max(dot(lightDirection, normal),0.0);

    if (min_dist > thickness)
    {
        if (color.w == 0.0)
            discard;
        out_Color = vec4(diffuse_color.rgb, color.w);
    }
    else
        out_Color = line_color;

}
