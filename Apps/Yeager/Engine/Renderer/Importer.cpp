#include "Importer.h"
#include "../../Application.h"
#include "../Editor/Explorer.h"
#include "TextureHandle.h"

#include "stb_image.h"

using namespace Yeager;
using namespace physx;

Uint Importer::m_ImportedModelsCount = 0;

Importer::Importer(String source, ApplicationCore* app) : m_Source(source), m_Application(app)
{
  Yeager::Log(INFO, "Intialize Importer from {}", m_Source.c_str());
  if (app == YEAGER_NULLPTR) {
    Yeager::Log(WARNING, "The importer application pointer have not been set!");
  }
}
/* TODO review the order of vectors here, it can change the direction of the output normal */
Vector3 Yeager::CalculateTriangleNormalFromPoints(Vector3 p1, Vector3 p2, Vector3 p3)
{
  Vector3 U = p2 - p1;
  Vector3 V = p3 - p1;
  return glm::cross(U, V);
}

std::vector<physx::PxVec3> Yeager::ConvertYgVectors3ToPhysXVec3(const std::vector<Vector3>& vectors)
{
  std::vector<physx::PxVec3> v;
  for (const auto& vec : vectors) {
    v.push_back(physx::PxVec3(vec.x, vec.y, vec.z));
  }
  return v;
}

/* TODO review this implementation */
Vector3 CalculatePolygonNormalFromPoints(std::vector<Vector3>& points)
{
  Vector3 Normal = YEAGER_ZERO_VECTOR3;
  for (Uint index = 0; index < points.size(); index++) {
    Vector3 Current = points.at(index);
    Vector3 Next = points.at((index + 1) % points.size());
    Normal += glm::cross(Current, Next);
  }
  return Normal;
}

Importer::~Importer()
{
  Yeager::Log(INFO, "Destrorying Importer from {}", m_Source.c_str());
}
ObjectModelData Importer::Import(Cchar path, const ObjectCreationConfiguration configuration, bool flip_image,
                                 Uint assimp_flags)
{
  m_CreationConfiguration = configuration;
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

ObjectModelData Importer::ImportToPhysX(Cchar path, physx::PxRigidActor* actor, bool flip_image, Uint assimp_flags)
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
  ProcessPhysXNode(actor, scene->mRootNode, scene, &data);
  data.SuccessfulLoaded = true;
  return data;
}

