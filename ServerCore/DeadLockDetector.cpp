#include "Precompiled.h"
#ifdef _DEBUG
#include "DeadLockDetector.h"

DeadLockDetector::DeadLockDetector() :
	_DiscoveredCount{0}
{
}

void DeadLockDetector::Initialize() & noexcept
{
}

void DeadLockDetector::PushLock(LockInfo&& PushInfo)
{
	std::lock_guard Lock{ _Lock };
	const LockIDType PrevLockID = RegistLockInfo(PushInfo);
	const LockIDType CurrentLockID = PushInfo.ID;
	constexpr LockIDType LockStackEmpty = -1;
	
	const bool IsNotEmpty = LockStackEmpty != PrevLockID;
	const bool IsNonRecursiveLock = PrevLockID != CurrentLockID;
	if (IsNonRecursiveLock && IsNotEmpty)
	{
		std::set<LockIDType>& LockHistory = _LockHistory[PrevLockID];
		// 재귀적인 락을 허용하므로 이미 과거에 Lock 한 적이 있다? -> 검사할 필요 없다.
		const auto [CurLockIDIter,IsUnlocked] = LockHistory.emplace(CurrentLockID);
		if (IsUnlocked)
		{
			DeadlockScan(CurrentLockID);
		}
	}
}

void DeadLockDetector::PopLock(const LockIDType PopId)
{
	ASSERT_CRASH(false == _LockStack.empty(), "Multiple unlock!");
	ASSERT_CRASH(_LockStack.top() == PopId, "Incorrect lock and unlock order");
	const bool IsContainsLockId =
		std::any_of(begin(_NameToLockID), end(_NameToLockID), [PopId](const decltype(_NameToLockID)::value_type& NameToLockId)
			{
				return NameToLockId.second == PopId;
			});
	ASSERT_CRASH(IsContainsLockId && _LockIDToName.contains(PopId), "is not contains lockId");

	_LockStack.pop();
}

void DeadLockDetector::DeadlockScan(const LockIDType TargetLockID)
{
	const LockIDType LockCount = static_cast<LockIDType>(_LockIDToName.size());
	_DiscoveredOrder.resize(LockCount, -1);
	_Parent.resize(LockCount, -1);
	_Finished.resize(LockCount, false);
	_DiscoveredCount = 0;

	for(const auto [LockID,Name] : _LockIDToName)
	{
		Dfs(LockID);
	}

	_DiscoveredOrder.clear();
	_Finished.clear();
	_Parent.clear();
}

void DeadLockDetector::Dfs(const LockIDType CurrentLockID)
{
	if (-1 != _DiscoveredOrder[CurrentLockID])
		return;

	_DiscoveredOrder[CurrentLockID] = _DiscoveredCount++;

	const auto FindHistory = _LockHistory.find(CurrentLockID);
	const bool IsNoOtherLocks = std::end(_LockHistory) == FindHistory;
	if(IsNoOtherLocks)
	{
		_Finished[CurrentLockID] = true;
		return;
	}

	const std::set<int32>& LockHistory = FindHistory->second;
	for(const auto VisitLockID : LockHistory)
	{
		const bool IsNeverVisited = -1 == _DiscoveredOrder[VisitLockID];
		if(IsNeverVisited)
		{
			_Parent[VisitLockID] = CurrentLockID;
			Dfs(VisitLockID);
			continue;
		}

		// 방문 순서가 Current < Visit 이므로 사이클은 아니다 Current->Visit
		const bool IsForwardOrder = _DiscoveredOrder[CurrentLockID] < _DiscoveredOrder[VisitLockID];
		if (IsForwardOrder)
		{
			continue;
		}

		// 방문 순서가 역전되었고 Dfs(VisitLockID) 아직 종료 전이므로 Current<->Visit인 상황
		if(false == _Finished[VisitLockID])
		{
			std::stringstream DeadlockLogBuilder{};
			DeadlockLogBuilder << "Deadlock Detected\n";
			DeadlockLogBuilder << "-------------------------\n";
			const std::string_view VisitLogFormat = "{} -> {}\n"sv;
			DeadlockLogBuilder << std::format(VisitLogFormat, _LockIDToName[CurrentLockID], _LockIDToName[VisitLockID]);
			LockIDType Now = CurrentLockID;
			while(true)
			{
				DeadlockLogBuilder << std::format(VisitLogFormat, _LockIDToName[_Parent[Now]], _LockIDToName[Now]);
				Now = _Parent[Now];
				if (Now == VisitLockID)
					break;
			}
			DeadlockLogBuilder << "-------------------------";
			CRASH(DeadlockLogBuilder.str());
		}
	}

	_Finished[CurrentLockID] = true;
}

static void RecusiveLockPrint(const DeadLockDetector::LockInfo& PrintInfo)
{
	std::cout << std::format("recursive lock : [{},{}]", PrintInfo.Name, PrintInfo.ID) << std::endl;
}

DeadLockDetector::LockIDType DeadLockDetector::RegistLockInfo(LockInfo&& PushInfo)
{
	const auto [NameAndLockId, IsNameToLockInsert] = _NameToLockID.try_emplace(PushInfo.Name, PushInfo.ID);
	const auto [LockIdAndName, IsIdToNameInsert] = _LockIDToName.try_emplace(PushInfo.ID, PushInfo.Name);

	LockIDType TopLockID = -1;
	if(false == _LockStack.empty())
		TopLockID = _LockStack.top();

	_LockStack.push(PushInfo.ID);
	RecusiveLockPrint(PushInfo);
	return TopLockID;
}

DeadLockDetector::LockIDType DeadLockDetector::RegistLockInfo(const LockInfo& PushInfo)
{
	const auto [NameAndLockId, IsNameToLockInsert] = _NameToLockID.try_emplace(PushInfo.Name, PushInfo.ID);
	const auto [LockIdAndName, IsIdToNameInsert ] = _LockIDToName.try_emplace(PushInfo.ID, PushInfo.Name);

	LockIDType TopLockID = -1;
	if (false == _LockStack.empty())
		TopLockID = _LockStack.top();

	_LockStack.push(PushInfo.ID);
	RecusiveLockPrint(PushInfo);
	return TopLockID;
}
#endif //_DEBUG
