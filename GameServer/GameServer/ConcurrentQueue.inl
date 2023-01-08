template <typename T>
::LockQueue<T>::LockQueue()
	: mutex_{}
	, queue_{}
	, condVar_{}
{
}

template <typename T>
template <typename ... Types>
void LockQueue<T>::Push(Types&&... value) noexcept
{
	lock_guard lock{ mutex_ };
	queue_.emplace(std::forward<Types...>(value)...);
	condVar_.notify_one();
}

template <typename T>
bool LockQueue<T>::TryPop(T& value) noexcept
{
	lock_guard lock(mutex_);
	if (queue_.empty())
		return false;

	// empty -> front -> pop
	value = move(queue_.front());
	queue_.pop();
	return true;
}

template <typename T>
void LockQueue<T>::WaitPop(T& value)
{
	unique_lock lock(mutex_);
	condVar_.wait(lock, [this] {return queue_.empty() == false; });
	value = move(queue_.front());
	queue_.pop();
}
