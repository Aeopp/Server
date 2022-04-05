#include "Precompiled.h"
#include "CommonGlobal.h"
#include "ThreadManager.h"
#include "DeadLockDetector.h"

CoreGlobal::CoreGlobal()
{
	
}

CoreGlobal::~CoreGlobal() noexcept
{

}

void CoreGlobal::Initialize()
{
	Memory::Instance().Initialize();
	ThreadManager::Instance().Initialize();
#ifdef _DEBUG
	DeadLockDetector::Instance().Initialize();
#endif //_DEBUG
}
