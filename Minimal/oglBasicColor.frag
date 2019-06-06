#version 410 core

//uniform vec4 Color = vec4(.4);
//layout(location = 3) in vec4 Color;

in vec3 vertNormal;
in vec4 color;
out vec4 fragColor;

void main(void) {
//    fragColor = Color;
    fragColor = color;
}
