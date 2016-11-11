#version 330 core

uniform mat4 modelView;
uniform mat4 projection;
uniform sampler2D tex;
in vec3 vNormal;
in vec4 vPosition;
in vec2 vTex;
out vec3 normal;
out vec3 position;
out vec2 tc;

void main(){
	vec4 p;

	p = vPosition + 0.1 * texture(tex,vTex) * vec4(vNormal,1.0);
	gl_Position = projection * modelView  * p;
	position = p.xyz;
	normal = vNormal;
	tc = vTex;
}