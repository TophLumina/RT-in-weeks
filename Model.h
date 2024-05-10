#pragma once

#include "Mesh.h"
#include "hittable_list.h"
#include "rtweekend.h"

#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Model : public hittable_list
{
public:
    std::vector<shared_ptr<Mesh>> meshes;
    std::string directory;

    Model(const char *path)
    {
        loadModel(path);
    }
private:
    std::vector<shared_ptr<MeshTexture>> textures_cached;

    void loadModel(std::string path)
    {
        Assimp::Importer importer;
        // TODO:: Find out if there is a need for uv flip
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::clog << "ASSIMP: " << importer.GetErrorString() << std::endl;
            return;
        }

        directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene);

        for (auto &mesh : meshes)
            add(std::static_pointer_cast<hittable_list>(mesh));
    }

    void processNode(aiNode *node, const aiScene *scene)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; ++i)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }

        for (unsigned int i = 0; i < node->mNumChildren; ++i)
            processNode(node->mChildren[i], scene);
    }

    shared_ptr<Mesh> processMesh(aiMesh *mesh, const aiScene *scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<shared_ptr<MeshTexture>> textures;

        // Vertex
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            Vertex vertex;
            vec3 position;

            position.x = mesh->mVertices[i].x;
            position.y = mesh->mVertices[i].y;
            position.z = mesh->mVertices[i].z;

            vertex.Position = position;

            if (mesh->HasNormals())
            {
                vec3 normal;

                normal.x = mesh->mNormals[i].x;
                normal.y = mesh->mNormals[i].y;
                normal.z = mesh->mNormals[i].z;

                vertex.Normal = normal;
            }

            if (mesh->mTextureCoords[0])
            {
                vec2 uv;

                uv.x = mesh->mTextureCoords[0][i].x;
                uv.y = mesh->mTextureCoords[0][i].y;

                vertex.uv = uv;

                if (mesh->HasTangentsAndBitangents())
                {
                    vec3 tangent;
                    vec3 bitangent;

                    tangent.x = mesh->mTangents[i].x;
                    tangent.y = mesh->mTangents[i].y;
                    tangent.z = mesh->mTangents[i].z;

                    bitangent.x = mesh->mBitangents[i].x;
                    bitangent.y = mesh->mBitangents[i].y;
                    bitangent.z = mesh->mBitangents[i].z;

                    vertex.Tangent = tangent;
                    vertex.BiTangent = bitangent;
                }
            }
            else
                vertex.uv = vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j)
                indices.push_back(face.mIndices[j]);
        }

        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
            std::vector<shared_ptr<MeshTexture>> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            std::vector<shared_ptr<MeshTexture>> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            std::vector<shared_ptr<MeshTexture>> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        }

        return make_shared<Mesh>(vertices, indices, textures);
    }

    std::vector<shared_ptr<MeshTexture>> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
    {
        std::vector<shared_ptr<MeshTexture>> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
        {
            aiString path;
            mat->GetTexture(type, i, &path);

            bool skip = false;
            for (unsigned int j = 0; j < textures_cached.size(); ++j)
            {
                if (std::strcmp(textures_cached[j]->path.data(), path.C_Str()) == 0)
                {
                    textures.push_back(textures_cached[j]);
                    skip = true;
                    break;
                }
            }

            if (!skip)
            {
                shared_ptr<MeshTexture> texture = make_shared<MeshTexture>(path.C_Str(), directory);
                texture->type = typeName;
                texture->path = path.C_Str();
                textures.push_back(texture);
                textures_cached.push_back(texture);
            }
        }

        return textures;
    }
};