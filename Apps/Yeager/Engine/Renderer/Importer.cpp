#include "Importer.h"
#include "../../Application.h"
#include "TextureHandle.h"

#include "stb_image.h"

using namespace Yeager;

unsigned int Importer::m_ImportedModelsCount = 0;

Importer::Importer(YgString source, ApplicationCore* app) : m_Source(source), m_Application(app)
{
  Yeager::Log(INFO, "Intialize Importer from {}", m_Source.c_str());
  if (app == YEAGER_NULLPTR) {
    Yeager::Log(WARNING, "The importer application pointer have not been set!");
  }
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
  std::vector<ObjectTexture*> textures;

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
    std::vector<ObjectTexture*> diffuseMaps =
        LoadMaterialTexture(material, aiTextureType_DIFFUSE, "texture_diffuse", data);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    std::vector<ObjectTexture*> specularMaps =
        LoadMaterialTexture(material, aiTextureType_SPECULAR, "texture_specular", data);
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    std::vector<ObjectTexture*> metallicMaps =
        LoadMaterialTexture(material, aiTextureType_METALNESS, "texture_metallic", data);
    textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());
    std::vector<ObjectTexture*> roughnessMaps =
        LoadMaterialTexture(material, aiTextureType_DIFFUSE_ROUGHNESS, "texture_roughness", data);
    textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());
  }

  return ObjectMeshData(indices, vertices, textures);
}

std::vector<ObjectTexture*> Importer::LoadMaterialTexture(aiMaterial* material, aiTextureType type, YgString typeName,
                                                          CommonModelData* data)
{
  std::vector<ObjectTexture*> textures;
  for (unsigned int x = 0; x < material->GetTextureCount(type); x++) {
    aiString str;
    material->GetTexture(type, x, &str);
    bool skip = false;
    for (unsigned int y = 0; y < data->TexturesLoaded.size(); y++) {
      YgString cmp_path = RemoveSuffixUntilCharacter(m_FullPath, YG_PS);
      if (Yeager::ValidatesPath(str.C_Str(), false)) {
        // Texture path in the mtl file is a complete path, we just assign the complete path to the compare_path without adding to it
        cmp_path = str.C_Str();
      } else {
        cmp_path += str.C_Str();
      }
      if (std::strcmp(data->TexturesLoaded[y]->first.Path.data(), cmp_path.data()) == 0) {
        ObjectTexture* rt = &data->TexturesLoaded[y]->first;
        textures.push_back(rt);
        skip = true;
        break;
      }
    }

    if (!skip) {
      YgString path_suffix_removed = RemoveSuffixUntilCharacter(m_FullPath, YG_PS);
      YgString path;
      if (Yeager::ValidatesPath(str.C_Str(), false)) {
        path = str.C_Str();
      } else {
        path = path_suffix_removed + str.C_Str();
      }
/* Checks if the texture path written in the mtl file isnt using / on windows build*/
#ifdef YEAGER_SYSTEM_WINDOWS_x64
      std::replace(path.begin(), path.end(), '/', '\\');
#endif
      auto tex = std::make_shared<std::pair<ObjectTexture, STBIDataOutput*>>();
      /* If the texture loading have been called in a thread without the openGL context loaded intro to it, the texture id will ALWAYS be 0, meaning it wont load, 
        we check if the current thread is with the openGL context, if not, the boolean incompleteID is set to true, and the texture loading is done after the thread is finished! */
      if (!m_Application->GetWindow()->CheckIfOpenGLContext()) {
        tex->first.ImcompleteId = true;
        tex->second = LoadStbiDataOutput(path, m_ImageFlip);
#ifdef YEAGER_DEBUG
        Yeager::Log(INFO, "Thread trying to use openGL context");
#endif
      } else {
        tex->first.ID = LoadTextureFromFile(path, m_ImageFlip);
      }
      tex->first.Name = typeName.c_str();
      tex->first.Path = path;
      ObjectTexture* rt = &tex->first;
      textures.push_back(rt);
      data->TexturesLoaded.push_back(tex);
    }
  }
  return textures;
}

