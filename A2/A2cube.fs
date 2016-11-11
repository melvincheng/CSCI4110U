#version 330 core

in vec3 normal;
in vec3 position;
in vec3 tc;
uniform samplerCube tex;
uniform vec4 colour;
uniform vec3 Eye;
uniform vec3 light;
uniform vec4 material;

void main(){

	gl_FragColor = texture(tex, position);
}