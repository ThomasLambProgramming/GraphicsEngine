#pragma once
#include "Mesh.h"
#include <vector>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

class Model
{
public:
    std::vector<Mesh> meshes;
    string directory;

    void LoadFile(string filePath);
    void Draw(Shader& shader);

private:
    vector<Texture> textures_loaded;
    
    void ProcessNode(aiNode* node, const aiScene* scene);
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
    vector<Texture> LoadMaterialTextures(aiMaterial* material, aiTextureType type, string typeName);
    unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);
};
