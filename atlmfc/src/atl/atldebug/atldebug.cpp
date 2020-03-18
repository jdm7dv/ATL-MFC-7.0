#include "StdAfx.H"
#include "..\AtlDebugTools\Common.h"
#include "..\AtlDebugTools\Allocate.h"
#include "..\AtlDebugTools\Lock.h"
#include <math.h>

HMODULE g_hInst;

// necessary for printf & co.
static double loadFP = fabs( 1.3 );

BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD dwReason, LPVOID)
{
    if(dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hInst);
		g_hInst = hInst;

		const int nSize = 64;
		char szFileMappingName[nSize], szMutexName[nSize], szEventName[nSize];

		sprintf(szFileMappingName, g_pszKernelObjFmt,
			g_pszAllocFileMapName, GetCurrentProcessId());
		sprintf(szMutexName, g_pszKernelObjFmt,
			g_pszAllocMutexName, GetCurrentProcessId());

		if(g_Allocator.Init(szFileMappingName, szMutexName, 1024 * 1024))
		{
			sprintf(szEventName, g_pszKernelObjFmt,
				g_pszLockEventName, GetCurrentProcessId());
			sprintf(szMutexName, g_pszKernelObjFmt,
				g_pszLockMutexName, GetCurrentProcessId());

			if(g_Lock.Init(szMutexName, szEventName))
				return TRUE;
		}
		return FALSE;
	}
	return TRUE;
}

