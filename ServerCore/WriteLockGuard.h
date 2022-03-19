#pragma once

class WriteLockGuard
{
public:
	WriteLockGuard(Lock& Target);
	~WriteLockGuard();

private:
	Lock& _Lock;
};

