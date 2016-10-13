#include "stdafx.h"
#include "kernel32.h"


extern "C" BOOL WINAPI InitializeCriticalSectionEx(
	LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount, DWORD Flags)
{
	BOOL result = InitializeCriticalSectionAndSpinCount(
		 lpCriticalSection, dwSpinCount);
	
	return result;
}
