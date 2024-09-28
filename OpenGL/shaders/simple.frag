//a simple shader for colors (this is the fragmentation shader)
#version 330 core

out vec4 FragColor;
in vec4 fragTriColor;

void main()
{
    FragColor = fragTriColor;
}