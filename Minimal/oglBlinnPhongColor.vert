#version 410 core

uniform mat4 ProjectionMatrix = mat4(1);
uniform mat4 CameraMatrix = mat4(1);
uniform mat4 ModelMatrix = mat4(1);
uniform mat4 LightSpaceMatrix = mat4(1);

layout(location = 0) in vec4 Position;
layout(location = 2) in vec3 Normal;
layout(location = 5) in mat4 InstanceTransform;

out vec3 normal;
out vec3 pos;
out vec4 lightSpacePos;

void main() {
   mat4 ViewXfm = CameraMatrix * ModelMatrix * InstanceTransform;
   normal = mat3(transpose(inverse(ModelMatrix * InstanceTransform))) * Normal;
   pos = vec3(ModelMatrix * InstanceTransform * Position);
   lightSpacePos = LightSpaceMatrix * vec4(pos, 1.0f);
   gl_Position = ProjectionMatrix * ViewXfm * Position;
}