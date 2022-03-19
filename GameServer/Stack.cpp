#include "Precompiled.h"
//#include "Stack.h"
//
//Concurrent::Stack::Stack() :
//	Top{nullptr }
//	, PopCount{0u }
//	, PendingList{nullptr }
//{
//}
//
//void Concurrent::Stack::Push(const T& Value) & noexcept
//{
//	return _PushLockFree(new Node{ Value });
//}
//
//void Concurrent::Stack::Push(T&& Value) & noexcept
//{
//	return _PushLockFree(new Node{ move(Value) });
//}
//
//optional<Concurrent::T> Concurrent::Stack::TryPop() & noexcept
//{
//	PopCount.fetch_add(+1);
//
//	auto CurTop = Top.load();
//	while (CurTop 
//		&& false == Top.compare_exchange_weak(CurTop, CurTop->Next))
//	{
//		__noop;
//	}
//
//	if (nullptr == CurTop)
//	{
//		PopCount.fetch_add(-1);
//		return std::nullopt;
//	}
//
//	optional<Concurrent::T> PopData = move(CurTop->Data);
//
//	Pending(CurTop); // PopData 하려는 순간! delete가 동시에 일어나면 Crash!!
//	                 // 이 구문 때문에 귀찮은 동기화 작업을 해줘야함..
//
//	return PopData;
//}
//
//
//
