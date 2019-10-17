#version 150

in vec4 color;
in vec3 normal;
in vec4 vert;

in vec2 texCoords;
in float depth;

out vec4 out_Color;

uniform mat4 lightViewMatrix;
uniform mat4 viewMatrix;
uniform float sigma;

void main(void)
{
    vec3 light = (viewMatrix * inverse(lightViewMatrix) * vec4(0.0, 0.0, 1.0, 0.0)).xyz;
    light = normalize(light);

    vec3 view = -normalize(vert.xyz);

    float sigma2 = sigma*sigma;
    float A = 1 - 0.5 * (sigma2) / (sigma2 + 0.57);
    float B = 0.45 * (sigma2) / (sigma2 + 0.09);

    float cos_theta_i = dot(light, normal);
    float theta_i = acos(cos_theta_i);
    float theta_r = acos(dot(view, normal));

    float alpha = max(theta_i, theta_r);
    float beta = min(theta_i, theta_r);

    // cos (phi_r - phi_i)
    float cos_diff_phi = dot(normalize(view - normal * dot(view, normal)), normalize(light - normal*dot(light,normal)));

    float w = cos_theta_i * (A + (B * max(0.0, cos_diff_phi) * sin(alpha) * tan(beta) ));

    vec4 L = w * color;//vec4(1.0);

    out_Color = L;
}
