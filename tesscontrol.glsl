#version 410

// this is for triangles (3 vertices per polygon)
layout(vertices = 3) out;
// input from vertex shader is now packed in arrays of 3
in vec3 vVertex[];
in vec3 vNormal[];
// those same arrays are passed on to the tessellation evaluation
out vec3 tcPosition[];
out vec3 tcNormal[];

// read tesselation settings from program
uniform float TessLevelInner;
uniform float TessLevelOuter;

// this is shorter
#define ID gl_InvocationID

void main()
{
    tcPosition[ID] = vVertex[ID];
    tcNormal[ID] = vNormal[ID];
    // tell the tesselator how to split
    if (ID == 0) {
        gl_TessLevelInner[0] = TessLevelInner;
        gl_TessLevelOuter[0] = TessLevelOuter;
        gl_TessLevelOuter[1] = TessLevelOuter;
        gl_TessLevelOuter[2] = TessLevelOuter;
    }
}