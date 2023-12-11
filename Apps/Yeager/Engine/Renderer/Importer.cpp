#include "Importer.h"
#include "TextureHandle.h"
using namespace Yeager;

unsigned int Importer::m_ImportedModelsCount = 0;

Importer::Importer(YgString source) : m_Source(source)
{
  Yeager::Log(INFO, "Intialize Importer from {}", m_Source.c_str());
}

Importer::~Importer()
{
  Yeager::Log(INFO, "Destrorying Importer from {}", m_Source.c_str());
}
ObjectModelData Importer::Import(YgCchar path, bool flip_image, unsigned int assimp_flags)
{
  m_ImageFlip = flip_image;
  ObjectModelData data;
  Assimp::Importer imp;
  const aiScene* scene = imp.ReadFile(path, assimp_flags);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    Yeager::Log(ERROR, "Cannot load imported model! Path {}, Error {}", path, imp.GetErrorString());
    return data;
  }
  m_FullPath = path;
  ProcessNode(scene->mRootNode, scene, &data);
  data.SuccessfulLoaded = true;
  return data;
}

void Importer::ProcessNode(aiNode* node, const aiScene* scene, ObjectModelData* data)
{
  for (unsigned int x = 0; x < node->mNumMeshes; x++) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[x]];
    data->Meshes.push_back(ProcessMesh(mesh, scene, data));
  }

  for (unsigned int x = 0; x < node->mNumChildren; x++) {
    ProcessNode(node->mChildren[x], scene, data);
  }
}

ObjectMeshData Importer::ProcessMesh(aiMesh* mesh, const aiScene* scene, ObjectModelData* data)
{
  std::vector<ObjectVertexData> vertices;
  std::vector<GLuint> indices;
  std::vector<ObjectTexture> textures;

  for (unsigned int x = 0; x < mesh->mNumVertices; x++) {
    ObjectVertexData vertex;
    YgVector3 vector;

    vector.x = mesh->mVertices[x].x;
    vector.y = mesh->mVertices[x].y;
    vector.z = mesh->mVertices[x].z;
    vertex.Position = vector;

    vector.x = mesh->mNormals[x].x;
    vector.y = mesh->mNormals[x].y;
    vector.z = mesh->mNormals[x].z;
    vertex.Normals = vector;

    if (mesh->mTextureCoords[0]) {  // has texture coords
      YgVector2 vec;
      vec.x = mesh->mTextureCoords[0][x].x;
      vec.y = mesh->mTextureCoords[0][x].y;
      vertex.TextureCoords = vec;
    } else {  // No texture coords
      vertex.TextureCoords = YgVector2(0.0f, 0.0f);
    }
    vertices.push_back(vertex);
  }

  for (unsigned int x = 0; x < mesh->mNumFaces; x++) {
    aiFace face = mesh->mFaces[x];
    for (unsigned int y = 0; y < face.mNumIndices; y++) {
      indices.push_back(face.mIndices[y]);
    }
  }

  if (mesh->mMaterialIndex >= 0) {
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<ObjectTexture> diffuseMaps =
        LoadMaterialTexture(material, aiTextureType_DIFFUSE, "texture_diffuse", data);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    std::vector<ObjectTexture> specularMaps =
        LoadMaterialTexture(material, aiTextureType_SPECULAR, "texture_specular", data);
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
  }

  return ObjectMeshData(indices, vertices, textures);
}

std::vector<ObjectTexture> Importer::LoadMaterialTexture(aiMaterial* material, aiTextureType type, YgString typeName,
                                                         CommonModelData* data)
{
  std::vector<ObjectTexture> textures;
  for (unsigned int x = 0; x < material->GetTextureCount(type); x++) {
    aiString str;
    material->GetTexture(type, x, &str);
    bool skip = false;
    for (unsigned int y = 0; y < data->TexturesLoaded.size(); y++) {
      YgString cmp_path = RemoveSuffixUntilCharacter(m_FullPath, YG_PS);
      cmp_path += str.C_Str();
      if (std::strcmp(data->TexturesLoaded[y].Path.data(), cmp_path.data()) == 0) {
        textures.push_back(data->TexturesLoaded[y]);
        skip = true;
        break;
      }
    }

    if (!skip) {
      YgString path_suffix_removed = RemoveSuffixUntilCharacter(m_FullPath, YG_PS);
      YgString path = path_suffix_removed + str.C_Str();

      ObjectTexture tex;
      tex.ID = LoadTextureFromFile(path, m_ImageFlip);
      tex.Name = typeName.c_str();
      tex.Path = path;
      textures.push_back(tex);
      data->TexturesLoaded.push_back(tex);
    }
  }
  return textures;
}

AnimatedObjectModelData Importer::ImportAnimated(YgCchar path, bool flip_image, unsigned int assimp_flags)
{
  m_ImageFlip = flip_image;
  AnimatedObjectModelData data;
  Assimp::Importer imp;
  const aiScene* scene = imp.ReadFile(path, assimp_flags);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    Yeager::Log(ERROR, "Cannot load imported model! Path {}, Error {}", path, imp.GetErrorString());
    return data;
  }
  m_FullPath = path;
  ProcessAnimatedNode(scene->mRootNode, scene, &data);
  data.SuccessfulLoaded = true;
  return data;
}

