#include "Precompiled.h"
#include "CommonGlobal.h"
#include "ThreadManager.h"

CoreGlobal::CoreGlobal()
{
	
}

CoreGlobal::~CoreGlobal() noexcept
{

}

void CoreGlobal::Initialize()
{
	ThreadManager::Instance().Initialize();

}
