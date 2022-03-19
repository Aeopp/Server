#pragma once

template<typename SingleTonType>
class SingleTonInterface
{
public:
	template<typename ...ParamTypes>
	auto Initialize(ParamTypes&&... Args)
	{
		return static_cast<SingleTonType* const>(this)->Initialize(std::forward<ParamTypes>(Args)...);
	}

	template<typename ...ParamTypes>
	auto Release(ParamTypes&&... Args) noexcept
	{
		return static_cast<SingleTonType* const>(this)->Release(std::forward<ParamTypes>(Args)...);
	}

	static SingleTonType& Instance()
	{
		static SingleTonType _Instance{};
		return _Instance;
	}

public:
	SingleTonInterface(const SingleTonInterface&) = delete;
	SingleTonInterface& operator=(const SingleTonInterface&) = delete;
	SingleTonInterface(SingleTonInterface&&) = delete;
	SingleTonInterface& operator=(SingleTonInterface&&) = delete;

protected:
	SingleTonInterface() = default;
	~SingleTonInterface()noexcept = default;
};
