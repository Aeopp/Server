#include "pch.h"
#include "SpinLock.h"

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

		if (++tryCount > TRY_LIMIT_COUNT)
		{
			tryCount = 0;
			std::this_thread::sleep_for(delay_);
		}
	}

}

void SpinLock::unlock() noexcept
{
	locked_.store(false);
}
