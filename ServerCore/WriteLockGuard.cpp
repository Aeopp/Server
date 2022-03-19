#include "Precompiled.h"
#include "WriteLockGuard.h"

WriteLockGuard::WriteLockGuard(Lock& Target) :
	_Lock{ Target }
{
	_Lock.WriteLock();
}

WriteLockGuard::~WriteLockGuard()
{
	_Lock.WriteUnlock();
}
