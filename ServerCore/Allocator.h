#pragma once

class MemoryPool;
class Memory;

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

template<typename Type>
static auto xalloc(const uint64 Size)
{
	auto AllocatePtr = static_cast<Type*>(StompAllocator::Alloc(Size));
#ifdef _DEBUG
	const auto Address = reinterpret_cast<uint64>(AllocatePtr);
	auto Log = std::format("allocate [{}], address [{}], size[{}]\n", typeid(Type).name(), Address, Size);
	std::cout << Log;
#endif //_DEBUG
	return AllocatePtr;
}

template<typename Type>
static auto xrelease(Type* const Target)
{
#ifdef _DEBUG
	const auto Address = reinterpret_cast<uint64>(Target);
	auto Log = std::format("release [{}], address[{}]\n", typeid(Type).name(), Address);
	std::cout << Log;
#endif //_DEBUG
	return StompAllocator::Release(Target);
}

template<typename Type>
class StlAllocator
{
public:
	using value_type = Type;

public:
	StlAllocator() {};

	template<typename Other>
	StlAllocator(const StlAllocator<Other>& rhs) {};

public:
	Type* allocate(size_t Count)
	{
		const auto Size = static_cast<int32>(Count * sizeof(Type));
		return xalloc<Type>(Size);
	}

	void deallocate(void* const Target, size_t Count)
	{
		xrelease(static_cast<Type*>(Target));
	}
};

class AllocatorTester
{
public:
	int SizeFill[20] = {0,};

	AllocatorTester()
	{
		std::cout << "Constructor\n";
	}

	~AllocatorTester()
	{
		std::cout << "Destructor\n";
	}
}

class PoolAllocator
{
public:
	template<typename Type>
	static Type* Alloc(const uint64 Size) noexcept
	{
		return Memory::Instance().Allocate<Type>(Size);
	}

	static void Release(void* const target) noexcept;
};
