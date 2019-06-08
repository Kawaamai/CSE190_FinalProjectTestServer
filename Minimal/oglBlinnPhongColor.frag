#version 410 core

in vec3 normal;
in vec3 pos;

//uniform int mode = 1;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 color;
uniform bool blinn = true;

//const vec3 lightPos = vec3(1.0,1.0,1.0);
//const vec3 lightColor = vec3(1.0, 1.0, 1.0);
//const float lightPower = 40.0;
//const vec3 ambientColor = vec3(0.1, 0.0, 0.0);
//const vec3 diffuseColor = vec3(0.5, 0.0, 0.0);
//const vec3 specColor = vec3(1.0, 1.0, 1.0);
//const float shininess = 16.0;
const float ambientStrength = 0.005;
const vec3 lightColor = vec3(0.1);
const float screenGamma = 2.2; // Assume the monitor is calibrated to the sRGB color space
const bool colorCorrect = false;

out vec4 FragColor;

void main() {
    // ambient
    vec3 ambient = ambientStrength * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - pos);
    vec3 nnormal = normalize(normal);
    float diff = max(dot(lightDir, nnormal), 0.0);
    vec3 diffuse = diff * color;
    // specular
    vec3 viewDir = normalize(viewPos - pos);
    vec3 reflectDir = reflect(-lightDir, nnormal);
    float spec = 0.0;
    if(blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(nnormal, halfwayDir), 0.0), 32.0);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, nnormal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    }
    vec3 specular = lightColor * spec; // assuming bright white light color
    vec3 colorLinear = ambient + diffuse + specular;
	// apply gamma correction (assume ambientColor, diffuseColor and specColor
	// have been linearized, i.e. have no gamma correction in them)
	vec3 colorGammaCorrected;

	if (colorCorrect) {
		colorGammaCorrected = pow(colorLinear, vec3(1.0/screenGamma));
	} else {
		colorGammaCorrected = colorLinear;
	}

	// use the gamma corrected color in the fragment
	FragColor = vec4(colorGammaCorrected, 1.0);
}