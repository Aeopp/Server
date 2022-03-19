#include "Precompiled.h"
#include "Lock.h"
#include "DeadLockDetector.h"

Lock::Lock() 
{
	static std::atomic<IDType> IDCounter{ 0 };
	_ID = IDCounter.fetch_add(+1);
}

/*
 * Lock 획득한 다른 스레드가 없거나 재귀적으로 소유중인
 * 스레드가 다시 진입할 경우에만 Lock 획득에 성공합니다.
 */
void Lock::WriteLock() noexcept
{
	// Lock 획득한 스레드가 다시 진입하면 무조건 성공
	const uint32 LockThreadId = (_LockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LockThreadId == CurrentThread::UniqueId)
	{
		++_WriteCount;
		return;
	}

	// Lock 획득한 다른 스레드가 없을 때 LockFlag에 ID를 입력하고 Lock 획득
	const uint32 WriteOwnerId = ((CurrentThread::UniqueId << 16) & WRITE_THREAD_MASK);
	using ClockType = std::chrono::system_clock;
	const auto BeginTime = ClockType::now();

	while (true)
	{
		for (uint32 SpinCount = 0; SpinCount < MAX_SPIN_COUNT; ++SpinCount)
		{
			constexpr auto EmptyFlag = EMPTY_FLAG;
			uint32 NoOwner = EmptyFlag;
			// 1. _LockFlag == EmptyFlag (Writer Id 0, Reader Count 0)
			// 2. 상위 16 비트를 thread id로 Lock 획득
			// 2.1 _LockFlag != EmptyFlag 다른 스레드가 Lock 획득 중이여서
			//	상위 16비트가 다른 스레드의 ID이거나 Shared Lock 중이여서 하위 16비트에 값이 써있는 경우
			if (_LockFlag.compare_exchange_strong(NoOwner, WriteOwnerId))
			{
				++_WriteCount;
				return;
			}
		}

		const auto SpinLockDuration = std::chrono::duration_cast<std::chrono::milliseconds>(ClockType::now() - BeginTime);
		if (SpinLockDuration.count() >= ACQUIRE_TIMEOUT_TICK)
		{
			CRASH(std::format("[{0}] [Line : {1}] [Call : {2}]",
				"Lock TimeOut", __LINE__, __FUNCTION__));
		}

		// 5천번 돌았으면 노력했다.. 할당된 시간 포기!
		std::this_thread::yield();
	}
}

void Lock::WriteUnlock() noexcept
{
	// 동일한 스레드는 Read,Write lock 둘 다 획득 가능한 정책
	// 마스킹 이후에 0이 아니다? => WriteUnlock 전에 ReadUnlock 빼먹었다 !!
	if ((_LockFlag.load() & READ_COUNT_MASK) != 0)
	{
		CRASH(std::format("[{}] [Line : {}] [Call : {}]",
			"Invalid Unlock Order", __LINE__, __FUNCTION__));
	}

	const uint32 LockCount = --_WriteCount;
	if (0 == LockCount)
	{
		_LockFlag.store(EMPTY_FLAG);
	}
}

/*
 * WriteLock 획득한 스레드가 없는 경우 성공
 */
void Lock::ReadLock() noexcept
{
	// Write Lock 이미 획득한 스레드면 Read Lock 획득도 허용 (어짜피 혼자 사용중...)
	const uint32 LockThreadId = (_LockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LockThreadId == CurrentThread::UniqueId)
	{
		_LockFlag.fetch_add(1);
		return;
	}

	// LockFlag 하위 16비트에 공유 카운트를 올리고 Lock 획득
	using ClockType = std::chrono::system_clock;
	const auto BeginTime = ClockType::now();

	while (true)
	{
		for (uint32 SpinCount = 0u; SpinCount < MAX_SPIN_COUNT; ++SpinCount)
		{
			// 하위 16비트 (Read Flag)만 비교하면 되므로 굳이 마스킹
			uint32 ReadCount = (_LockFlag.load() & READ_COUNT_MASK);

			// 1. Write 중인 스레드가 있는 경우 상위 16비트가 0이 아니므로 획득 불가
			// 2. Lock 획득 시에는 하위 16비트 공유 카운트 증가시키고 Lock 획득
			if (_LockFlag.compare_exchange_strong(ReadCount, ReadCount + 1))
			{
				return;
			}
		}

		const auto SpinLockDuration = std::chrono::duration_cast<std::chrono::milliseconds>(ClockType::now() - BeginTime);
		if (SpinLockDuration.count() >= ACQUIRE_TIMEOUT_TICK)
		{
			CRASH(std::format("[{}] [Line : {}] [Call : {}]",
				"Lock TimeOut", __LINE__, __FUNCTION__));
		}

		// 5천번 돌았으면 노력했다.. 할당된 시간 포기!
		std::this_thread::yield();
	}
}

void Lock::ReadUnlock() noexcept
{
	// 공유 카운트가 이미 0이였다? => 뭔가 잘못됐다. unlock를 여러번 했다거나
	if ((_LockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0)
	{
		CRASH(std::format("[{}] [Line : {}] [Call : {}]",
			"Multiple Unlock", __LINE__, __FUNCTION__));
	}
}

#ifdef _DEBUG
void Lock::WriteLock(const std::string& Name) noexcept
{
	DeadLockDetector::Instance().EmplaceLock(GetLockID(), Name);
	WriteLock();
}

void Lock::WriteUnlock(const std::string& Name) noexcept
{
	DeadLockDetector::Instance().PopLock(GetLockID());
	WriteUnlock();
}

void Lock::ReadLock(const std::string& Name) noexcept
{
	DeadLockDetector::Instance().EmplaceLock(GetLockID(), Name);
	ReadLock();
}

void Lock::ReadUnlock(const std::string& Name) noexcept
{
	DeadLockDetector::Instance().PopLock(GetLockID());
	ReadUnlock();
}
#endif //_DEBUG

int32 Lock::GetLockID() const
{
	return _ID;
}
