#include "DefaultValues.h"
#include "Components/Renderer/Texture/TextureHandle.h"
#include "Main/Core/Application.h"
using namespace Yeager;

DefaultValues::DefaultValues(Yeager::ApplicationCore* application) : m_Application(application)
{
  GenerateDefaults();
}

void DefaultValues::GenerateDefaults()
{
  m_DefaultTexture = std::make_shared<MaterialTexture2D>(EntityBuilder(m_Application, "Default_Scene_Texture"),
                                                         MaterialTextureType::eDIFFUSE);
  m_DefaultTexture->GenerateFromFile(GetPathFromShared("/Resources/Textures/Default/image.jpg").value());
}

std::shared_ptr<MaterialTexture2D> DefaultValues::GetTexture()
{
  return m_DefaultTexture;
}