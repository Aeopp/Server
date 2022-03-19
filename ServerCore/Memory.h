#pragma once
#include "Allocator.h"
#include "DefineMemory.h"

template<typename Type, typename ...Params>
Type* xnew(Params&&... Args)
{
	constexpr uint64 ALLOCATE_SIZE = sizeof(Type);
	Type* const Memory = xalloc<Type>(ALLOCATE_SIZE); // 할당

#ifdef _DEBUG
	auto Log = std::format("allocate [{}], size[{}]\n", typeid(Type).name(), ALLOCATE_SIZE);
	std::cout << Log;
#endif //_DEBUG

	return new(Memory)Type(std::forward<Params>(Args)...); // 생성자 호출
}

template<typename Type>
void xdelete(Type* Target)
{
#ifdef _DEBUG
	auto Log = std::format("release [{}]\n", typeid(Type).name());
	std::cout << Log;
#endif //_DEBUG
	Target->~Type();
	xrelease(Target);
}

