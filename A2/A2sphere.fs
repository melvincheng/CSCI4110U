#version 330 core

in vec3 normal;
in vec3 position;
in vec2 tc;
uniform samplerCube tex;
uniform vec4 colour;
uniform vec3 Eye;
uniform vec3 light;
uniform vec4 material;

int num = 1235;
int a = 141;
int c = 28411;
int m = 134456;

float rand() {
	float f;
	num = (a*num+c) % m;
	f = (num+0.0)/m;
	return((f-0.5)*2.0);
}

void main(){
	vec4 colour = vec4(0.0,0.0,0.0,1.0);
	vec3 N = normalize(normal);
	int count = 0;

	vec4 total;

	num = int((normal.x*normal.y + normal.x*normal.z) * 123456);  
	num = num % m;
	for (int i = 0; i < 10000; i++){
		vec3 temp = normalize(vec3(rand(),rand(),rand()));
		temp = normalize(temp);
		if(dot(normal, temp) > 0) {
			colour += texture(tex, temp);
			count++;
		}
	}


	float n1 = 1.52f;
	float n2 = 1.0f;

	float eta = n1/n2;

	vec3 reflectColour = reflect(normalize(position-Eye), N);
	vec3 refractColour = refract(normalize(position-Eye), N, eta);

	float Fo = pow(((n1 - n2)/(n1 + n2)),2.0f);
	float SA = Fo + (1.0f - Fo)*pow((1.0f - max(0.0f, dot(N, reflectColour))), 5.0f);

	
	//colour = texture(tex,reflectColour) * SA + texture(tex,refractColour) * (1.0f - SA);
	//colour = texture(tex,refractColour);

	colour = colour/count;
	gl_FragColor = colour;
	gl_FragColor.a = colour.a;
}