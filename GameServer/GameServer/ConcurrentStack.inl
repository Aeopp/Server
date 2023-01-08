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

	// :: 의사 코드
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

// #1 head 읽기
// #2 head->next 읽기
// #3 head = head->next
// #4 data 추출해서 반환
// #5 추출한 노드를 삭제

// [ ][ ][ ][ ][ ]
// [head]
template <typename T>
bool LockFreeStack<T>::TryPop(T& value) noexcept
{
	Node* oldHead = head_;

	// 탑 바로 아래의 노드로 헤드를 교체해서 이전 헤드의 소유권을 획득한다.
	//if(head_ == oldhead)
	//{
	//	head_ = oldHead->next;
	//	return true;
	//}

	// 데이터를 경합하는 도중 다른 스레드가 Head를 바꾼 경우 실패한다.
	// oldHead를 다른 스레드가 갱신한 헤드로 교체하고 소유권 획득을 계속 시도한다.
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

	// 예외는 고려하지 않는다.
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
		// 삭제 대상 노드의 소유권은 나만 가지고 있다.


		// 삭제 예약 노드들의 소유권 획득을 시도한다.
		Node* node = pendingList_.exchange(nullptr);
		if (0 == --popCount_)
		{
			// 삭제 예약 노드의 소유권도 독점 중이다.
			// 삭제해도 무방하다. 해당 컨텍스트에 진입 직전에 exchange로
			// 노드를 분리해놨기 때문에 다른 스레드에서 끼어들더라도 상관 없다.
			DeleteNodes(node);
		}
		else
		{
			// 소유권 독점에 실패했으므로 롤백
			ChainPendingNodeList(node);
		}

		delete target;
		return true;
	}
	else
	{
		// 삭제 대상 노드의 소유권 확보에 실패하였다.
		// 삭제 예약 리스트에 삽입한다.
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
