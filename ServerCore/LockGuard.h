#pragma once

class LockGuard
{
public:
	enum class Use : uint32
	{
		Read,
		Write,
	};

public:
	LockGuard(Lock& Target, const Use UseFlag)
		: _Lock{ Target }
		, _UseFlag{ UseFlag }
	{
		if (_UseFlag == Use::Read)
		{
			_Lock.ReadLock();
		}
		else
		{
			_Lock.WriteLock();
		}
	};

#ifdef _DEBUG
	LockGuard(const std::string& DebugName, Lock& Target, const Use UseFlag)
		: _Lock{ Target }
		, _UseFlag{ UseFlag }
	{
		_Name = std::format("[ ID:{} Class:{} ]", Target.GetLockID(), DebugName);

		if (_UseFlag == Use::Read)
		{
			_Lock.ReadLock(_Name);
		}
		else
		{
			_Lock.WriteLock(_Name);
		}
	};
#endif //_DEBUG

	~LockGuard();
private:
	Lock& _Lock;
	Use _UseFlag;
#ifdef _DEBUG
	std::string _Name;
#endif //_DEBUG
};

#ifdef _DEBUG

static LockGuard MakeLockGuardImpl(const char* DebugName, Lock& Target, const LockGuard::Use UseFlag)
{
	return LockGuard{ DebugName, Target, UseFlag };
}
#	define MAKE_LOCKGUARD(Name, Param1, Param2) MakeLockGuardImpl(Name, Param1, Param2)

#else //_DEBUG

static LockGuard MakeLockGuardImpl(Lock& Target, const LockGuard::Use UseFlag)
{
	return LockGuard{ Target, UseFlag };
}
#	define MAKE_LOCKGUARD(Name, Param1, Param2) MakeLockGuardImpl(Param1, Param2)

#endif //_DEBUG
