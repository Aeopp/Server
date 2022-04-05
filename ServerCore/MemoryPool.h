#pragma once

struct MemoryHeader
{
	// [MemoryHeader][Data]
	MemoryHeader(const uint32 Size) : AllocSize{ Size } 
	{

	};

	static void* AttachHeader(MemoryHeader* Header, const uint32 Size)
	{
		new(Header)MemoryHeader(Size);
		return reinterpret_cast<void*>(++Header);
	};

	static MemoryHeader* DetachHeader(void* const Ptr)
	{
		MemoryHeader* Header = reinterpret_cast<MemoryHeader*>(Ptr) - 1;
		return Header;
	};

	uint32 AllocSize;
};

class MemoryPool
{
public:
	MemoryPool(const uint32 AllocSize);
	~MemoryPool() noexcept;

	void Push(MemoryHeader* Ptr);
	MemoryHeader* Pop();

private:
	uint32 _AllocSize{ 0 };
	std::atomic<uint32> _AllocCount{ 0 };

	Lock _Lock;
	std::queue<MemoryHeader*> _Queue;
};

