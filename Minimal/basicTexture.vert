#version 330 core
uniform mat4 Projection = mat4(1);
uniform mat4 View = mat4(1);
uniform mat4 Model = mat4(1);

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 texCoord;

out vec2 TexCoord;

void main()
{
	mat4 ViewXfm = View * Model;
	gl_Position = Projection * ViewXfm * vec4(Position, 1.0);
	TexCoord = texCoord;
}