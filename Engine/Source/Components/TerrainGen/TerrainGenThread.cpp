#include "TerrainGenThread.h"
#include "Components/Kernel/Memory/Allocator.h"
using namespace Yeager;

std::shared_ptr<FaultFormationTerrain> TerrainGenThreadManagement::mTerrain = YEAGER_NULLPTR;
std::shared_ptr<WpThread> TerrainGenThreadManagement::mThread = YEAGER_NULLPTR;
std::atomic_bool TerrainGenThreadManagement::bIsExecutingThread = false;
bool TerrainGenThreadManagement::bTerrainCanBeDraw = false;
Shader* TerrainGenThreadManagement::mShader = YEAGER_NULLPTR;
bool TerrainGenThreadManagement::bMustGenerateGL = false;

void TerrainGenThreadManagement::CreateThreadFaultFormationTerrain(std::vector<String> TexturesPaths,
                                                                   int TerrainChunkPositionX, int TerrainChunkPositionY,
                                                                   Shader* shader, int TerrainSize, int It,
                                                                   int MinHeight, int MaxHeight, float FIR, int octaves,
                                                                   int bias)
{
  if (mThread != YEAGER_NULLPTR || bIsExecutingThread)
    return;
  mShader = shader;
  mTerrain =
      BaseAllocator::MakeSharedPtr<FaultFormationTerrain>(TexturesPaths, TerrainChunkPositionX, TerrainChunkPositionY);
  bIsExecutingThread = true;
  bTerrainCanBeDraw = false;
  mThread = ThreadManagement::NewThread("TerrainGen", [=]() {
    IntervalElapsedTimeManager::StaticStartTimeInterval("TerrainGeneration_Thread");
    mTerrain->CreateFaultFormationTerrain(shader, TerrainSize, It, MinHeight, MaxHeight, FIR, octaves, bias);
    bIsExecutingThread = false;
    bMustGenerateGL = true;
    Yeager::LogDebug(INFO, "Terrain generation was finished!");
    IntervalElapsedTimeManager::StaticEndTimeInterval("TerrainGeneration_Thread");
  });
}

void TerrainGenThreadManagement::UpdateThread()
{
  if (!bIsExecutingThread && bMustGenerateGL) {

    mTerrain->SetupGL(mShader);
    bTerrainCanBeDraw = true;
    mThread.reset();
    mThread = YEAGER_NULLPTR;
    bMustGenerateGL = false;
    Yeager::LogDebug(INFO, "Update thread, kill thread!");
  }
}

void TerrainGenThreadManagement::DrawTerrain()
{
  if (bTerrainCanBeDraw) {
    glDisable(GL_CULL_FACE);
    IntervalElapsedTimeManager::StartTimeInterval("Terrain drawing");
    //Yeager::LogDebug(INFO, "Drawing terrain!");
    mTerrain->Draw(mShader);
    IntervalElapsedTimeManager::EndTimeInterval("Terrain drawing");
    glEnable(GL_CULL_FACE);
  }
}
