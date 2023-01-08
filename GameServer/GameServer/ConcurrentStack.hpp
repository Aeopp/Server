#pragma once
#include "pch.h"

template<typename T>
class LockFreeStack
{
public:
	struct CountedNodePtr
	{
		explicit CountedNodePtr();

		int32 externalCount;
		class Node* ptr;
	};

	explicit LockFreeStack();

	LockFreeStack(const LockFreeStack&) = delete;
	LockFreeStack& operator=(const LockFreeStack&) = delete;

private:
	struct Node
	{
		template<typename... Types>
		explicit Node(Types&&... args);

		friend class LockFreeStack;

		CountedNodePtr next;
		atomic<int32> internalCount;
		shared_ptr<T> data;
	};

public:
	template<typename... Types>
	void Push(Types&&... args) noexcept;
	shared_ptr<T> TryPop() noexcept;

private:
	void IncreaseHeadCount(CountedNodePtr& oldCounter);

private:
	atomic<CountedNodePtr> head_;
};

template<typename T>
class LockStack
{
public:
	explicit LockStack();

	explicit LockStack(const LockStack&) = delete;
	LockStack& operator= (const LockStack&) = delete;

	template<typename... Types>
	void Push(Types&&... value) noexcept;
	bool TryPop(T& value) noexcept;
	void WaitPop(T&& value);

private:
	mutex mutex_;
	stack<T> stack_;
	condition_variable condVar_;
};
#include "ConcurrentStack.inl"
