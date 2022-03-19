#pragma once
#include "Types.h"

/*
 * shared_mutex와 비슷한 컨셉 스핀락
 * Write : 상호 베타적, Read : 임계 영역 공유 허용
 */

class Lock
{
public:
	using IDType = int32;

	Lock();

public:
	// mutually exclusive 동시에 획득 불가
	void WriteLock() noexcept;
	void WriteUnlock() noexcept;
	// 같은 reader 끼리만 획득 가능
	void ReadLock() noexcept;
	void ReadUnlock() noexcept;

#ifdef _DEBUG
	void WriteLock(const std::string& Name) noexcept;
	void WriteUnlock(const std::string& Name) noexcept;
	void ReadLock(const std::string& Name) noexcept;
	void ReadUnlock(const std::string& Name) noexcept;
#endif //_DEBUG

	IDType GetLockID()const;

private:
	enum FormatDefine : uint32
	{
		// 32 bit [Write Thread ID...16~31 bit][Read Thread ID...0~15 bit]
		ACQUIRE_TIMEOUT_TICK = 10'000u
		, MAX_SPIN_COUNT = 5'000u	// 스핀락 루프 횟수..
		, WRITE_THREAD_MASK = 0xFFFF'0000u
		, READ_COUNT_MASK = 0x0000'FFFFu
		, EMPTY_FLAG = 0x0000'0000u
	};
	
private:
	std::atomic<std::underlying_type_t<FormatDefine>> _LockFlag{ EMPTY_FLAG };
	IDType _WriteCount{ 0u }; // 재귀적인 Lock 획득을 허용하기 위해 정의
	IDType _ID;

};
