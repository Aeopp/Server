template<typename T>
LockFreeStack<T>::CountedNodePtr::CountedNodePtr()
	: ptr{ nullptr }
	, externalCount { 0 }
{
}

template <typename T>
LockFreeStack<T>::LockFreeStack()
	: head_{}
{
}

template <typename T>
template <typename ... Types>
void LockFreeStack<T>::Push(Types&&... args) noexcept
{
	CountedNodePtr node;
	node.ptr = new Node{ forward<Types>(args)... };
	node.externalCount = 1;

	node.ptr->next = head_;
	while (false == head_.compare_exchange_weak(node.ptr->next, node))
	{
	}
}

// #1 head �б�
// #2 head->next �б�
// #3 head = head->next
// #4 data �����ؼ� ��ȯ
// #5 ������ ��带 ����

// [ ][ ][ ][ ][ ]
// [head]
template <typename T>
shared_ptr<T> LockFreeStack<T>::TryPop() noexcept
{
	CountedNodePtr oldHead = head_;
	while(true)
	{
		// ������ ȹ��
		IncreaseHeadCount(oldHead);

		Node* ptr = oldHead.ptr;

		// ������ ����
		if (ptr == nullptr)
			return {};

		// ������ ȹ�� (ptr->next�� head�� �ٲ�ġ�� �� �ְ� �̱�)
		if (head_.compare_exchange_strong(oldHead, ptr->next))
		{
			shared_ptr<T> res;
			res.swap(ptr->data);

			// �� ���� �� ���� �ִ°�?
			const int32 countIncrease = oldHead.externalCount - 2;

			if (ptr->internalCount.fetch_add(countIncrease) == -countIncrease)
				delete ptr;

			return res;
		}
		else if (ptr->internalCount.fetch_sub(1) == 1)
		{
			// �������� �������, �������� ���� -> �� ������ ���� �Ѵ�
			delete ptr;
		}
	}
}

template<typename T>
void LockFreeStack<T>::IncreaseHeadCount(CountedNodePtr& oldCounter)
{
	while(true)
	{
		CountedNodePtr newCounter = oldCounter;
		newCounter.externalCount++;

		if(head_.compare_exchange_strong(oldCounter, newCounter))
		{
			oldCounter.externalCount = newCounter.externalCount;
			break;
		} 
	}
}

template <typename T>
template <typename ... Types>
LockFreeStack<T>::Node::Node(Types&&... args)
	: data{ make_shared<T>(std::forward<Types>(args)...) }
	, next{}
	, internalCount{ 0 }
{
}

template <typename T>
::LockStack<T>::LockStack()
	: mutex_{}
	, stack_{}
	, condVar_{}
{
}

template <typename T>
template <typename ... Types>
void LockStack<T>::Push(Types&&... value) noexcept
{
	lock_guard lock{ mutex_ };
	stack_.emplace(std::forward<Types...>(value)...);
	condVar_.notify_one();
}

template <typename T>
bool LockStack<T>::TryPop(T& value) noexcept
{
	lock_guard lock(mutex_);
	if (stack_.empty())
		return false;

	// empty -> top -> pop
	value = move(stack_.top());
	stack_.pop();
	return true;
}

template <typename T>
void LockStack<T>::WaitPop(T&& value)
{
	unique_lock lock(mutex_);
	condVar_.wait(lock, [this] {return stack_.empty() == false; });
	value = move(stack_.top());
	stack_.pop();
}
