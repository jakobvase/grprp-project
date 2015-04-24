#version 410

layout(triangles, equal_spacing, cw) in;
in vec3 tcPosition[];
in vec3 tcNormal[];

out vec3 light;

uniform mat4 MVMatrix;
uniform mat4 MVPMatrix;
uniform vec3 LightPosition;
uniform vec3 LightColor;

float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
    vec3 p0 = gl_TessCoord.x * tcPosition[0];
    vec3 p1 = gl_TessCoord.y * tcPosition[1];
    vec3 p2 = gl_TessCoord.z * tcPosition[2];
    vec4 Vertex = vec4((p0 + p1 + p2), 1);
    gl_Position = MVPMatrix * Vertex;

    vec3 normal = (MVMatrix*vec4(tcNormal[0], 0.0)).xyz;
    vec3 to_light = LightPosition - (MVMatrix*Vertex).xyz;
    vec3 n_to_light = normalize(to_light);
    float intense = clamp(dot(normal, n_to_light.xyz), 0.0, 1.0);
    light = vec3(intense * vec3(pow(rand(p0.xy), 3.0)));
}