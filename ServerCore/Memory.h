#pragma once
#include "Allocator.h"
#include "DefineMemory.h"

template<typename Type, typename ...Params>
Type* xnew(Params&&... Args)
{
	constexpr uint64 ALLOCATE_SIZE = sizeof(Type);
	Type* const Memory = xalloc<Type>(ALLOCATE_SIZE); // 할당
	return new(Memory)Type(std::forward<Params>(Args)...); // 생성자 호출
}

template<typename Type>
void xdelete(Type* Target)
{
	Target->~Type();
	xrelease(Target);
}

