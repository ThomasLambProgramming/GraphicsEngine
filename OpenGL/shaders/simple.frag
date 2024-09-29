//a simple shader for colors (this is the fragmentation shader)
#version 330 core

uniform sampler2D colorTexture;

out vec4 FragColor;
in vec2 TexCoord;
in float specular;
void main()
{
    FragColor = specular * texture(colorTexture, TexCoord);
    FragColor.a = 1.0;
}