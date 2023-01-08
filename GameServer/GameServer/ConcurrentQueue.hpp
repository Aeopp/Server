#pragma once
#include "pch.h"
template<typename T>
class LockQueue
{
public:
	explicit LockQueue();

	explicit LockQueue(const LockQueue&) = delete;
	LockQueue& operator= (const LockQueue&) = delete;

	template<typename... Types>
	void Push(Types&&... value) noexcept;
	bool TryPop(T&& value) noexcept;
	void WaitPop(T&& value);

private:
	mutex mutex_;
	queue<T> queue_;
	condition_variable condVar_;
};

#include "ConcurrentQueue.inl"
