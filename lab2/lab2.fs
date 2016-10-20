/*
 *  Simple fragment sharder for Lab 2
 */

#version 130

in vec3 normal;
in vec4 position;


void main() {
	vec4 base = vec4(1.0, 0.0, 0.0, 1.0);
	vec4 white = vec4(1.0, 1.0, 1.0, 1.0);
	float diffuse;
	vec3 L = vec3(1.0, 0.0, 0.0) - position.xyz;
	vec3 N;
	vec3 H = normalize(L + vec3(0.0, 0.0, 1.0));
	float specular;

	N = normalize(normal);
	diffuse = dot(N,L);
	if(diffuse < 0.0) {
		diffuse = 0.0;
		specular = 0.0;
	} else {
		specular = pow(max(0.0, dot(N,H)),100.0);
	}

	gl_FragColor = min(0.3*base + 0.7*diffuse*base + 0.7*white*specular, vec4(1.0));
	gl_FragColor.a = base.a;

}