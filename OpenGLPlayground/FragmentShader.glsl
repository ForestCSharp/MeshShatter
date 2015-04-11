#version 420

uniform vec4 inputColor;
uniform vec4 lightDir;


in vec4 normal;
in vec4 viewDirection;
in vec2 texCoords;
out vec4 fragColor;

void main() {
	

	//Normalize the normal
	vec4 norm = normalize(normal);

	//Diffuse
	float lambertian = max(dot(lightDir,norm), 0.0) * 2.0;

	//Specular
	vec4 halfVec = normalize(viewDirection + lightDir);
	float NdotH = dot(normal, halfVec);
	float intensity = pow( clamp(NdotH,0.0,1.0), 32.0 );

	fragColor = vec4(lambertian * 2 * vec4(1.0, 0.0, 0.0, 1.0) + intensity);
}