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
bool LockFreeStack<T>::TryPop(T&& value) noexcept
{
	Node* oldHead = head_;

	//if(head_==oldhead)
	//{
	//	head_ = oldHead->next;
	//	return true;
	//}
	//else
	//{
	//	oldHead = head_;
	//	return false;
	//}

	while (oldHead && 
		(false == head_.complare_exchange_weak(oldHead, oldHead->next)))
	{
		// oldHead = head_;
	}

	if (nullptr == oldHead)
		return false;

	// ���ܴ� ������� �ʴ´�.
	value = move(oldHead->data);

	// ��� ���� ����
	return true;
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
bool LockStack<T>::TryPop(T&& value) noexcept
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
