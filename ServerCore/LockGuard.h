#pragma once

class LockGuard
{
public:
	enum class Use : uint32
	{
		Read,
		Write,
	};

public:
	LockGuard(Lock& Target, const Use UseFlag);
#ifdef _DEBUG
	LockGuard(Lock& Target, const Use UseFlag, const std::string& Name);
#endif //_DEBUG

	~LockGuard();
private:
	Lock& _Lock;
	Use _UseFlag;
	std::string _Name;
};

