#version 330 core

in vec3 normal;
in vec3 position;
vec4 colour;
uniform vec3 Eye;
uniform vec3 light;
uniform vec4 material;

void main(){
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
	if (position.z < -2.0) {
		//Water
 		colour.x = 0.11;
 		colour.y = 0.42;
 		colour.z = 0.63;
	} else if (position.z > 30) {
		//Mountain peak
		colour.x = 1.0;
		colour.y = 1.0;
		colour.z = 1.0;
		
	} else if (position.z < 0 && position.z > -2) {
		//Sand
		colour.x = 0.75;
		colour.y = 0.74;
		colour.z = 0.07;
	} else if (position.z > 10){
		//Mountain 
		colour.x = 0.5;
		colour.y = 0.5;
		colour.z = 0.5;
	} else {
		//Land
		colour.x = 0.49;
		colour.y = 0.99;
		colour.z = 0.0;
	}
	gl_FragColor = min(material.x*colour + material.y*diffuse*colour + material.z*white*specular, vec4(1.0));
	gl_FragColor.a = colour.a;
}