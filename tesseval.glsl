#version 410

layout(triangles, equal_spacing, cw) in;
in vec3 tcPosition[];
in vec3 tcNormal[];

out vec4 Vertex;
out vec3 Normal;
out vec3 light;

uniform mat4 MVMatrix;
uniform mat4 MVPMatrix;
uniform vec3 LightPosition;
uniform vec3 LightColor;
uniform float TangentLength;

struct point {
	vec3 p, n;
};

float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

point bezier(vec3 p0, vec3 p3, vec3 n1, vec3 n2, float t) {
	vec3 t1, t2, e1, e2, tangent;
	vec3 v, n;
	e1 = p3 - p0;
	e2 = p0 - p3;
	t1 = cross(cross(n1, e1), n1); // Create the vector pointing toward the curve.
	t2 = cross(cross(n2, e2), n2);
	float l = length(e1);
	t1 = normalize(t1) * (l * TangentLength);
	t2 = normalize(t2) * (l * TangentLength);

	float nt = 1 - t;
	vec3 p1 = p0 + t1;
	vec3 p2 = p3 + t2;
	v = nt * nt * nt * p0 +
		3 * nt * nt * t * p1 +
		3 * nt * t * t * p2 +
		t * t * t * p3;
	tangent = 3 * nt * nt * (p1 - p0) +
		6 * nt * t * (p2 - p1) +
		3 * t * t * (p3 - p2);
	n = cross(tangent, nt * cross(n1, e1) + t * cross(e2, n2));
	//n = n1 + n2;
	n = normalize(n);

	point p;
	p.p = v;
	p.n = n;
	return p;
}

void main()
{
	/**
    vec3 p0 = gl_TessCoord.x * tcPosition[0];
    vec3 p1 = gl_TessCoord.y * tcPosition[1];
    vec3 p2 = gl_TessCoord.z * tcPosition[2];
    /**/

    float t1 = 0.5;
    if (gl_TessCoord.z < 1.0)
		t1 = gl_TessCoord.y / (gl_TessCoord.x + gl_TessCoord.y);
    point edge = bezier(tcPosition[0], tcPosition[1], tcNormal[0], tcNormal[1], t1);

    float t2 = gl_TessCoord.z;
    point middle = bezier(edge.p, tcPosition[2], edge.n, tcNormal[2], t2);
    point mn = bezier(edge.p, tcPosition[2], edge.n, tcNormal[2], 1);

    light = gl_TessCoord;

    //vec4 Vertex = vec4((p0 + p1 + p2), 1);
    Vertex = vec4(middle.p, 1);
    gl_Position = MVPMatrix * Vertex;

    Normal = (MVMatrix*vec4(middle.n, 0.0)).xyz;
    /**
    vec3 to_light = LightPosition - (MVMatrix*Vertex).xyz;
    vec3 n_to_light = normalize(to_light);
    float intense = clamp(dot(normal, n_to_light.xyz), 0.0, 1.0);
    light = intense * 0.95 * LightColor + 0.05;
    /**/
}