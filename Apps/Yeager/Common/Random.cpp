#include "Random.h"

std::mt19937 Yeager::g_RandomGenerator;

void Yeager::InitializeRandomGenerator()
{
  std::random_device rd;
  auto seed_data = std::array<int, std::mt19937::state_size>{};
  std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
  std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
  g_RandomGenerator = std::mt19937(seq);
}

uuids::uuid Yeager::GetRandomUUID()
{
  uuids::uuid_random_generator gen{g_RandomGenerator};
  return gen();
}