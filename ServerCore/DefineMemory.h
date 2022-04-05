#pragma once
#include "Allocator.h"
#include "Types.h"

#ifdef _DEBUG
template<typename Type>
static auto xalloc(const uint64 Size)
{
	return static_cast<Type*>(PoolAllocator::Alloc(Size));
}

static auto xrelease(void* const Target)
{
	return PoolAllocator::Release(Target);
}
#else //_DEBUG
static auto xalloc(const uint64 Size)
{
	return static_cast<Type*>(PoolAllocator::Alloc(Size));
}

static auto xrelease(void* const Target)
{
	return PoolAllocator::Release(Target);
}
#endif //_DEBUG
