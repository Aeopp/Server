#include "pch.h"

class SpinLock
{
public:
	using DelayUnit = TimeUnit;
	static constexpr auto TRY_LIMIT_COUNT = 5000u;
	static constexpr DelayUnit DEFAULT_DELAY = 64ms;
	explicit SpinLock(const DelayUnit delay = DEFAULT_DELAY);

	void lock() noexcept;
	void unlock() noexcept;

private:
	std::atomic<bool> locked_;
	DelayUnit delay_;
};

SpinLock::SpinLock(const DelayUnit delay)
	: locked_{ false }
	, delay_{ delay }
{

}

void SpinLock::lock() noexcept
{
	// CAS (Compare-And-Swap)

	bool expected = false;
	constexpr bool desired = true;

	// CAS (Compare-And-Swap) 의사 코드
	//if (locked_ == expected)
	//{
	//	expected = locked_;
	//	locked_ = desired;
	//	return true;
	//}
	//else
	//{
	//	expected = locked_;
	//	return false;
	//}
	std::decay_t<decltype(TRY_LIMIT_COUNT)> tryCount = 0u;

	while (false == locked_.compare_exchange_strong(expected, desired))
	{
		expected = false;

		if(++tryCount > TRY_LIMIT_COUNT)
		{
			tryCount = 0;
			std::this_thread::sleep_for(delay_);
		}
	}
}

inline void SpinLock::unlock() noexcept
{
	locked_.store(false);
}

int main()
{
	int value = 0;
	SpinLock spinLock;

	auto testFn = [&value, &spinLock]()
	{
		for (int i = 0; i < 100'000; ++i)
		{
			spinLock.lock();
			++value;
			spinLock.unlock();
		}
	};

	std::vector<std::jthread> tasks;

	constexpr auto taskNumber = 256;

	for (std::decay_t<decltype(taskNumber)> i = 0; i < taskNumber; ++i)
	{
		tasks.emplace_back(testFn);
	}

	for(std::jthread& task : tasks)
	{
		task.join();
	}

	std::cout << "value : " << value << std::endl;

	return 0;
}
