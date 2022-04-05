#pragma once
#include <string_view>
template<typename Type>
constexpr const char* GET_TYPENAME()
{
	return typeid(Type).name();
};

template<typename Type>
constexpr const char* GET_TYPENAME(Type PtrType)
{
	using DecayType = std::decay_t<Type>;
	if (nullptr == PtrType)
	{
		return typeid(DecayType).name();
	}
	
	return GET_TYPENAME<DecayType>();
};
/* -----------------
		Log
-------------------*/
template<typename LoggableType>
static inline void LOG(LoggableType&& Log)
{
	std::cout << Log;
}

/*-------------------
	Crash
 -------------------*/
template<typename ...LoggableType>
static inline constexpr void CRASH(LoggableType&&... Because) noexcept
{
	// TODO :: cause ..
	LOG(std::forward<LoggableType>(Because)...);
	uint32* Dead = nullptr;
	__analysis_assume(Dead != nullptr);
	*Dead = 0xDEADBEEF;
}

template<typename ExpressionType, typename ...LoggableType>
static inline constexpr void ASSERT_CRASH(const ExpressionType& Expression
	, LoggableType&&... Because) noexcept
{
	if (!Expression)
	{
		CRASH(std::forward<LoggableType>(Because)...);
		__analysis_assume(Expression);
	}
}

/*-------------------
	TODO :: ...??
 -------------------*/


 //#define CRASH(Because)													\
 //{																		\
 //	uint32* Crash = nullptr;											\
 //	__analysis_assume(Crash != nullptr);								\
 //	*Crash = 0xDEADBEEF;												\
 //}