STBIDataOutput* Importer::LoadStbiDataOutput(YgString path, bool flip)
{
  STBIDataOutput* output = new STBIDataOutput;

  stbi_set_flip_vertically_on_load(flip);
  ValidatesPath(path);
  output->Data = stbi_load(path.c_str(), &output->Width, &output->Height, &output->NrComponents, 0);
  output->OriginalPath = path;

  if (output->Data == YEAGER_NULLPTR) {
    Yeager::Log(ERROR, "Cannot load data to STBIDataOutput! Path: {}, Reason {}", path, stbi_failure_reason());
  }

  return output;
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
  std::vector<ObjectTexture*> textures;

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
    std::vector<ObjectTexture*> diffuseMaps =
        LoadMaterialTexture(material, aiTextureType_DIFFUSE, "texture_diffuse", data);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    std::vector<ObjectTexture*> specularMaps =
        LoadMaterialTexture(material, aiTextureType_SPECULAR, "texture_specular", data);
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    std::vector<ObjectTexture*> normalMaps =
        LoadMaterialTexture(material, aiTextureType_HEIGHT, "texture_normal", data);
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    std::vector<ObjectTexture*> heightMaps =
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

ImporterThreaded::ImporterThreaded(YgString source, ApplicationCore* app) : Importer(source, app)
{
  m_FutureObject = m_PromiseObject.get_future();
  Yeager::Log(INFO, "Intialize Thread Importer from {}", source.c_str());
}

ImporterThreaded::~ImporterThreaded() {}

void ImporterThreaded::ThreadImport(YgCchar path, bool flip_image, unsigned int assimp_flags)
{
  m_ImageFlip = flip_image;

  m_FullPath = path;
  m_Thread = std::thread([this, path, assimp_flags] {
    Assimp::Importer imp;
    m_Scene = const_cast<aiScene*>(imp.ReadFile(m_FullPath.c_str(), assimp_flags));

    if (!m_Scene || m_Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_Scene->mRootNode) {
      Yeager::Log(ERROR, "Cannot load imported model! m_FullPath {}, Error {}", m_FullPath, imp.GetErrorString());
      return;
    }
    ProcessNode(m_Scene->mRootNode, m_Scene, &m_Data);
    Yeager::Log(INFO, "Thread import has finished");
    m_Data.SuccessfulLoaded = true;
    m_ThreadFinished = true;
    m_PromiseObject.set_value(m_Data);
  });
}

ImporterThreadedAnimated::ImporterThreadedAnimated(YgString source, ApplicationCore* app)
    : ImporterThreaded(source, app)
{
  m_FutureObject = m_PromiseObject.get_future();
  Yeager::Log(INFO, "Intialize Thread Importer from {}", source.c_str());
}
ImporterThreadedAnimated::~ImporterThreadedAnimated() {}

void ImporterThreadedAnimated::ThreadImport(YgCchar path, bool flip_image, unsigned int assimp_flags)
{
  m_ImageFlip = flip_image;

  m_FullPath = path;
  m_Thread = std::thread([this, path, assimp_flags] {
    Assimp::Importer imp;
    m_Scene = const_cast<aiScene*>(imp.ReadFile(m_FullPath.c_str(), assimp_flags));

    if (!m_Scene || m_Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_Scene->mRootNode) {
      Yeager::Log(ERROR, "Cannot load imported model! m_FullPath {}, Error {}", m_FullPath, imp.GetErrorString());
      return;
    }
    ProcessAnimatedNode(m_Scene->mRootNode, m_Scene, &m_Data);
    Yeager::Log(INFO, "Thread import has finished");
    m_Data.SuccessfulLoaded = true;
    m_ThreadFinished = true;
    m_PromiseObject.set_value(m_Data);
  });
}

bool ImporterThreaded::IsThreadFinish()
{
  if (m_FutureObject.valid()) {
    if (m_ThreadFinished) {
      Yeager::Log(INFO, "Thread finished");
    }
    return (m_ThreadFinished);

  } else {
    Yeager::Log(WARNING, "Future object is invalid! {}", m_Source.c_str());
    return false;
  }
}