#include "Model.h"
#include <assimp/postprocess.h>
#include <GL/gl.h>
#include <GL/glew.h>
#include <stb_image/stb_image.h>

void Model::LoadFile(string filePath)
{
    Assimp::Importer importer;
    //aiProcess->Triangulate turns points that arent triangles into triangles, uv is to flip them to be y=up if they arent, gen normals is for making normals if they are not currently present.
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_EmbedTextures);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ASSIMP ERROR: " << importer.GetErrorString() << std::endl;
        return;
    }
    directory = filePath.substr(0, filePath.find_last_of('/'));
    ProcessNode(scene->mRootNode, scene);
    aiSceneModel = scene;
}

void Model::Draw(Shader& shader)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
        meshes.push_back(ProcessMesh(scene->mMeshes[node->mMeshes[i]], scene));
    for (unsigned int i = 0; i < node->mNumChildren; i++)
        ProcessNode(node->mChildren[i], scene);
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        vertex.Position.x = mesh->mVertices[i].x;
        vertex.Position.y = mesh->mVertices[i].y;
        vertex.Position.z = mesh->mVertices[i].z;

        if (mesh->HasNormals())
        {
            vertex.Normal.x = mesh->mNormals[i].x;
            vertex.Normal.y = mesh->mNormals[i].y;
            vertex.Normal.z = mesh->mNormals[i].z;
        }

        if (mesh->HasTextureCoords(0))
        {
            //vertex can contain up to 8 texture coords (not really seen that used much probably is common)
            //we assume the first set for now.
            vertex.TextureCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.TextureCoords.y = mesh->mTextureCoords[0][i].y;
        }
        else
        {
            vertex.TextureCoords.x = 0;
            vertex.TextureCoords.y = 0;
        }
        if (mesh->HasTangentsAndBitangents())
        {
            vertex.Tangent.x = mesh->mTangents[i].x;
            vertex.Tangent.y = mesh->mTangents[i].y;
            vertex.Tangent.z = mesh->mTangents[i].z;
        
            vertex.Binormal.x = mesh->mBitangents[i].x;
            vertex.Binormal.y = mesh->mBitangents[i].y;
            vertex.Binormal.z = mesh->mBitangents[i].z;
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
            indices.push_back(mesh->mFaces[i].mIndices[j]);
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    vector<Texture> diffuseMaps = LoadMaterialTextures(scene, material, aiTextureType_BASE_COLOR, "texture_diffuse");
    if (!diffuseMaps.empty())
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    
    return Mesh(vertices, indices, textures);
}

vector<Texture> Model::LoadMaterialTextures(const aiScene* scene, aiMaterial* material, aiTextureType type, string typeName)
{
    vector<Texture> textures;

    if(material->GetTextureCount(type) <= 0)
        return vector<Texture>();
        
    std::cout << "This model has textures " << std::endl;
    aiString path;
    
    if (material->GetTexture(type, 0, &path) == AI_FAILURE)
        return vector<Texture>();
            
    if (const aiTexture* texture = scene->GetEmbeddedTexture(path.C_Str()))
    {
        std::cout << "Material get texture successful " << std::endl;
        int width, height;
        int channelsInFile= 4;
        unsigned char* data = stbi_load_from_memory((const stbi_uc*)texture->pcData, texture->mWidth, &width, &height, &channelsInFile, 3);

        if (data != nullptr)
        {
            unsigned int textureID;
            glGenTextures(1, &textureID);

            GLenum format;
            switch (channelsInFile)
            {
                case 1:
                    format = GL_RED;
                break;
                case 3:
                    format = GL_RGB;
                break;
                case 4:
                    format = GL_RGBA;
                break;
                default:
                    std::cout << "Format is default somehow in Load Material Textures";
                break;
            }
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            stbi_image_free(data);
            
            Texture newTex;
            //for now we are only getting diffuse so embedded is the 0th element not a filepath.
            newTex.path = "0";
            newTex.Id = textureID;
            newTex.type = typeName;
            
            vector<Texture> newTextures;
            newTextures.push_back(newTex);
            return newTextures;
        }
        stbi_image_free(data);
    }
    return vector<Texture>();
}