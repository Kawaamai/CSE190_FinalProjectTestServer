#version 410 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 fragPos;

uniform vec3 eyePos;
uniform vec3 lightPos;
uniform sampler2D tex;

void main()
{
	vec3 eyeV = normalize(eyePos - fragPos);
	vec3 lightV = normalize(lightPos - fragPos);
	float angle = -min(dot(eyeV, lightV), 0.0);
	
	FragColor = angle * texture(tex, TexCoord);
}