void Importer::ProcessPhysXNode(physx::PxRigidActor* actor, aiNode* node, const aiScene* scene, ObjectModelData* data)
{
  for (Uint x = 0; x < node->mNumMeshes; x++) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[x]];
    data->Meshes.push_back(ProcessPhysXMesh(actor, mesh, scene, data));
  }

  for (Uint x = 0; x < node->mNumChildren; x++) {
    ProcessPhysXNode(actor, node->mChildren[x], scene, data);
  }
}
ObjectMeshData Importer::ProcessPhysXMesh(physx::PxRigidActor* actor, aiMesh* mesh, const aiScene* scene,
                                          ObjectModelData* data)
{
  std::vector<ObjectVertexData> vertices;
  std::vector<physx::PxVec3> PhysxVertices;
  std::vector<GLuint> indices;
  std::vector<MaterialTexture2D*> textures;

  for (Uint x = 0; x < mesh->mNumVertices; x++) {
    ObjectVertexData vertex;
    Vector3 vector;

    vector.x = mesh->mVertices[x].x;
    vector.y = mesh->mVertices[x].y;
    vector.z = mesh->mVertices[x].z;
    vertex.Position = vector;
    if (mesh->HasNormals()) {
      vector.x = mesh->mNormals[x].x;
      vector.y = mesh->mNormals[x].y;
      vector.z = mesh->mNormals[x].z;
      vertex.Normals = vector;
    } else {
      vertex.Normals = Vector3(0.0f, 1.0f, 0.0f);
    }

    if (mesh->mTextureCoords[0]) {  // has texture coords
      Vector2 vec;
      vec.x = mesh->mTextureCoords[0][x].x;
      vec.y = mesh->mTextureCoords[0][x].y;
      vertex.TextureCoords = vec;
    } else {  // No texture coords
      vertex.TextureCoords = Vector2(0.0f, 0.0f);
    }
    vertices.push_back(vertex);
    PhysxVertices.push_back(Vector3ToPxVec3(vertex.Position));
  }

  for (Uint x = 0; x < mesh->mNumFaces; x++) {

    aiFace face = mesh->mFaces[x];
    for (Uint y = 0; y < face.mNumIndices; y++) {
      indices.push_back(face.mIndices[y]);
    }
  }

  if (mesh->mMaterialIndex >= 0) {
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<MaterialTexture2D*> diffuseMaps =
        LoadMaterialTexture(material, aiTextureType_DIFFUSE, "texture_diffuse", data);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    std::vector<MaterialTexture2D*> specularMaps =
        LoadMaterialTexture(material, aiTextureType_SPECULAR, "texture_specular", data);
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    std::vector<MaterialTexture2D*> metallicMaps =
        LoadMaterialTexture(material, aiTextureType_METALNESS, "texture_metallic", data);
    textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());
    std::vector<MaterialTexture2D*> roughnessMaps =
        LoadMaterialTexture(material, aiTextureType_DIFFUSE_ROUGHNESS, "texture_roughness", data);
    textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());
  }
  PxMaterial* material = m_Application->GetPhysXHandle()->GetPxPhysics()->createMaterial(1.0f, 1.0f, 1.0f);
  PxShape* shape = m_Application->GetPhysXHandle()->GetPxPhysics()->createShape(
      PxTriangleMeshGeometry(m_Application->GetPhysXHandle()->GetGeometryHandle()->CreateTriangleMesh(
          mesh->mNumVertices, mesh->mNumFaces, sizeof(PxVec3), sizeof(GLuint) * 3, &PhysxVertices[0],
          (PxU32*)&indices[0])),
      *material);
  actor->attachShape(*shape);
  shape->release();

  return ObjectMeshData(indices, vertices, textures);
}

void Importer::ProcessNode(aiNode* node, const aiScene* scene, ObjectModelData* data)
{
  for (Uint x = 0; x < node->mNumMeshes; x++) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[x]];
    data->Meshes.push_back(ProcessMesh(mesh, scene, data));
  }

  for (Uint x = 0; x < node->mNumChildren; x++) {
    ProcessNode(node->mChildren[x], scene, data);
  }
}

ObjectMeshData Importer::ProcessMesh(aiMesh* mesh, const aiScene* scene, ObjectModelData* data)
{
  std::vector<ObjectVertexData> vertices;
  std::vector<GLuint> indices;
  std::vector<MaterialTexture2D*> textures;

  for (Uint x = 0; x < mesh->mNumVertices; x++) {
    ObjectVertexData vertex;
    Vector3 vector;

    vector.x = mesh->mVertices[x].x;
    vector.y = mesh->mVertices[x].y;
    vector.z = mesh->mVertices[x].z;
    vertex.Position = vector;
    if (mesh->HasNormals()) {
      vector.x = mesh->mNormals[x].x;
      vector.y = mesh->mNormals[x].y;
      vector.z = mesh->mNormals[x].z;
      vertex.Normals = vector;
    } else {
      vertex.Normals = Vector3(0.0f, 1.0f, 0.0f);
    }

    if (mesh->mTextureCoords[0]) {  // has texture coords
      Vector2 vec;
      vec.x = mesh->mTextureCoords[0][x].x;
      vec.y = mesh->mTextureCoords[0][x].y;
      vertex.TextureCoords = vec;
    } else {  // No texture coords
      vertex.TextureCoords = Vector2(0.0f, 0.0f);
    }
    vertices.push_back(vertex);
  }

  for (Uint x = 0; x < mesh->mNumFaces; x++) {

    aiFace face = mesh->mFaces[x];
    for (Uint y = 0; y < face.mNumIndices; y++) {
      indices.push_back(face.mIndices[y]);
    }
  }

  if (mesh->mMaterialIndex >= 0) {
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<MaterialTexture2D*> diffuseMaps =
        LoadMaterialTexture(material, aiTextureType_DIFFUSE, "texture_diffuse", data);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    std::vector<MaterialTexture2D*> specularMaps =
        LoadMaterialTexture(material, aiTextureType_SPECULAR, "texture_specular", data);
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    std::vector<MaterialTexture2D*> metallicMaps =
        LoadMaterialTexture(material, aiTextureType_METALNESS, "texture_metallic", data);
    textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());
    std::vector<MaterialTexture2D*> roughnessMaps =
        LoadMaterialTexture(material, aiTextureType_DIFFUSE_ROUGHNESS, "texture_roughness", data);
    textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());
  }

  return ObjectMeshData(indices, vertices, textures);
}

