#pragma once
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
