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

// #1 head 읽기
// #2 head->next 읽기
// #3 head = head->next
// #4 data 추출해서 반환
// #5 추출한 노드를 삭제

// [ ][ ][ ][ ][ ]
// [head]
template <typename T>
shared_ptr<T> LockFreeStack<T>::TryPop() noexcept
{
	CountedNodePtr oldHead = head_;
	while(true)
	{
		// 참조권 획득
		IncreaseHeadCount(oldHead);

		Node* ptr = oldHead.ptr;

		// 데이터 없음
		if (ptr == nullptr)
			return {};

		// 소유권 획득 (ptr->next로 head를 바꿔치기 한 애가 이김)
		if (head_.compare_exchange_strong(oldHead, ptr->next))
		{
			shared_ptr<T> res;
			res.swap(ptr->data);

			// 나 말고 또 누가 있는가?
			const int32 countIncrease = oldHead.externalCount - 2;

			if (ptr->internalCount.fetch_add(countIncrease) == -countIncrease)
				delete ptr;

			return res;
		}
		else if (ptr->internalCount.fetch_sub(1) == 1)
		{
			// 참조권은 얻었으나, 소유권은 실패 -> 뒷 수습은 내가 한다
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