std::vector<MaterialTexture2D*> Importer::LoadMaterialTexture(aiMaterial* material, aiTextureType type, String typeName,
                                                              CommonModelData* data)
{
  std::vector<MaterialTexture2D*> textures;

  for (Uint x = 0; x < material->GetTextureCount(type); x++) {
    bool skip = false;
    aiString str;
    material->GetTexture(type, x, &str);
    String textureString = String(str.C_Str());
    String comparePath;

    if (g_OperatingSystemString == "WIN32") {  // Sometimes the textureString in the MTL file is from a windows computer
      std::replace(textureString.begin(), textureString.end(), '/', '\\');
    } else {
      std::replace(textureString.begin(), textureString.end(), '\\', '/');
    }

    if (m_CreationConfiguration.TextureFolder.Valid &&
        std::filesystem::is_directory(m_CreationConfiguration.TextureFolder.path)) {
      comparePath = ReadSuffixUntilCharacter(textureString, YG_PS);
      comparePath = m_CreationConfiguration.TextureFolder.path + comparePath;
      Yeager::ValidatesPath(comparePath);

    } else {
      comparePath = RemoveSuffixUntilCharacter(m_FullPath, YG_PS);

      // Texture path in the mtl file is a complete path, we just assign the complete path to the compare_path without adding to it
      Yeager::ValidatesPath(comparePath + textureString) ? comparePath += textureString : comparePath = textureString;

      // Checks if the texture is already loaded to the model, if so, it skips the loading, saving a lot of time
    }
    for (Uint y = 0; y < data->TexturesLoaded.size(); y++) {
      if (std::strcmp(data->TexturesLoaded[y]->first.GetTextureDataHandle()->Path.data(), comparePath.data()) == 0) {
        MaterialTexture2D* rt = &data->TexturesLoaded[y]->first;
        textures.push_back(rt);
        skip = true;
        break;
      }
    }

    if (!skip) {

      auto tex = std::make_shared<std::pair<MaterialTexture2D, STBIDataOutput*>>();

      /* If the texture loading have been called in a thread without the openGL context loaded intro to it, the texture id will ALWAYS be 0, meaning it wont load, 
        we check if the current thread is with the openGL context, if not, the boolean incompleteID is set to true, and the texture loading is done after the thread is finished! */
      if (!m_Application->GetWindow()->CheckIfOpenGLContext()) {
        tex->first.GetTextureDataHandle()->ImcompletedID = true;
        tex->second = LoadStbiDataOutput(comparePath, m_ImageFlip);
      } else {
        tex->first.GenerateFromFile(comparePath, m_ImageFlip);
      }

      tex->first.SetName(typeName.c_str());
      MaterialTexture2D* rt = &tex->first;
      textures.push_back(rt);
      data->TexturesLoaded.push_back(tex);
    }
  }
  return textures;
}

STBIDataOutput* Importer::LoadStbiDataOutput(String path, bool flip)
{
  STBIDataOutput* output = new STBIDataOutput;

  stbi_set_flip_vertically_on_load(flip);
  ValidatesPath(path);
  output->Data = stbi_load(path.c_str(), &output->Width, &output->Height, &output->NrComponents, 0);
  output->OriginalPath = path;
  output->Flip = flip;

  if (output->Data == YEAGER_NULLPTR) {
    Yeager::Log(ERROR, "Cannot load data to STBIDataOutput! Path: {}, Reason {}", path, stbi_failure_reason());
  }

  return output;
}

