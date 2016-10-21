#version 330 core

in vec3 position;
uniform vec4 colour;
uniform vec3 Eye;
uniform vec3 light;
uniform vec4 material;

void main(){
	gl_FragColor = colour;	
	gl_FragColor.a = colour.a;
}