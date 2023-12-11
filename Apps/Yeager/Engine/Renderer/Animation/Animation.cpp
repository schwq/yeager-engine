#include "Animation.h"
using namespace Yeager;

Animation::Animation(const YgString& path, AnimatedObject* model)
{
  Assimp::Importer imp;
  const aiScene* scene = imp.ReadFile(path, aiProcess_Triangulate);

  if (!scene || !scene->mRootNode) {
    Yeager::Log(ERROR, "Assimp cannot load animation file! Path {}", path);
  }

  auto animation = scene->mAnimations[0];
  m_Duration = animation->mDuration;
  m_TicksPerSecond = animation->mTicksPerSecond;
  aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
  globalTransformation = globalTransformation.Inverse();
  ReadHeirarchyData(m_RootNode, scene->mRootNode);
  ReadMissingBones(animation, *model);
}

Bone* Animation::FindBone(const YgString& name)
{
  auto iter = std::find_if(m_Bones.begin(), m_Bones.end(), [&](Bone& Bone) { return Bone.GetBoneName() == name; });
  if (iter == m_Bones.end()) {
    return nullptr;
  } else {
    return &(*iter);
  }
}

void Animation::ReadMissingBones(const aiAnimation* animation, AnimatedObject& model)
{
  int size = animation->mNumChannels;
  auto& BoneInfoMap = model.GetModelData()->GetBoneInfoMap();
  int& BoneCount = model.GetModelData()->GetBoneCount();

  for (int x = 0; x < size; x++) {
    auto channel = animation->mChannels[x];
    YgString boneName = channel->mNodeName.data;

    if (BoneInfoMap.find(boneName) == BoneInfoMap.end()) {
      BoneInfoMap[boneName].ID = BoneCount;
      BoneCount++;
    }

    m_Bones.push_back(Bone(channel->mNodeName.data, BoneInfoMap[channel->mNodeName.data].ID, channel));
  }

  m_BoneInfoMap = BoneInfoMap;
}

void Animation::ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src)
{
  assert(src);

  dest.Name = src->mName.data;
  dest.Transformation = ConvertAssimpMatrixToGLMFormat(src->mTransformation);
  dest.ChildrenCount = src->mNumChildren;

  for (int x = 0; x < src->mNumChildren; x++) {
    AssimpNodeData newData;
    ReadHeirarchyData(newData, src->mChildren[x]);
    dest.Children.push_back(newData);
  }
}