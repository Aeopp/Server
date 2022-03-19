#pragma once
#include "SingletonInterface.hpp"

class CoreGlobal final : public SingleTonInterface<CoreGlobal>
{
public:
	CoreGlobal();
	~CoreGlobal() noexcept;

public:
	void Initialize();

};
