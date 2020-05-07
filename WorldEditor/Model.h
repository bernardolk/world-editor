#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

unsigned int load_texture_from_file(const char* path, const string& directory, bool gamma = false);

class Model
{
public:
    /*  Model Data */
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh> meshes;
    string directory;
    bool gammaCorrection;
    vector<float> boundingBox;
    unsigned int bb_vao, bb_vbo;


    /*  Functions   */
    // constructor, expects a filepath to a 3D model.
    Model(string const& path, bool gamma = false) : gammaCorrection(gamma)
    {
        loadModel(path);
        calculate_bounding_box();
        setup_bounding_box_mesh();
    }

    Model(Mesh mesh) {
        this->meshes.push_back(mesh);
        calculate_bounding_box();
        setup_bounding_box_mesh();
    }

    // draws the model, and thus all its meshes
    void Draw(Shader shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

private:

    void setup_bounding_box_mesh() {
        glGenVertexArrays(1, &bb_vao);
        glGenBuffers(1, &bb_vbo);
        glBindVertexArray(bb_vao);
        glBindBuffer(GL_ARRAY_BUFFER, bb_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * boundingBox.size(), &boundingBox[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void calculate_bounding_box() {
        float min_x = numeric_limits<float>::max();
        float min_y = numeric_limits<float>::max();
        float min_z = numeric_limits<float>::max();
        float max_x = numeric_limits<float>::min();
        float max_y = numeric_limits<float>::min();
        float max_z = numeric_limits<float>::min();


        vector<Mesh>::iterator mesh = meshes.begin();   
        for (mesh; mesh != meshes.end(); mesh++) {
            vector<Vertex>::iterator vertex = mesh->vertices.begin();
            for (vertex; vertex != mesh->vertices.end(); vertex++) {
                if (min_x > vertex->Position.x)
                    min_x = vertex->Position.x;
                if (max_x < vertex->Position.x)
                    max_x = vertex->Position.x;
                if (min_y > vertex->Position.y)
                    min_y = vertex->Position.y;
                if (max_y < vertex->Position.y)
                    max_y = vertex->Position.y;
                if (min_z > vertex->Position.z)
                    min_z = vertex->Position.z;
                if (max_z < vertex->Position.z)
                    max_z = vertex->Position.z;
            }
        } // This took 1 sec to scan... holy cow

        boundingBox = {
            min_x, min_y, min_z,    //0 0,0,0 
            max_x, min_y, min_z,    //1 1,0,0

            max_x, min_y, min_z,    //2 1,0,0
            max_x, min_y, max_z,    //3 1,0,1

            max_x, min_y, max_z,    //4 1,0,1
            min_x, min_y, max_z,    //5 0,0,1
            
            min_x, min_y, max_z,    //6 0,0,1
            min_x, min_y, min_z,    //7 0,0,0

            min_x, min_y, min_z,    //8 0,0,0
            min_x, max_y, min_z,    //9 0,1,0

            max_x, min_y, min_z,    //10 1,0,0
            max_x, max_y, min_z,    //11 1,1,0

            max_x, min_y, max_z,    //12 1,0,1
            max_x, max_y, max_z,    //13 1,1,1

            min_x, min_y, max_z,    //14 0,0,1
            min_x, max_y, max_z,    //15 0,1,1

            min_x, max_y, min_z,    //16 0,1,0
            max_x, max_y, min_z,    //17 1,1,0

            max_x, max_y, min_z,    //18 1,1,0
            max_x, max_y, max_z,    //19 1,1,1

            max_x, max_y, max_z,    //20 1,1,1
            min_x, max_y, max_z,    //21 0,1,1

            min_x, max_y, max_z,    //22 0,1,1
            min_x, max_y, min_z,    //23 0,1,0
        };
    }


    /*  Functions   */
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const& path)
    {
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        // retrieve the directory path of the filepath
        std::size_t ind = path.find_last_of('/');
        if(ind != std::string::npos)
            directory = path.substr(0, ind);
        else {
            ind = path.find_last_of('\\');
            directory = path.substr(0, ind);
        }
        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene)
    {
        // process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }

    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        // data to fill
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        // Walk through each of the mesh's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // normals
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
            // texture coordinates
            if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
            vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        // Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN

        // 1. diffuse maps
        vector<Texture> diffuseMaps = load_material_textures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        vector<Texture> specularMaps = load_material_textures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture> normalMaps = load_material_textures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = load_material_textures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures);
    }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> load_material_textures(aiMaterial* mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if (!skip)
            {   // if texture hasn't been loaded already, load it
                Texture texture;
                texture.id = load_texture_from_file(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
        }
        return textures;
    }
};


unsigned int load_texture_from_file(const char* filename, const string& directory, bool gamma)
{
    string name = string(filename);
  
    string path;
    if (path.substr(0, path.length() - 2) == "/")
        path = directory + name;
    else
        path = directory + "/" + name;

    //if (directory == "assets/models/stone_wall") {
    //    path = "assets/models/stone_wall/texture/ruins.jpg";
    //}

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
