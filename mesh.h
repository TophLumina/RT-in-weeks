#pragma once

#include "Vertex.h"
#include "hittable_list.h"
#include "material.h"
#include "rtweekend.h"
#include "texture.h"
#include "triangle.h"

#include <assimp/Importer.hpp>
#include <assimp/types.h>

#include <memory>
#include <string>
#include <vector>

struct MeshTexture : public image_texture
{
    std::string type;
    std::string path;
    MeshTexture(const std::string name, const std::string directory) : path(directory + '/' + name), image_texture((directory + '/' + name).c_str()) {}
};

class Mesh : public hittable_list
{
public:
    shared_ptr<material> material;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<shared_ptr<MeshTexture>> textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<shared_ptr<MeshTexture>> textures) : vertices(vertices), indices(indices), textures(textures)
    {
        setupMesh();
    }
private:
    void setupMesh()
    {
        loadTextures();

        for (unsigned int i = 0; i < indices.size(); i += 3)
        {
            auto a = vertices[indices[i]];
            auto b = vertices[indices[i + 1]];
            auto c = vertices[indices[i + 2]];

            add(make_shared<triangle>(a, b, c, material));
        }
    }

    // TODO: Improve texture loading when use BRDFs
    void loadTextures()
    {
        unsigned int diffuseIdx = 0;
        // unsigned int specularIdx = 0;
        // unsigned int normalIdx = 0;
        for (unsigned int i = 0; i < textures.size(); ++i)
        {
            if (textures[i]->type == "texture_diffuse")
            {
                diffuseIdx = i;
                material = make_shared<lambertian>(std::static_pointer_cast<Texture>(textures[diffuseIdx]));
                break;
            }
        }

        if (!material)
        {
            material = make_shared<lambertian>(color(1, 0, 1));
        }
    }
};