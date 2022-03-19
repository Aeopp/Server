#include "Precompiled.h"

int main()
{
	int* a = new int{1};
	ASSERT_CRASH(a);
}