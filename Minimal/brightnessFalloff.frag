#version 410 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 fragPos;
in vec3 fragNormal;

uniform vec3 eyePos;
uniform sampler2D tex;

void main()
{
	vec3 norm = normalize(fragNormal);
	vec3 eyeDir = normalize(eyePos - fragPos);
	float angle = max(dot(norm, eyeDir), 0.0);

	//FragColor = texture(tex, TexCoord);
	FragColor = angle * texture(tex, TexCoord);
}
