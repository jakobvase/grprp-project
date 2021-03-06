#version 410

// input from glBegin
in vec4 Vertex;
in vec3 Normal;

// output to tesselation control
out vec3 vVertex;
out vec3 vNormal;

/** Since we are using tesselation, the tesselation evaluation, is now
	used as the new vertex shader.
uniform mat4 MVMatrix;
uniform mat4 MVPMatrix;
uniform vec3 LightPosition;
uniform vec3 LightColor;

out vec3 light;

void main()
{
    gl_Position =  MVPMatrix*Vertex;
    vec3 normal = (MVMatrix*vec4(Normal, 0.0)).xyz;
    vec3 to_light = LightPosition - (MVMatrix*Vertex).xyz;
    vec3 n_to_light = normalize(to_light);
    float intense = clamp(dot(normal, n_to_light.xyz), 0.0, 1.0);
    light = vec3(intense * LightColor * 0.9 + 0.1);
}
/**/

// as the tesselation still haven't created all vertices yet,
//	calculating light at this step is useless.
void main()
{
	vVertex = Vertex.xyz;
	vNormal = Normal;
}
