#include "pch.h"
#include "ConcurrentStack.hpp"
#include "SpinLock.h"

int main()
{
	LockFreeStack<int> ts;
	vector<jthread> ths{};
	ths.reserve(10u);

	for(int i=0;i<ths.capacity()/2;++i)
	{
		ths.emplace_back([&ts]
		{
			for(int i=0;i<1'0000000000;++i)
			{
				ts.Push(rand());
			}
		});
	}

	for(int i = ths.capacity()/2;i < ths.capacity();++i)
	{
		ths.emplace_back([&ts]()
		{
			int value = -1;
			while(true)
			{
				auto data = ts.TryPop();
				if (nullptr == data)
					return;

				std::cout << "pop value : " << *data << std::endl;
			}
		});
	}

	return 0;
}
