#version 410

layout(vertices = 3) out;
in vec3 vVertex[];
in vec3 vNormal[];
out vec3 tcPosition[];
out vec3 tcNormal[];

uniform float TessLevelInner;
uniform float TessLevelOuter;

#define ID gl_InvocationID

void main()
{
	//gl_out[ID].gl_Position = vec4(vVertex[ID], 1.0);
    tcPosition[ID] = vVertex[ID];
    tcNormal[ID] = vNormal[ID];
    if (ID == 0) {
        gl_TessLevelInner[0] = TessLevelInner;
        gl_TessLevelOuter[0] = TessLevelOuter;
        gl_TessLevelOuter[1] = TessLevelOuter;
        gl_TessLevelOuter[2] = TessLevelOuter;
    }
}