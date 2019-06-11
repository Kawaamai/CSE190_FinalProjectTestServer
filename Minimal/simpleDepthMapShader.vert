#version 410 core

uniform mat4 ProjectionMatrix = mat4(1);
//uniform mat4 CameraMatrix = mat4(1);
uniform mat4 ModelMatrix = mat4(1);
uniform mat4 InstanceTransform = mat4(1);

layout(location = 0) in vec4 Position;
layout(location = 1) in vec3 Normal;

void main() {
   mat4 ViewXfm = ModelMatrix * InstanceTransform;
   gl_Position = ProjectionMatrix * ViewXfm * Position;
}
