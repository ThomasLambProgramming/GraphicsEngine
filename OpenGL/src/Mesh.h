#pragma once
#include "Shader.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

#include <string>
#include <vector>

using namespace std;

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec3 Tangent;
    glm::vec3 Binormal;
    glm::vec2 TextureCoords;
};
struct Texture
{
    unsigned int Id;
    string type;
    string path;
};
class Mesh
{
public:
    vector<Vertex> vertices;
    vector<unsigned int> indicies;
    vector<Texture> textures;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
    void Draw(Shader &shader);
private:
    unsigned int VAO, VBO, EBO;

    void SetupMesh();
};
