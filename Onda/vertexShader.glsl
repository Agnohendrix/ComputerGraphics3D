#version 420 core

layout(location=0) in vec3 vertexPosition_modelSpace;

uniform mat4 MV;
uniform float time;

out vec3 Color;
out vec3 coord;
void main(){

vec4 t=vec4(vertexPosition_modelSpace,1);
t.y = 0.1*sin(0.001*time+5.0*t.x)*sin(0.001*time+5.0*t.z);
coord=vec3(t);
gl_Position=MV*t;

Color=vec3(1.0,0.0,0.0);
}
