#pragma once
#include "pch.h"

template<typename T>
class LockFreeStack
{
public:
	explicit LockFreeStack();

	LockFreeStack(const LockFreeStack&) = delete;
	LockFreeStack& operator=(const LockFreeStack&) = delete;

public:
	template<typename... Types>
	void Push(Types&&... args) noexcept;
	bool TryPop(T&& value) noexcept;

private:
	struct Node
	{
		template<typename... Types>
		explicit Node(Types&&... args);

		friend class LockFreeStack;

		Node* next;
		T data;
	};

private:
	atomic<Node<T>*> head_;
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
	bool TryPop(T&& value) noexcept;
	void WaitPop(T&& value);

private:
	mutex mutex_;
	stack<T> stack_;
	condition_variable condVar_;
};
#include "ConcurrentStack.inl"
