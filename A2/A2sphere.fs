#version 330 core

in vec3 normal;
in vec3 position;
in vec2 tc;
uniform samplerCube tex;
uniform vec4 colour;
uniform vec3 Eye;
uniform vec3 light;
uniform vec4 material;

void main(){
	vec3 tc;

	vec3 L = normalize(light - position);
	vec3 N = normalize(normal);
	
	float n1 = 1.52f;
	float n2 = 1.0f;

	float eta = n1/n2;


	vec4 white = vec4(1.0, 1.0, 1.0, 1.0);
	float diffuse;

	vec3 R = normalize(reflect(-L, normal));

	float specular;
	vec4 colour;

	N = normalize(normal);
	diffuse = dot(N, L);
	if(diffuse < 0.0) {
		diffuse = 0.0;
		specular = 0.0;
	} else {
		specular = pow(max(0.0, dot(N,R)), material.w);
	}

	vec3 reflectColour = reflect(normalize(position-Eye), N);
	vec3 refractColour = refract(normalize(position-Eye), N, eta);

	float Fo = pow(((n1 - n2)/(n1 + n2)),2.0f);
	float SA = Fo + (1.0f - Fo)*pow((1.0f - max(0.0f, dot(N, reflectColour))), 5.0f);

	
	//colour = texture(tex,reflectColour) * SA + texture(tex,refractColour) * (1.0f - SA);
	colour = texture(tex,normal);
	//gl_FragColor = min(material.x * colour + material.y * diffuse * colour, vec4(1.0));
	gl_FragColor = colour;
	gl_FragColor.a = colour.a;
}