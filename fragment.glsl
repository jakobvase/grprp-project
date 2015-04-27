#version 410


uniform mat4 MVMatrix;
uniform vec3 LightPosition;
uniform vec3 LightColor;

in vec4 Vertex;
in vec3 Normal;

in vec3 light;

void main()
{

    vec3 to_light = LightPosition - (MVMatrix*Vertex).xyz;
    vec3 n_to_light = normalize(to_light);
    float intense = clamp(dot(Normal, n_to_light.xyz), 0.0, 1.0);
    vec3 l = intense * 0.95 * LightColor + 0.05;

    gl_FragColor = vec4(l, 1.0);
}
