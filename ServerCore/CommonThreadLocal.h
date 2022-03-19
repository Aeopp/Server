#pragma once

class CurrentThread
{
public:
	static inline thread_local uint32 UniqueId{ 0u };
};

