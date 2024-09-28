#version 330 core
layout (location = 0) in vec3 aPos; 
 
uniform vec4 triangleColor; 
out vec4 fragTriColor;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    fragTriColor = triangleColor;
    gl_Position = projectionMatrix * viewMatrix * worldMatrix * vec4(aPos, 1.0f);
}
