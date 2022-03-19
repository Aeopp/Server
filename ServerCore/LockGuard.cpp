#include "Precompiled.h"
#include "LockGuard.h"

#ifdef _DEBUG
LockGuard::LockGuard(Lock& Target, const Use UseFlag, const std::string& Name)
	: LockGuard(Target, UseFlag)
{
	_Name = std::format("[ ID:{} Class:{} ]", Target.GetLockID(), Name);
	if (_UseFlag == Use::Read)
	{
		_Lock.ReadLock(_Name);
	}
	else
	{
		_Lock.WriteLock(_Name);
	}

}
#endif //_DEBUG

LockGuard::LockGuard(Lock& Target, const Use UseFlag) :
	_Lock{ Target }
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
}
#ifdef _DEBUG
LockGuard::~LockGuard()
{
	if (_UseFlag == Use::Read)
	{
		if (_Name.empty())
		{
			_Lock.ReadUnlock();
		}
		else
		{
			_Lock.ReadUnlock(_Name);
		}
	}
	else
	{
		if (_Name.empty())
		{
			_Lock.WriteUnlock();
		}
		else
		{
			_Lock.WriteUnlock(_Name);
		}
	}
}
#else //_DEBUG
LockGuard::~LockGuard()
{
	if (_UseFlag == Use::Read)
	{
		_Lock.ReadUnlock();
	}
	else
	{
		_Lock.WriteUnlock();
	}
}
#endif //_DEBUG
