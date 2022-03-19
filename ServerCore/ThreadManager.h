#pragma once
#include "SingletonInterface.hpp"

class ThreadManager final : public SingleTonInterface<ThreadManager>
{
public:
	ThreadManager();
	~ThreadManager() noexcept;

public:
	void Initialize() noexcept;

public:
	/*
	 *	@warning Callable 항상 값으로 받음에 유의
	 */
	template<typename Callable,typename ...ParamTypes>
	void Launch(Callable Task, ParamTypes&&... Args)
	{
		auto TaskPackage = [Call = std::move(Task), &Args...]()
		{
			InitializeTLS();
			std::invoke(Call, std::forward<ParamTypes>(Args)...);
			ReleaseTLS();
		};

		{
			std::lock_guard<std::mutex>	LaunchSection{_Lock};
			_Threads.emplace_back(std::move(TaskPackage));
		}
	}

	void Join();

private:
	static void InitializeTLS() noexcept;
	static void ReleaseTLS() noexcept;

private:
	std::mutex _Lock;
	std::vector<std::jthread> _Threads;
};

