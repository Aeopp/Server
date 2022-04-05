#pragma once
#include "Allocator.h"
#include "DefineMemory.h"
#include "SingletonInterface.hpp"

class MemoryPool;

class Memory final : public SingleTonInterface<Memory>
{
	enum
	{
		// ~1024까지 32단위, ~2048까지 128단위, ~4096까지 256단위
		POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256),
		MAX_ALLOC_SIZE = 4096
	};

public:
	Memory();
	~Memory();

	void Initialize() & noexcept;
	void* Allocate(uint32 Size) & noexcept;
	void Release(void* Ptr) & noexcept;

private:
	std::vector<MemoryPool*> _Pools;

	// 메모리크기 <-> 메모리 풀
	// 0(1) 빠르게 찾기 위한 테이블
	// 0~32
	MemoryPool* _PoolTable[MAX_ALLOC_SIZE + 1];
};

template<typename Type, typename ...Params>
Type* xnew(Params&&... Args)
{
	constexpr uint64 ALLOCATE_SIZE = sizeof(Type);
	Type* const Memory = xalloc<Type>(ALLOCATE_SIZE); // 할당
	
#ifdef _DEBUG
	auto Log = std::format("allocate [{}], size[{}]\n", GET_TYPENAME<Type>(), ALLOCATE_SIZE);
	std::cout << Log;
#endif //_DEBUG

	return new(Memory)Type(std::forward<Params>(Args)...); // 생성자 호출
}

template<typename Type>
void xdelete(Type* Target)
{
#ifdef _DEBUG
	auto Log = std::format("release [{}]\n", GET_TYPENAME<Type>());
	std::cout << Log;
#endif //_DEBUG
	Target->~Type();
	xrelease(Target);
}

