#include "stdafx.h"
#include "advapi32.h"


extern "C" LONG WINAPI RegGetValueA(
	HKEY hkey, LPCTSTR lpSubKey, LPCTSTR lpValue, DWORD dwFlags, 
	LPDWORD pdwType, PVOID pvData, LPDWORD pcbData)
{
	LSTATUS result = RegQueryValueExA(hkey, lpSubKey, NULL, NULL, (LPBYTE) pvData, pcbData);
	return result;
}