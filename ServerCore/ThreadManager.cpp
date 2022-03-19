#include "Precompiled.h"
#include "ThreadManager.h"

ThreadManager::ThreadManager()
{
	// 메인 스레드용 초기화
	InitializeTLS();
}

ThreadManager::~ThreadManager() noexcept
{
	Join();
}

void ThreadManager::Initialize() noexcept
{
	
}

void ThreadManager::Join()
{
	std::for_each(std::begin(_Threads), std::end(_Threads)
		, [](auto& Worker)
		{
			if (Worker.joinable())
			{
				Worker.join();
			}
		});

	_Threads.clear();
}

void ThreadManager::InitializeTLS() noexcept
{
	static std::atomic<uint32> UniqueThreadId{0u };
	CurrentThread::UniqueId = UniqueThreadId.fetch_add(1u);
}

void ThreadManager::ReleaseTLS() noexcept
{

}
