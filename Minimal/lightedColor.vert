#version 410 core

uniform mat4 ProjectionMatrix = mat4(1);
uniform mat4 CameraMatrix = mat4(1);
//uniform mat4 InstanceTransform = mat4(1);
uniform mat4 ModelMatrix = mat4(1);

layout(location = 0) in vec4 Position;
layout(location = 2) in vec3 Normal;
layout(location = 3) in vec4 Color;
layout(location = 5) in mat4 InstanceTransform;

out vec3 fragNormal;
out vec3 fragPos;
out vec4 color;

void main(void) {
   mat4 ViewXfm = CameraMatrix * ModelMatrix * InstanceTransform;
   fragNormal = mat3(transpose(inverse(ModelMatrix * InstanceTransform))) * Normal;
   fragPos = vec3(ModelMatrix * InstanceTransform * Position);
   color = Color;
   gl_Position = ProjectionMatrix * ViewXfm * Position;
}
