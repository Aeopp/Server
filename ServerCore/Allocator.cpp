#include "Precompiled.h"
#include "Allocator.h"

void* Allocator::Alloc(const uint64 Size) noexcept
{
	return ::malloc(Size);
}

void Allocator::Release(void* const Target) noexcept
{
	::free(Target);
}

void* StompAllocator::Alloc(const uint64 Size) noexcept
{
	const int64 PageCount = (Size + PAGE_SIZE - 1) / PAGE_SIZE;
	// page count * 4096
	const uint64 AllocateSize = PageCount * PAGE_SIZE;
	const DWORD ImmediatelyAllocation = MEM_RESERVE | MEM_COMMIT;

	const uint32 DateOffset = PageCount * PAGE_SIZE - Size;
	auto Address = static_cast<BYTE*>(::VirtualAlloc(nullptr, AllocateSize, ImmediatelyAllocation, PAGE_READWRITE));
	return Address += DateOffset;
}

void StompAllocator::Release(void* const Target) noexcept
{
	::VirtualFree(Target, 0, MEM_RELEASE);
}