AnimatedObjectModelData Importer::ImportAnimated(Cchar path, const ObjectCreationConfiguration configuration,
                                                 bool flip_image, Uint assimp_flags)
{
  m_CreationConfiguration = configuration;
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
  for (Uint x = 0; x < node->mNumMeshes; x++) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[x]];
    data->Meshes.push_back(ProcessAnimatedMesh(mesh, scene, data));
  }

  for (Uint x = 0; x < node->mNumChildren; x++) {
    ProcessAnimatedNode(node->mChildren[x], scene, data);
  }
}

AnimatedObjectMeshData Importer::ProcessAnimatedMesh(aiMesh* mesh, const aiScene* scene, AnimatedObjectModelData* data)
{
  std::vector<AnimatedVertexData> vertices;
  std::vector<GLuint> indices;
  std::vector<MaterialTexture2D*> textures;

  for (Uint x = 0; x < mesh->mNumVertices; x++) {
    AnimatedVertexData vertex;
    SetVertexBoneDataToDefault(vertex);
    Vector3 vector;

    vector.x = mesh->mVertices[x].x;
    vector.y = mesh->mVertices[x].y;
    vector.z = mesh->mVertices[x].z;
    vertex.Position = vector;

    vector.x = mesh->mNormals[x].x;
    vector.y = mesh->mNormals[x].y;
    vector.z = mesh->mNormals[x].z;
    vertex.Normals = vector;

    if (mesh->mTextureCoords[0]) {  // has texture coords
      Vector2 vec;
      vec.x = mesh->mTextureCoords[0][x].x;
      vec.y = mesh->mTextureCoords[0][x].y;
      vertex.TextureCoords = vec;
    } else {  // No texture coords
      vertex.TextureCoords = Vector2(0.0f, 0.0f);
    }
    vertices.push_back(vertex);
  }

  for (Uint x = 0; x < mesh->mNumFaces; x++) {
    aiFace face = mesh->mFaces[x];
    for (Uint y = 0; y < face.mNumIndices; y++) {
      indices.push_back(face.mIndices[y]);
    }
  }

  if (mesh->mMaterialIndex >= 0) {
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<MaterialTexture2D*> diffuseMaps =
        LoadMaterialTexture(material, aiTextureType_DIFFUSE, "texture_diffuse", data);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    std::vector<MaterialTexture2D*> specularMaps =
        LoadMaterialTexture(material, aiTextureType_SPECULAR, "texture_specular", data);
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    std::vector<MaterialTexture2D*> normalMaps =
        LoadMaterialTexture(material, aiTextureType_HEIGHT, "texture_normal", data);
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    std::vector<MaterialTexture2D*> heightMaps =
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
    String boneName = String(cc);
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

ImporterThreaded::ImporterThreaded(String source, ApplicationCore* app) : Importer(source, app)
{
  m_FutureObject = m_PromiseObject.get_future();
  Yeager::Log(INFO, "Intialize Thread Importer from {}", source.c_str());
}

ImporterThreaded::~ImporterThreaded() {}

void ImporterThreaded::ThreadImport(Cchar path, const ObjectCreationConfiguration configuration, bool flip_image,
                                    Uint assimp_flags)
{
  m_CreationConfiguration = configuration;
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

ImporterThreadedAnimated::ImporterThreadedAnimated(String source, ApplicationCore* app) : ImporterThreaded(source, app)
{
  m_FutureObject = m_PromiseObject.get_future();
  Yeager::Log(INFO, "Intialize Thread Importer from {}", source.c_str());
}
ImporterThreadedAnimated::~ImporterThreadedAnimated() {}

void ImporterThreadedAnimated::ThreadImport(Cchar path, const ObjectCreationConfiguration configuration,
                                            bool flip_image, Uint assimp_flags)
{
  m_CreationConfiguration = configuration;
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