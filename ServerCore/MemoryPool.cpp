#include "Precompiled.h"
#include "MemoryPool.h"

MemoryPool::MemoryPool(const uint32 AllocSize)
	: _AllocSize{ AllocSize }
{

}

MemoryPool::~MemoryPool() noexcept
{
	while (false == _Queue.empty())
	{
		MemoryHeader* Header = _Queue.front();
		_Queue.pop();
		Allocator::Release(Header);
	}
}

void MemoryPool::Push(MemoryHeader* Ptr)
{
	auto WriteLock = MAKE_LOCKGUARD(GET_TYPENAME(this), _Lock, LockGuard::Use::Write);

	_Queue.push(Ptr);

	_AllocCount.fetch_sub(1);
}

MemoryHeader* MemoryPool::Pop()
{
	MemoryHeader* Header = nullptr;

	{
		auto WriteLock = MAKE_LOCKGUARD(GET_TYPENAME(this), _Lock, LockGuard::Use::Write);

		if (false == _Queue.empty())
		{
			Header = _Queue.front();
			_Queue.pop();
		}
	}

	if (nullptr == Header)
	{
		Header = reinterpret_cast<MemoryHeader*>(Allocator::Alloc(_AllocSize));
	}
	else
	{
		ASSERT_CRASH(0 == Header->AllocSize,"Error!! MemoryHeader AllocSize is Zero");
	}

	_AllocCount.fetch_add(1);

	return Header;
};
