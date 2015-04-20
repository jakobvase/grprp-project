#version 420

in vec4 MCvertex;
in vec3 MCnormal;

uniform mat4 MVMatrix;
uniform mat4 MVPMatrix;
uniform mat3 NormalMatrix;

uniform vec3 LightPosition;

out vec3 light;
out vec3 MCposition;

void main()
{
    MCposition = (MVPMatrix*MCvertex).xyz;

    vec3 normal = NormalMatrix*MCnormal;
    vec3 to_light = LightPosition - (MVMatrix*MCvertex).xyz;
    vec3 n_to_light = normalize(to_light);
    float intense = clamp(dot(normal, n_to_light.xyz), 0.0, 1.0);
    light = vec3(intense * 0.9 + 0.1);
}
