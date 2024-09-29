#version 330 core
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aNormal; 
layout (location = 2) in vec2 aTexCoord; 

out vec2 TexCoord;
out float specular;

uniform vec3 mainLightDirection; 
uniform vec3 cameraPosition; 
uniform vec4 triangleColor; 
uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    TexCoord = aTexCoord;
    gl_Position = projectionMatrix * viewMatrix * worldMatrix * vec4(aPos, 1.0f);
    
    vec3 viewDir = normalize(cameraPosition - aPos);
    vec3 halfwayDir = normalize(-mainLightDirection + viewDir);
    specular = max(dot(aNormal, halfwayDir), 0.0);
}
