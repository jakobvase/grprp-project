varying vec3 light;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;

    vec3 normal = gl_NormalMatrix*gl_Normal;
    vec3 to_light = (gl_LightSource[0].position - (gl_ModelViewMatrix*gl_Vertex)).xyz;
    vec3 n_to_light = normalize(to_light);
    float intense = clamp(dot(normal, n_to_light), 0.0, 1.0);
    light = gl_LightSource[0].diffuse.xyz * intense * 0.9 + 0.1;
}
