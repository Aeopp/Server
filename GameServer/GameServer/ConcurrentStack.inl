template <typename T>
LockFreeStack<T>::LockFreeStack()
	: head_{ nullptr }
	, popCount_{ 0u }
	, pendingList_ { nullptr }
{

}

template <typename T>
template <typename ... Types>
void LockFreeStack<T>::Push(Types&&... args) noexcept
{
	Node* node = new Node{forward<Types>(args)...};
	node->next = head_;

	// :: �ǻ� �ڵ�
	//if(head_ == node->next)
	//{
	//	head_ = node;
	//	return true;
	//}
	//else
	//{
	//	node->next = head_;
	//	return false;
	//}

	while(false == head_.compare_exchange_weak(node->next, node))
	{
		//node->next = head_;	
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
bool LockFreeStack<T>::TryPop(T& value) noexcept
{
	Node* oldHead = head_;

	// ž �ٷ� �Ʒ��� ���� ��带 ��ü�ؼ� ���� ����� �������� ȹ���Ѵ�.
	//if(head_ == oldhead)
	//{
	//	head_ = oldHead->next;
	//	return true;
	//}

	// �����͸� �����ϴ� ���� �ٸ� �����尡 Head�� �ٲ� ��� �����Ѵ�.
	// oldHead�� �ٸ� �����尡 ������ ���� ��ü�ϰ� ������ ȹ���� ��� �õ��Ѵ�.
	//else
	//{
	//	oldHead = head_;
	//	return false;
	//}

	while (oldHead && 
		(false == head_.compare_exchange_weak(oldHead, oldHead->next)))
	{
		// oldHead = head_;
	}

	if (nullptr == oldHead)
	{
		--popCount_;
		return false;
	}

	// ���ܴ� ������� �ʴ´�.
	value = move(oldHead->data);
	TryDelete(oldHead);
	return true;
}

template <typename T>
bool LockFreeStack<T>::TryDelete(Node* const target) noexcept
{
	const bool isExclusiveOwnerShip = (1 == popCount_);
	if(isExclusiveOwnerShip)
	{
		// ���� ��� ����� �������� ���� ������ �ִ�.


		// ���� ���� ������ ������ ȹ���� �õ��Ѵ�.
		Node* node = pendingList_.exchange(nullptr);
		if (0 == --popCount_)
		{
			// ���� ���� ����� �����ǵ� ���� ���̴�.
			// �����ص� �����ϴ�. �ش� ���ؽ�Ʈ�� ���� ������ exchange��
			// ��带 �и��س��� ������ �ٸ� �����忡�� �������� ��� ����.
			DeleteNodes(node);
		}
		else
		{
			// ������ ������ ���������Ƿ� �ѹ�
			ChainPendingNodeList(node);
		}

		delete target;
		return true;
	}
	else
	{
		// ���� ��� ����� ������ Ȯ���� �����Ͽ���.
		// ���� ���� ����Ʈ�� �����Ѵ�.
		ChainPendingNode(target);
		return false;
	}

	return false;
}

template<typename T>
void ::LockFreeStack<T>::DeleteNodes(const Node* target)
{
	while (target)
	{
		const Node* next = target->next;
		delete target;
		target = next;
	}
}

template<typename T>
void LockFreeStack<T>::ChainPendingNodeList(Node* const first, Node* const last)
{
	last->next = pendingList_;

	while(false == pendingList_.compare_exchange_weak(last->next, first))
	{
		
	}
}

template<typename T>
void LockFreeStack<T>::ChainPendingNodeList(Node* const node)
{
	if (nullptr == node)
		return;

	Node* last = node;

	while (last->next)
		last = last->next;

	ChainPendingNodeList(node, last);
}

template<typename T>
void LockFreeStack<T>::ChainPendingNode(Node* const node)
{
	ChainPendingNodeList(node, node);
}

template <typename T>
template <typename ... Types>
LockFreeStack<T>::Node::Node(Types&&... args)
	: next{ nullptr }
	, data{ std::forward<Types>(args)... }
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
