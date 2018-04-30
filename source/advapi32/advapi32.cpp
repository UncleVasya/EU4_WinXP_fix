// advapi.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"


extern "C" LONG WINAPI RegGetValueA(
	HKEY hkey, LPCTSTR lpSubKey, LPCTSTR lpValue, DWORD dwFlags, 
	LPDWORD pdwType, PVOID pvData, LPDWORD pcbData)
{
	LSTATUS result = RegQueryValueExA(hkey, lpSubKey, NULL, NULL, (LPBYTE) pvData, pcbData);
	return result;
}