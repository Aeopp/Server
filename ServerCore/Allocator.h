#pragma once

class Allocator
{
public:
	static void* Alloc(const uint64 Size) noexcept;
	static void  Release(void* const target) noexcept;
};

class StompAllocator
{
public:
	// 4KB = 4096Byte = 32,768bit
	static constexpr uint32 PAGE_SIZE = 4096;

public:
	static void* Alloc(const uint64 Size) noexcept;
	static void  Release(void* const target) noexcept;
};
