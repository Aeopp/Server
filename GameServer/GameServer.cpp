#include "Precompiled.h"
#include "ThreadManager.h"
#include <queue>
#include <array>
#include "LockGuard.h"

class TestLock
{
public:
	int32 Read()
	{
		int Data = -1;

		{
#ifdef _DEBUG
			LockGuard Lock{ _Lock, LockGuard::Use::Read,typeid(this).name() };
#else //_DEBUG
			LockGuard Lock{ _Lock, LockGuard::Use::Read };
#endif //_DEBUG

			if (_Queue.empty())
				return Data;

			Data = _Queue.front();
		}

		return Data;
	}

	void Push()
	{
		{
#ifdef _DEBUG
			LockGuard Lock{ _Lock, LockGuard::Use::Write,typeid(this).name() };
#else //_DEBUG
			LockGuard Lock{ _Lock, LockGuard::Use::Write };
#endif //_DEBUG

			static uint32 TestCount{ 0u };
			_Queue.push(++TestCount);
		}
		std::this_thread::sleep_for(1ms);
	}

	void Pop()
	{
		{
#ifdef _DEBUG
			LockGuard Lock{ _Lock, LockGuard::Use::Write,typeid(this).name() };
#else //_DEBUG
			LockGuard Lock{ _Lock, LockGuard::Use::Write };
#endif //_DEBUG

			if (false == _Queue.empty())
			{
				_Queue.pop();
			}
		}
		std::this_thread::sleep_for(1ms);
	}

private:
	std::queue<int32> _Queue;
	Lock _Lock;
};

class DeadlockTester
{
public:
	static void Case1()
	{
		auto Tester_1 = []()
		{
			LockGuard Lock1{ _Locks[0], LockGuard::Use::Read, "Tester_1"};
			LockGuard Lock2{ _Locks[1], LockGuard::Use::Read, "Tester_1" };
		};

		auto Tester_2 = []()
		{
			LockGuard Lock2{ _Locks[1], LockGuard::Use::Read, "Tester_2" };
			LockGuard Lock1{ _Locks[0], LockGuard::Use::Read, "Tester_2" };
		};

		ThreadManager::Instance().Launch(Tester_1);
		ThreadManager::Instance().Launch(Tester_2);
	}

	static void Case2()
	{
		auto Tester_1 = []()
		{
			LockGuard Lock1{ _Locks[0], LockGuard::Use::Read, "Tester_1" };
			LockGuard Lock2{ _Locks[1], LockGuard::Use::Read, "Tester_1" };
		};

		auto Tester_2 = []()
		{
			LockGuard Lock2{ _Locks[1], LockGuard::Use::Read, "Tester_2" };
			LockGuard Lock3{ _Locks[2], LockGuard::Use::Read, "Tester_2" };
		};

		auto Tester_3 = []()
		{
			LockGuard Lock3{ _Locks[2], LockGuard::Use::Read, "Tester_3" };
			LockGuard Lock1{ _Locks[0], LockGuard::Use::Read, "Tester_3" };
		};

		ThreadManager::Instance().Launch(Tester_1);
		ThreadManager::Instance().Launch(Tester_2);
		ThreadManager::Instance().Launch(Tester_3);
	}

	static void Case3()
	{
		auto Tester_1 = []()
		{
			LockGuard Lock1{ _Locks[0], LockGuard::Use::Read, "Tester_1" };
			LockGuard Lock2{ _Locks[1], LockGuard::Use::Read, "Tester_1" };
		};

		auto Tester_2 = []()
		{
			LockGuard Lock2{ _Locks[1], LockGuard::Use::Read, "Tester_2" };
			LockGuard Lock3{ _Locks[2], LockGuard::Use::Read, "Tester_2" };
		};

		auto Tester_3 = []()
		{
			LockGuard Lock3{ _Locks[2], LockGuard::Use::Read, "Tester_3" };
			LockGuard Lock4{ (rand() % 2) == 0 ? _Locks[3] : _Locks[0], LockGuard::Use::Read, "Tester_3" };
		};

		auto Tester_4 = []()
		{
			LockGuard Lock4{ _Locks[3], LockGuard::Use::Read, "Tester_4" };
			LockGuard Lock1{ _Locks[0], LockGuard::Use::Read, "Tester_4" };
		};

		ThreadManager::Instance().Launch(Tester_1);
		ThreadManager::Instance().Launch(Tester_2);
		ThreadManager::Instance().Launch(Tester_3);
		ThreadManager::Instance().Launch(Tester_4);
	}

private:
	static inline std::array<Lock, 4u> _Locks{};
};


static void TestCase1()
{

};


int main()
{
	CoreGlobal::Instance().Initialize();
	//DeadlockTester::Case1();
	DeadlockTester::Case3();
	//DeadlockTester::Case3();

	// 데드락 케이스 1 : 0 <-> 1 
	


	//auto TestReader = [](TestLock& _TestLock)
	//{
	//	while(true)
	//	{
	//		const auto Value = _TestLock.Read();
	//		std::cout << Value << std::endl;
	//		std::this_thread::sleep_for(1ms);
	//	}
	//};

	//auto TestPush = [](TestLock& _TestLock)
	//{
	//	while (true)
	//	{
	//		_TestLock.Push();
	//		std::this_thread::sleep_for(1ms);
	//	}
	//};

	//auto TestPop = [](TestLock& _TestLock)
	//{
	//	while (true)
	//	{
	//		_TestLock.Pop();
	//		std::this_thread::sleep_for(1ms);
	//	}
	//};

	//TestLock _TestLock;

	//for (uint32 i = 0; i < 3; ++i)
	//{
	//	ThreadManager::Instance().Launch(TestPush, std::ref(_TestLock));
	//}

	//for (uint32 i = 0; i < 3; ++i)
	//{
	//	ThreadManager::Instance().Launch(TestPop, std::ref(_TestLock));
	//}

	//for (uint32 i = 0; i < 10; ++i)
	//{
	//	ThreadManager::Instance().Launch(TestReader, std::ref(_TestLock));
	//}

	ThreadManager::Instance().Join();
}
