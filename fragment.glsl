#version 140
in vec3 light;

void main()
{
    gl_FragColor = vec4(light, 1.0);
}
