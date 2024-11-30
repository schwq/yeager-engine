#include "Allocator.h"
using namespace Yeager;

PointerTypeStats BaseAllocator::sSharedPointersCreated;
PointerTypeStats BaseAllocator::sUniquePointersCreated;
PointerTypeStats BaseAllocator::sWeakPointersCreated;

GeneralMemoryAllocationStats BaseAllocator::sGenMemoryStats;

std::unordered_map<void*, std::size_t> BaseAllocator::mPointersInUse;