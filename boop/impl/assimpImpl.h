#ifndef ASSIMPIMPL_H
#define ASSIMPIMPL_H

#pragma once

#include <assimp/scene.h>
#include "../common/model.h"

template<typename VertexFormat>
class AssimpLoader
{
public:
    // Returns the number of indices in the loaded model
    uint64_t index_count() const {return indices.size();}

    // Returns the number of vertices in the loaded model
    uint64_t vertex_count() const {return vertices.size();}

    // Returns the number of meshes in the loaded model
    uint64_t mesh_count() const {return mesh_infos.size();}

    VertexFormat* vertex_data() {return vertices.data();}

    uint32_t* index_data() {return indices.data();}

    std::vector<boop::ModelMeshInfo> mesh_infos;

    ~AssimpLoader(){}

    void load_scene(const aiScene *scene)
    {
        uint32_t mesh_count = scene->mNumMeshes;
        uint32_t material_count = scene->mNumMaterials;

        process_node_recursive(scene->mRootNode, scene);
    }

private:
    void process_node_recursive(aiNode *node, const aiScene *scene)
    {
        // Load Meshes
        for (uint32_t i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            process_mesh(mesh, scene);
        }

        // TODO: Load materials

        // Call for children
        for (uint32_t i = 0; i < node->mNumChildren; i++)
        {
            process_node_recursive(node->mChildren[i], scene);
        }
    }

    void process_mesh(aiMesh *mesh, const aiScene *scene)
    {
        for (uint32_t i = 0; i < mesh->mNumVertices; i++)
        {
            aiVector3D position = mesh->mVertices[i];
            aiVector3D normal = mesh->mNormals[i];

            VertexFormat vertex = {};

            vertex.position[0] = position.x;
            vertex.position[1] = position.y;
            vertex.position[2] = position.z;

            vertex.normal[0] = normal.x;
            vertex.normal[1] = normal.y;

            if (mesh->HasTextureCoords(0))
            {
                aiVector3D uv = mesh->mTextureCoords[0][i];
				vertex.uv[0] = uv.x;
				vertex.uv[1] = uv.y;
            }
            else
            {
				vertex.uv[0] = 0;
				vertex.uv[1] = 0;
            }

            vertices.push_back(vertex);
        }

        int mesh_index_count = 0;
        for (uint32_t i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];

            for (uint32_t j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
                mesh_index_count++;
            }
        }
		auto vertex_format_enum = boop::VertexFormat::F32_PNCV;

        boop::ModelMeshInfo mesh_info = {};
        mesh_info.vertex_buffer_size = mesh->mNumVertices * sizeof(VertexFormat);
        mesh_info.index_buffer_size = mesh_index_count * sizeof(uint32_t);
        mesh_info.mesh_name = mesh->mName.data;

        mesh_infos.push_back(mesh_info);
    }
private:
    std::vector<VertexFormat> vertices;
    std::vector<uint32_t> indices;
};

#endif