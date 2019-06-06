#version 410 core

uniform mat4 Projection = mat4(1);
uniform mat4 View = mat4(1);
uniform mat4 Model = mat4(1);
uniform vec3 Normal = vec3(0.0, 0.0, 1.0);

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 texCoord;

out vec3 fragNormal;
out vec3 fragPos;
out vec2 TexCoord;

void main(void) {
	mat4 ViewXfm = View * Model;
	gl_Position = Projection * ViewXfm * vec4(Position, 1.0);
	TexCoord = texCoord;
	fragPos = vec3(Model * vec4(Position, 1.0));
	fragNormal = mat3(transpose(inverse(Model))) * Normal;
	//fragNormal = Normal;
}
