#include "Precompiled.h"
#include "ThreadManager.h"
#include <queue>
#include "LockGuard.h"

class TestLock
{
public:
	int32 Read()
	{
		int Data = -1;

		{
			LockGuard Lock{ _Lock, LockGuard::Use::Read };

			if (_Queue.empty())
				return Data;

			Data = _Queue.front();
		}

		return Data;
	}

	void Push()
	{
		LockGuard Lock{ _Lock, LockGuard::Use::Write };
		static uint32 TestCount{ 0u };
		_Queue.push(TestCount++);
	}

	void Pop()
	{
		LockGuard Lock{ _Lock, LockGuard::Use::Write };
		if (false == _Queue.empty())
		{
			_Queue.pop();
		}
	}

	bool isEmpty() const
	{
		return _Queue.empty();
	}

private:
	std::queue<int32> _Queue;
	Lock _Lock;
};

TestLock _TestLock{};

TEST(TESTTEST,tt)
{
	
}

constexpr int TestCount = 10'000;
constexpr int PushThreadCount = 3;
constexpr int PopThreadCount = 3;
constexpr int ReadThreadCount = 10;

int PopCount = 0;
int PushCount = 0;


void BuildTestData()
{
	auto TestReader = [](TestLock& _TestLock)
	{
		while (false == _TestLock.isEmpty())
		{
			const auto Value = _TestLock.Read();
		}
	};

	auto TestPush = [](TestLock& _TestLock)
	{
		for (int i = 0; i < TestCount; ++i)
		{
			_TestLock.Push();
			++PushCount;
		}
	};

	auto TestPop = [](TestLock& _TestLock)
	{
		while(false == _TestLock.isEmpty())
		{
			_TestLock.Pop();
			++PopCount;
		}
	};

	TestLock _TestLock;

	for (uint32 i = 0; i < PushThreadCount; ++i)
	{
		ThreadManager::Instance().Launch(TestPush, std::ref(_TestLock));
	}

	for (uint32 i = 0; i < PopThreadCount; ++i)
	{
		ThreadManager::Instance().Launch(TestPop, std::ref(_TestLock));
	}

	for (uint32 i = 0; i < ReadThreadCount; ++i)
	{
		ThreadManager::Instance().Launch(TestReader, std::ref(_TestLock));
	}
}

int main(int argc, char** argv)
{
	CoreGlobal::Instance().Initialize();
	BuildTestData();
	::testing::InitGoogleTest(&argc, argv);
	const auto Result = RUN_ALL_TESTS();
	ThreadManager::Instance().Join();
	
	return Result;
}