void Importer::ProcessAnimatedNode(aiNode* node, const aiScene* scene, AnimatedObjectModelData* data)
{
  for (unsigned int x = 0; x < node->mNumMeshes; x++) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[x]];
    data->Meshes.push_back(ProcessAnimatedMesh(mesh, scene, data));
  }

  for (unsigned int x = 0; x < node->mNumChildren; x++) {
    ProcessAnimatedNode(node->mChildren[x], scene, data);
  }
}

AnimatedObjectMeshData Importer::ProcessAnimatedMesh(aiMesh* mesh, const aiScene* scene, AnimatedObjectModelData* data)
{
  std::vector<AnimatedVertexData> vertices;
  std::vector<GLuint> indices;
  std::vector<ObjectTexture> textures;

  for (unsigned int x = 0; x < mesh->mNumVertices; x++) {
    AnimatedVertexData vertex;
    SetVertexBoneDataToDefault(vertex);
    YgVector3 vector;

    vector.x = mesh->mVertices[x].x;
    vector.y = mesh->mVertices[x].y;
    vector.z = mesh->mVertices[x].z;
    vertex.Position = vector;

    vector.x = mesh->mNormals[x].x;
    vector.y = mesh->mNormals[x].y;
    vector.z = mesh->mNormals[x].z;
    vertex.Normals = vector;

    if (mesh->mTextureCoords[0]) {  // has texture coords
      YgVector2 vec;
      vec.x = mesh->mTextureCoords[0][x].x;
      vec.y = mesh->mTextureCoords[0][x].y;
      vertex.TextureCoords = vec;
    } else {  // No texture coords
      vertex.TextureCoords = YgVector2(0.0f, 0.0f);
    }
    vertices.push_back(vertex);
  }

  for (unsigned int x = 0; x < mesh->mNumFaces; x++) {
    aiFace face = mesh->mFaces[x];
    for (unsigned int y = 0; y < face.mNumIndices; y++) {
      indices.push_back(face.mIndices[y]);
    }
  }

  if (mesh->mMaterialIndex >= 0) {
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<ObjectTexture> diffuseMaps =
        LoadMaterialTexture(material, aiTextureType_DIFFUSE, "texture_diffuse", data);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    std::vector<ObjectTexture> specularMaps =
        LoadMaterialTexture(material, aiTextureType_SPECULAR, "texture_specular", data);
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    std::vector<ObjectTexture> normalMaps = LoadMaterialTexture(material, aiTextureType_HEIGHT, "texture_normal", data);
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    std::vector<ObjectTexture> heightMaps =
        LoadMaterialTexture(material, aiTextureType_AMBIENT, "texture_height", data);
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
  }
  ExtractBoneWeightForVertices(vertices, mesh, scene, data);

  return AnimatedObjectMeshData(indices, vertices, textures);
}

void Importer::SetVertexBoneDataToDefault(AnimatedVertexData& vertex)
{
  for (int x = 0; x < MAX_BONE_INFLUENCE; x++) {
    vertex.BonesIDs[x] = -1;
    vertex.Weights[x] = 0.0f;
  }
}

void Importer::SetVertexBoneData(AnimatedVertexData& vertex, int boneID, float weight)
{
  for (int x = 0; x < MAX_BONE_INFLUENCE; ++x) {
    if (vertex.BonesIDs[x] < 0) {
      vertex.Weights[x] = weight;
      vertex.BonesIDs[x] = boneID;
      break;
    }
  }
}

void Importer::ExtractBoneWeightForVertices(std::vector<AnimatedVertexData>& vertices, aiMesh* mesh,
                                            const aiScene* scene, AnimatedObjectModelData* data)
{
  auto& BoneInfoMap = data->m_BoneInfoMap;
  int& BoneCount = data->m_BoneCounter;

  for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
    int boneID = -1;
    const char* cc = mesh->mBones[boneIndex]->mName.C_Str();
    YgString boneName = YgString(cc);
    if (BoneInfoMap.find(boneName) == BoneInfoMap.end()) {
      BoneInfo newBoneInfo;
      newBoneInfo.ID = BoneCount;
      newBoneInfo.OffSet = ConvertAssimpMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
      BoneInfoMap[boneName] = newBoneInfo;
      boneID = BoneCount;
      BoneCount++;
    } else {
      boneID = BoneInfoMap[boneName].ID;
    }
    assert(boneID != -1);
    auto weights = mesh->mBones[boneIndex]->mWeights;
    int NumWeights = mesh->mBones[boneIndex]->mNumWeights;

    for (int weightIndex = 0; weightIndex < NumWeights; ++weightIndex) {
      int vertexId = weights[weightIndex].mVertexId;
      float weight = weights[weightIndex].mWeight;
      assert(vertexId <= vertices.size());
      SetVertexBoneData(vertices[vertexId], boneID, weight);
    }
  }
}