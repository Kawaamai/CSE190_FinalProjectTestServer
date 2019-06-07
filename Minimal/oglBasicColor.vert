#version 410 core

uniform mat4 ProjectionMatrix = mat4(1);
uniform mat4 CameraMatrix = mat4(1);
//uniform mat4 InstanceTransform = mat4(1);
//uniform mat4 MModelMatrix = mat4(1);

layout(location = 0) in vec4 Position;
layout(location = 2) in vec3 Normal;
layout(location = 3) in vec4 Color;
layout(location = 5) in mat4 InstanceTransform;

out vec3 vertNormal;
out vec4 color;

void main(void) {
//    mat4 ViewXfm = CameraMatrix * InstanceTransform * MModelMatrix;
    mat4 ViewXfm = CameraMatrix * InstanceTransform;
    vertNormal = Normal;
	color = Color;
//    gl_Position = ProjectionMatrix * ViewXfm * vec4(Position, 1.0);
    gl_Position = ProjectionMatrix * ViewXfm * Position;
}
