#pragma once
#include "pch.h"

template<typename T>
class LockFreeStack
{
public:
	explicit LockFreeStack();

	LockFreeStack(const LockFreeStack&) = delete;
	LockFreeStack& operator=(const LockFreeStack&) = delete;

private:
	struct Node
	{
		template<typename... Types>
		explicit Node(Types&&... args);

		friend class LockFreeStack;

		Node* next;
		T data;
	};

public:
	template<typename... Types>
	void Push(Types&&... args) noexcept;
	bool TryPop(T& value) noexcept;
	bool TryDelete(Node* const target) noexcept;

private:
	static void DeleteNodes(const Node* target);

private:
	void ChainPendingNodeList(Node* const first, Node* const last);
	void ChainPendingNodeList(Node* const node);
	void ChainPendingNode(Node* const node);

private:
	atomic<Node*> head_;
	atomic<uint64> popCount_; // 현재 pop 시도 중인 스레드 개수
	atomic<Node*> pendingList_; // 삭제 보류 노드의 Head
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
