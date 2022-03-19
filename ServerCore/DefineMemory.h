#pragma once
#include "Allocator.h"
#include "Types.h"

#ifdef _DEBUG
template<typename Type>
static auto xalloc(const uint64 Size)
{
	return static_cast<Type*>(StompAllocator::Alloc(Size));
}

static auto xrelease(void* const Target)
{
	return StompAllocator::Release(Target);
}
#else //_DEBUG
static auto xalloc(const uint64 Size)
{
	return StompAllocator::Alloc(Size);
}

static auto xrelease(void* const Target)
{
	return StompAllocator::Release(Target);
}
#endif //_DEBUG