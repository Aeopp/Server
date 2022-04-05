#include "Precompiled.h"
#include "MemoryPool.h"

Memory::Memory()
{
	int32 TableIndex = 0;
	int32 Size = 0;

	for (Size = 32; Size <= 1024; Size += 32)
	{
		MemoryPool* Pool = new MemoryPool(Size);
		_Pools.push_back(Pool);

		while (TableIndex <= Size)
		{
			_PoolTable[TableIndex] = Pool;
			++TableIndex;
		}
	}

	for (; Size <= 2048; Size += 128)
	{
		MemoryPool* Pool = new MemoryPool(Size);
		_Pools.push_back(Pool);

		while (TableIndex <= Size)
		{
			_PoolTable[TableIndex] = Pool;
			++TableIndex;
		}
	}

	for (; Size <= 4096; Size += 256)
	{
		MemoryPool* Pool = new MemoryPool(Size);
		_Pools.push_back(Pool);

		while (TableIndex <= Size)
		{
			_PoolTable[TableIndex] = Pool;
			++TableIndex;
		}
	}
}

Memory::~Memory()
{
	for (MemoryPool* Pool : _Pools)
	{
		delete Pool;
	}

	_Pools.clear();
	_Pools.shrink_to_fit();
}

void Memory::Release(void* Ptr) & noexcept
{
	MemoryHeader* Header = MemoryHeader::DetachHeader(Ptr);

	const auto AllocSize = Header->AllocSize;
	ASSERT_CRASH(AllocSize > 0, "Invalid Memory AllocSize");

	if (AllocSize > MAX_ALLOC_SIZE)
	{
		::free(Header);
	}
	else
	{
		_PoolTable[AllocSize]->Push(Header);
	}
}

void Memory::Initialize() & noexcept
{

}

void* Memory::Allocate(uint32 Size) & noexcept
{
	MemoryHeader* Header = nullptr;
	const int32 AllocSize = Size + sizeof(MemoryHeader);

	// 메모리 풀링 최대 크기를 벗어나면 일반 할당
	if (AllocSize > MAX_ALLOC_SIZE)
	{
		Header = reinterpret_cast<MemoryHeader*>(::malloc(AllocSize));
	}
	else
	{
		Header = _PoolTable[AllocSize]->Pop();
	}

	return MemoryHeader::AttachHeader(Header, AllocSize);
}
