#include "DefaultValues.h"
#include "Application.h"
#include "Engine/Renderer/TextureHandle.h"
using namespace Yeager;

DefaultValues::DefaultValues(Yeager::ApplicationCore* application) : m_Application(application)
{
  GenerateDefaults();
}

void DefaultValues::GenerateDefaults()
{
  m_DefaultTexture = std::make_shared<MaterialTexture2D>(m_Application, "Default", MaterialTextureType::eDIFFUSE);
  m_DefaultTexture->GenerateFromFile(GetPath("/Configuration/Editor/Private/Default/DefaultTexture.jpg"));
}

std::shared_ptr<MaterialTexture2D> DefaultValues::GetTexture()
{
  return m_DefaultTexture;
}