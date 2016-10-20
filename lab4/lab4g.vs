#version 330 core

uniform mat4 modelView;
uniform mat4 projection;
uniform vec4 colour;
uniform vec3 Eye;
uniform vec3 light;
uniform vec4 material;
in vec3 vNormal;
in vec4 vPosition;
out vec4 newColor;

void main(){
	
	vec3 position;
	vec3 normal;

	gl_Position = projection * modelView  * vPosition;
	position = vPosition.xyz;
	normal = vNormal;

	vec4 white = vec4(1.0, 1.0, 1.0, 1.0);
	float diffuse;
	vec3 L = normalize(light - position);
	vec3 N;
	vec3 H = normalize(L + (Eye - position));
	float specular;

	N = normalize(normal);
	diffuse = dot(N,L);
	if(diffuse < 0.0) {
		diffuse = 0.0;
		specular = 0.0;
	} else {
		specular = pow(max(0.0, dot(N,H)),material.w);
	}

	newColor = min(material.x*colour + material.y*diffuse*colour + material.z*white*specular, vec4(1.0));
	newColor.a = colour.a;
}