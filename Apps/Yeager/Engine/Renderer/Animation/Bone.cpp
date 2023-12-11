#include "Bone.h"
using namespace Yeager;

Bone::Bone(const YgString& Name, int ID, const aiNodeAnim* Channel) : m_Name(Name), m_ID(ID), m_LocalTransform(1.0f)
{
  m_NumPositions = Channel->mNumPositionKeys;
  for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex) {
    aiVector3D aiPos = Channel->mPositionKeys[positionIndex].mValue;
    float time = Channel->mPositionKeys[positionIndex].mTime;
    KeyPosition data;
    data.Position = GetGLMVec(aiPos);
    data.TimeStamp = time;
    m_Positions.push_back(data);
  }

  m_NumRotations = Channel->mNumRotationKeys;
  for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex) {
    aiQuaternion aiOrientation = Channel->mRotationKeys[rotationIndex].mValue;
    float time = Channel->mRotationKeys[rotationIndex].mTime;
    KeyRotation data;
    data.Orientation = GetGLMQuat(aiOrientation);
    data.TimeStamp = time;
    m_Rotations.push_back(data);
  }

  m_NumScalings = Channel->mNumScalingKeys;
  for (int scalingIndex = 0; scalingIndex < m_NumScalings; ++scalingIndex) {
    aiVector3D scale = Channel->mScalingKeys[scalingIndex].mValue;
    float time = Channel->mScalingKeys[scalingIndex].mTime;
    KeyScale data;
    data.Scale = GetGLMVec(scale);
    data.TimeStamp = time;
    m_Scales.push_back(data);
  }
}

void Bone::Update(float AnimationTime)
{
  YgMatrix4 trans = InterpolatePosition(AnimationTime);
  YgMatrix4 rotation = InterpolateRotation(AnimationTime);
  YgMatrix4 scale = InterpolateScaling(AnimationTime);
  m_LocalTransform = trans * rotation * scale;
}

int Bone::GetPositionIndex(float AnimationTime)
{
  for (int index = 0; index < m_NumPositions - 1; ++index) {
    if (AnimationTime < m_Positions[index + 1].TimeStamp) {
      return index;
    }
  }
  Yeager::Log(WARNING, "Warning! Bone cannot find positions index associated!");
  assert(0);
}

int Bone::GetRotationIndex(float AnimationTime)
{
  for (int index = 0; index < m_NumRotations - 1; ++index) {
    if (AnimationTime < m_Rotations[index + 1].TimeStamp) {
      return index;
    }
  }
  Yeager::Log(WARNING, "Warning! Bone cannot find rotation index associated!");
  assert(0);
}

int Bone::GetScaleIndex(float AnimationTime)
{
  for (int index = 0; index < m_NumScalings - 1; ++index) {
    if (AnimationTime < m_Scales[index + 1].TimeStamp) {
      return index;
    }
  }
  Yeager::Log(WARNING, "Warning! Bone cannot find scaling index associated!");
  assert(0);
}

float Bone::GetScaleFactor(float LastTimeStamp, float NextTimeStamp, float AnimationTime)
{
  float scaleFator = 0.0f;
  float midWay = AnimationTime - LastTimeStamp;
  float frameDiff = NextTimeStamp - LastTimeStamp;
  scaleFator = midWay / frameDiff;
  return scaleFator;
}

YgMatrix4 Bone::InterpolatePosition(float AnimationTime)
{
  if (m_NumPositions == 1) {
    return glm::translate(YgMatrix4(1.0f), m_Positions[0].Position);
  }

  int p0Index = GetPositionIndex(AnimationTime);
  int p1Index = p0Index + 1;
  float scaleFactor = GetScaleFactor(m_Positions[p0Index].TimeStamp, m_Positions[p1Index].TimeStamp, AnimationTime);
  YgVector3 finalPos = glm::mix(m_Positions[p0Index].Position, m_Positions[p1Index].Position, scaleFactor);
  return glm::translate(YgMatrix4(1.0f), finalPos);
}

YgMatrix4 Bone::InterpolateRotation(float AnimationTime)
{
  if (m_NumRotations == 1) {
    auto rotation = glm::normalize(m_Rotations[0].Orientation);
    return glm::toMat4(rotation);
  }

  int p0Index = GetRotationIndex(AnimationTime);
  int p1Index = p0Index + 1;
  float scaleFactor = GetScaleFactor(m_Rotations[p0Index].TimeStamp, m_Rotations[p1Index].TimeStamp, AnimationTime);
  glm::quat finalRot = glm::slerp(m_Rotations[p0Index].Orientation, m_Rotations[p1Index].Orientation, scaleFactor);
  finalRot = glm::normalize(finalRot);
  return glm::toMat4(finalRot);
}

YgMatrix4 Bone::InterpolateScaling(float AnimationTime)
{
  if (m_NumScalings == 1) {
    return glm::scale(YgMatrix4(1.0f), m_Scales[0].Scale);
  }
  int p0Index = GetScaleIndex(AnimationTime);
  int p1Index = p0Index + 1;
  float scaleFactor = GetScaleFactor(m_Scales[p0Index].TimeStamp, m_Scales[p1Index].TimeStamp, AnimationTime);
  YgVector3 finalScale = glm::mix(m_Scales[p0Index].Scale, m_Scales[p1Index].Scale, scaleFactor);
  return glm::scale(YgMatrix4(1.0f), finalScale);
}
