#version 410 core

in vec3 normal;
in vec3 pos;

//uniform int mode = 1;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 color;
//uniform bool blinn = false;

struct DirLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

const float shininess = 32.0; // power of two

//const float screenGamma = 2.2; // Assume the monitor is calibrated to the sRGB color space
//const bool colorCorrect = false;

//const float ambientStrength = 0.05;
//const vec3 lightColor = vec3(0.1);

out vec4 FragColor;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 pos, vec3 viewDir);

//void main() {
//    // ambient
//    vec3 ambient = ambientStrength * color;
//    // diffuse
//    vec3 lightDir = normalize(lightPos - pos);
//    vec3 nnormal = normalize(normal);
//    float diff = max(dot(lightDir, nnormal), 0.0);
//    vec3 diffuse = diff * color;
//    // specular
//    vec3 viewDir = normalize(viewPos - pos);
//    vec3 reflectDir = reflect(-lightDir, nnormal);
//    float spec = 0.0;
//    if(blinn)
//    {
//        vec3 halfwayDir = normalize(lightDir + viewDir);  
//        spec = pow(max(dot(nnormal, halfwayDir), 0.0), 32.0);
//    }
//    else
//    {
//        vec3 reflectDir = reflect(-lightDir, nnormal);
//        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
//    }
//    vec3 specular = lightColor * spec; // assuming bright white light color
//    vec3 colorLinear = ambient + diffuse + specular;
//
//	// use the gamma corrected color in the fragment
//	FragColor = vec4(colorLinear, 1.0);
//}

void main() {
	PointLight plight;
	plight.position = lightPos;
	plight.constant = 1.0;
	plight.linear =  0.09;
	plight.quadratic = 0.032;
	plight.ambient = vec3(0.05);
	plight.diffuse = vec3(0.8);
	plight.specular = vec3(1.0);

	DirLight dlight;
	dlight.direction = normalize(vec3(-3, -1.5, 2));
	dlight.ambient = vec3(0.1, 0.05, 0.05);
	dlight.diffuse = vec3(0.6, 0.4, 0.4);
	dlight.specular = vec3(0.7, 0.5, 0.5);

	vec3 norm = normalize(normal);
	vec3 viewDir = normalize(viewPos - pos);

	vec3 result = CalcDirLight(dlight, norm, viewDir);
	result += CalcPointLight(plight, norm, pos, viewDir);

	FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
//    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 reflectDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // combine results
    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * color;
    vec3 specular = light.specular * spec * color;
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
//    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 reflectDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * color;
    vec3 specular = light.specular * spec * color;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}
