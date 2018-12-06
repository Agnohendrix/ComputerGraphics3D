#version 420 core

in vec3 Color;
in vec3 coord;

out vec4 Color_interpolato;

void main()
{
	vec3 col1 = vec3(0.0, 0.8, 0.8);
	vec3 col2 = vec3(0.8, 0.0, 0.0);
	float value = 0.5 * (1.0+(sin(coord.x*5.0)*sin(coord.z*2.0)) );
	vec3 color = mix(col1, col2, value);
	Color_interpolato=vec4(color, 1.0);
}