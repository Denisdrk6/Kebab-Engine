#include "Application.h"
#include "MeshLoader.h"

//#include "assimp/cimport.h"
//#include "assimp/scene.h"
//#include "assimp/postprocess.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

MeshLoader* MeshLoader::instance = nullptr;

MeshLoader* MeshLoader::GetInstance()
{
    if (instance == nullptr) instance = new MeshLoader();
    return instance;
}

MeshLoader::MeshLoader()
{
}

MeshLoader::~MeshLoader()
{
    RELEASE(instance);
}

void MeshLoader::Draw(bool showNormals)
{       
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
 
    for (unsigned int i = 0; i < meshes.size(); ++i)
    {
        meshes[i].BeginDraw();
        meshes[i].Draw(showNormals);
        meshes[i].EndDraw();
    }

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

GameObject* MeshLoader::LoadModel(std::string path)
{
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LOG_CONSOLE("ERROR::ASSIMP:: %s", import.GetErrorString());
        return nullptr;
    }
    directory = path.substr(0, path.find_last_of('/'));

    GameObject* baseGO = new GameObject();

    ProcessNode(scene->mRootNode, scene, baseGO);

    return baseGO;
}

void MeshLoader::ProcessNode(aiNode* node, const aiScene* scene, GameObject* baseGO)
{
    // Process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        //meshes.push_back(ProcessMesh(mesh, scene));
        ProcessMesh(mesh, scene, baseGO);
    }
    // Then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, baseGO);
    }
}

std::vector<Tex> MeshLoader::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    std::vector<Tex> textures;
    /*for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        Texture texture;
        texture.id = TextureFromFile(str.C_Str(), directory);
        texture.type = typeName;
        texture.path = str.C_Str();
        textures.push_back(texture);
    }*/
    return textures;
}

void MeshLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, GameObject* baseGO)
{
    GameObject* go = new GameObject();

    ComponentMesh* meshComp = (ComponentMesh*)go->CreateComponent(ComponentType::MESH);

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    //std::vector<float2> texCoords;

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex vertex;
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        if (mesh->HasNormals())
        {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }
        
        if (mesh->mTextureCoords[0])
        {
            vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
        }

        vertices.push_back(vertex);
    }

    // Process indices
    for (int i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j)
            indices.push_back(face.mIndices[j]);
    }

    meshComp->SetData(vertices, indices, textures);

    aiVector3D scale, rotation, position;
    scene->mRootNode->mTransformation.Decompose(scale, rotation, position);

    float3 s = { scale.x,scale.y,scale.z };
    Quat r = { rotation.x, rotation.y, rotation.z, 1 };
    float3 p = { position.x, position.y, position.z };

    ComponentTransform* trans = (ComponentTransform*)go->GetComponent(ComponentType::TRANSFORM);
    trans->SetPosition(p);

    go->AddComponent(meshComp);
    go->SetParent(baseGO);

    baseGO->AddChild(go);


    ///*if (mesh->mMaterialIndex > 0)
    //{
    //    aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

    //    aiString str;
    //    for (unsigned int i = 0; i < mat->GetTextureCount(aiTextureType_DIFFUSE); ++i)
    //    {
    //        mat->GetTexture(aiTextureType_DIFFUSE, i, &str);
    //        Texture* texDiffuse = app->textures->CreateTexture(str.C_Str());

    //        diffuseMaps.push_back(*texDiffuse);
    //        textures.push_back(*texDiffuse);
    //    }
    //    for (unsigned int i = 0; i < mat->GetTextureCount(aiTextureType_SPECULAR); ++i)
    //    {
    //        mat->GetTexture(aiTextureType_DIFFUSE, i, &str);
    //        Texture* texSpecular = app->textures->CreateTexture(str.C_Str());

    //        specularMaps.push_back(*texSpecular);
    //        textures.push_back(*texSpecular);
    //    }
    //}*/
    //return KbMesh(vertices, indices, textures, texCoords);
}