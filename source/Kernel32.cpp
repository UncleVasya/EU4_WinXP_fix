// Kernel32.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Kernel32.h"
#include <windows.h>


// This is the constructor of a class that has been exported.
// see Kernel32.h for the class definition
CKernel32::CKernel32()
{
	return;
}


typedef struct _CLIENT_ID
{
    DWORD UniqueProcess; 
    DWORD UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef LONG NTSTATUS;
#define STATUS_SUCCESS ((NTSTATUS) 0x00000000)

typedef LONG KPRIORITY;

typedef struct _THREAD_BASIC_INFORMATION
{
    NTSTATUS   ExitStatus;
    PVOID      TebBaseAddress;
    CLIENT_ID  ClientId;
    KAFFINITY  AffinityMask;
    KPRIORITY  Priority;
    KPRIORITY  BasePriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

typedef enum _THREAD_INFORMATION_CLASS
{
    ThreadBasicInformation = 0,
    //ThreadTimes,
    //ThreadPriority,
    //ThreadBasePriority,
    //ThreadAffinityMask,
    //ThreadImpersonationToken,
    //ThreadDescriptorTableEntry,
    //ThreadEnableAlignmentFaultFixup,
    //ThreadEventPair,
    //ThreadQuerySetWin32StartAddress,
    //ThreadZeroTlsCell,
    //ThreadPerformanceCount,
    //ThreadAmILastThread,
    //ThreadIdealProcessor,
    //ThreadPriorityBoost,
    //ThreadSetTlsArrayAddress,
    //ThreadIsIoPending,
    //ThreadHideFromDebugger
} THREAD_INFORMATION_CLASS, *PTHREAD_INFORMATION_CLASS;

typedef NTSTATUS (__stdcall *pfnNtQueryInformationThread) (HANDLE, THREAD_INFORMATION_CLASS, PVOID, ULONG, PULONG);
pfnNtQueryInformationThread NtQueryInformationThread;


// Hooked GetThreadId().

extern "C" DWORD WINAPI GetThreadId(HANDLE Thread)
{
	// load NtQueryInformationThread function
    HMODULE hModule = LoadLibrary((LPCSTR) "ntdll.dll");
    NtQueryInformationThread = (pfnNtQueryInformationThread) GetProcAddress(hModule, "NtQueryInformationThread");
    if (NtQueryInformationThread == NULL)
        return 0;	// failed to get proc address

    // use function
    THREAD_BASIC_INFORMATION tbi;
    THREAD_INFORMATION_CLASS tic = ThreadBasicInformation;
    if (NtQueryInformationThread(Thread, tic, &tbi, sizeof(tbi), NULL) != STATUS_SUCCESS)
    {
        // NtQueryInformationThread failed...
        FreeLibrary(hModule);
        return 0;
    }

    // print uniqe thread id
    DWORD thread_id = tbi.ClientId.UniqueThread;

    // clean up
    FreeLibrary(hModule);

    return thread_id;
}
