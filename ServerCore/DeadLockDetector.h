#pragma once
#ifdef _DEBUG

class DeadLockDetector : public SingleTonInterface<DeadLockDetector>
{
public:
	using LockIDType = int32;
	struct LockInfo
	{
		using KeyType = LockIDType;
		using ValueType = std::string; 
		KeyType ID{};
		ValueType Name{};
	};

public:
	DeadLockDetector();

public:
	void Initialize() & noexcept;

public:
	void PushLock(LockInfo&& PushInfo);
	template<typename ...LockInfoParams>
	void EmplaceLock(LockInfoParams&&... LockInfoArgs)
	{
		PushLock(LockInfo{ std::forward<LockInfoParams>(LockInfoArgs)... });
	}
	void PopLock(const LockIDType PopId);

private:
	LockIDType RegistLockInfo(LockInfo&& PushInfo);
	LockIDType RegistLockInfo(const LockInfo& PushInfo);

	void DeadlockScan(const LockIDType TargetLockID);
	void Dfs(const LockIDType CurrentLockID);

private:
	static inline thread_local std::stack<LockInfo::KeyType>	_LockStack;
	std::unordered_map<LockInfo::ValueType, LockInfo::KeyType>	_NameToLockID;
	std::unordered_map<LockInfo::KeyType, LockInfo::ValueType>	_LockIDToName;
	std::map<LockIDType, std::set<LockIDType>>					_LockHistory;

	std::mutex _Lock;

private:
	std::vector<int32>											_DiscoveredOrder;
	int32														_DiscoveredCount;
	std::vector<bool>											_Finished;
	std::vector<int32>											_Parent;

};
#endif //_DEBUG
