#version 410 core

uniform vec3 lightPos = vec3(1);
uniform vec3 lightColor = vec3(1);
//uniform vec3 viewPos TODO: add in view pos

in vec3 fragNormal;
in vec3 fragPos;
in vec4 color;
out vec4 fragColor;

void main(void) {
	// ambient
	float ambientStrength = 0.3;
	vec3 ambient = ambientStrength * lightColor;

	// diffuse
	vec3 norm = normalize(fragNormal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	// specular
//	float specularStrength = 0.5;
//	vec3 viewDir = normalize(viewPos - fragPos);
//    vec3 reflectDir = reflect(-lightDir, norm);  
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
//    vec3 specular = specularStrength * spec * lightColor;  
//        
//    vec3 result = (ambient + diffuse + specular) * objectColor;
    vec3 result = (ambient + diffuse) * vec3(color);
    fragColor = vec4(result, 1.0);
    //fragColor = Color;
}
