/**
in vec4 Position;
in vec3 MCnormal;

uniform mat4 MVMatrix;
uniform mat4 MVPMatrix;
uniform mat3 NormalMatrix;

uniform vec3 LightPosition;
**/
varying vec3 light;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

    vec3 normal = gl_NormalMatrix * gl_Normal;
    vec3 to_light = gl_LightSource[0].position - (gl_ModelViewMatrix * gl_Vertex).xyz;
    vec3 n_to_light = normalize(to_light);
    float intense = clamp(dot(normal, n_to_light), 0.0, 1.0);
    light = vec3(gl_LightSource[0].diffuse * intense * 0.9 + 0.1);
}
