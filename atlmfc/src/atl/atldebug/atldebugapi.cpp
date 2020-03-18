// This is a part of the Active Template Library.
// Copyright (C) 1996-2000 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the	
// Active Template Library product.

#include "StdAfx.h"

#include "Common.h"
#include "AtlTraceModuleManager.h"

namespace ATL
{
static bool ShouldTraceOutput(HINSTANCE hInst,
							  UINT nCategory,
							  UINT nLevel,
							  const CAtlTraceCategory **ppCategory,
							  CAtlTraceModule::fnCrtDbgReport_t *pfnCrtDbgReport);

void NotifyTool()
{
	HANDLE hEvent;
	hEvent = OpenEventA(EVENT_MODIFY_STATE, FALSE, g_pszUpdateEventName);

	if(hEvent)
	{
		SetEvent(hEvent);
		CloseHandle(hEvent);
	}
}

// API
BOOL __stdcall AtlTraceRegister(HINSTANCE hInst,
								int (__cdecl *fnCrtDbgReport)(int,const char *,int,const char *,const char *,...))
{
	UINT nModule;

	CAtlAllocatorLock lock(&g_Allocator);
	if(!lock.Locked())
		return FALSE;

	BOOL bRetVal = FALSE;

	CAtlTraceModule *pModule;
	if(g_Allocator.FindModule(hInst, &nModule))
		g_Allocator.RemoveModule(nModule);

	nModule = g_Allocator.AddModule(hInst);
	pModule = g_Allocator.GetModule(nModule);
	ATLASSERT(pModule);
	if(pModule)
	{
		bRetVal = TRUE;
		pModule->CrtDbgReport(fnCrtDbgReport);
		NotifyTool();
	}

	return bRetVal;
}

void __stdcall AtlTraceUnregister(HINSTANCE hInst)
{
	CAtlAllocatorLock lock(&g_Allocator);
	if(!lock.Locked())
		return;

	UINT nModule;
	if(g_Allocator.FindModule(hInst, &nModule) &&
		g_Allocator.RemoveModule(nModule))
	{
		if(!g_Allocator.GetModuleCount())
			g_Allocator.CleanUp();
		NotifyTool();
	}
}

BOOL __stdcall AtlTraceRegisterCategoryA(HINSTANCE hInst, UINT nCategory, const CHAR szCategoryName[MAX_NAME_SIZE])
{
	USES_CONVERSION;
	return AtlTraceRegisterCategoryU(hInst, nCategory, A2W(szCategoryName));
}

BOOL __stdcall AtlTraceRegisterCategoryU(HINSTANCE hInst, UINT nCategory, const WCHAR szCategoryName[MAX_NAME_SIZE])
{
	CAtlAllocatorLock lock(&g_Allocator);
	if(!lock.Locked())
		return FALSE;

	BOOL bRetVal = FALSE;

	UINT nModule;
	if(g_Allocator.FindModule(hInst, &nModule) &&
		!g_Allocator.GetCategory(nModule, nCategory))
	{
		g_Allocator.AddCategory(nModule, nCategory, szCategoryName);
		bRetVal = TRUE;
		NotifyTool();
	}

	return bRetVal;
}

BOOL __stdcall AtlTraceModifyProcess(UINT nLevel, BOOL bEnabled,
									 BOOL bFuncAndCategoryNames, BOOL bFileNameAndLineNo)
{
	CAtlAllocatorLock lock(&g_Allocator);
	if(!lock.Locked())
		return FALSE;

	BOOL bRet = FALSE;

	CAtlTraceProcess *pProcess = g_Allocator.GetProcess();
	ATLASSERT(pProcess);
	if(pProcess)
	{
		pProcess->m_nLevel = nLevel;
		pProcess->m_bEnabled = 0 != bEnabled;
		pProcess->m_bFuncAndCategoryNames = 0 != bFuncAndCategoryNames;
		pProcess->m_bFileNameAndLineNo = 0 != bFileNameAndLineNo;

		bRet = TRUE;
	}
	return bRet;
}

BOOL __stdcall AtlTraceModifyModule(HINSTANCE hInst, UINT nLevel, ATLTRACESTATUS eStatus)
{
	CAtlAllocatorLock lock(&g_Allocator);
	if(!lock.Locked())
		return FALSE;

	UINT nModule;
	BOOL bRet = FALSE;
	if(g_Allocator.FindModule(hInst, &nModule))
	{
		CAtlTraceModule *pModule = g_Allocator.GetModule(nModule);
		ATLASSERT(pModule);
		if(pModule)
		{
			bRet = TRUE;
			switch(eStatus)
			{
			case ATLTRACESTATUS_INHERIT:
				pModule->m_eStatus = CAtlTraceSettings::Inherit;
				break;
			case ATLTRACESTATUS_ENABLED:
				pModule->m_eStatus = CAtlTraceSettings::Enabled;
				break;
			case ATLTRACESTATUS_DISABLED:
				pModule->m_eStatus = CAtlTraceSettings::Disabled;
				break;
			default:
				bRet = FALSE;
				break;
			}
			if(bRet)
				pModule->m_nLevel = nLevel;
		}
	}
	return bRet;
}

BOOL __stdcall AtlTraceModifyCategory(HINSTANCE hInst, UINT nCategory,
									  UINT nLevel, ATLTRACESTATUS eStatus)
{
	CAtlAllocatorLock lock(&g_Allocator);
	if(!lock.Locked())
		return FALSE;

	UINT nModule;
	BOOL bRet = FALSE;
	if(g_Allocator.FindModule(hInst, &nModule))
	{
		CAtlTraceCategory *pCategory = g_Allocator.GetCategory(nModule, nCategory);
		if(pCategory)
		{
			bRet = TRUE;
			switch(eStatus)
			{
			case ATLTRACESTATUS_INHERIT:
				pCategory->m_eStatus = CAtlTraceSettings::Inherit;
				break;
			case ATLTRACESTATUS_ENABLED:
				pCategory->m_eStatus = CAtlTraceSettings::Enabled;
				break;
			case ATLTRACESTATUS_DISABLED:
				pCategory->m_eStatus = CAtlTraceSettings::Disabled;
				break;
			default:
				bRet = FALSE;
			}
			if(bRet)
				pCategory->m_nLevel = nLevel;
		}
	}
	return bRet;
}

BOOL __stdcall AtlTraceGetProcess(UINT *pnLevel, BOOL *pbEnabled,
								  BOOL *pbFuncAndCategoryNames, BOOL *pbFileNameAndLineNo)
{
	CAtlAllocatorLock lock(&g_Allocator);
	if(!lock.Locked())
		return FALSE;

	BOOL bRet = FALSE;

	CAtlTraceProcess *pProcess = g_Allocator.GetProcess();
	ATLASSERT(pProcess);
	if(pProcess)
	{
		if(pnLevel)
			*pnLevel = pProcess->m_nLevel;
		if(pbEnabled)
			*pbEnabled = pProcess->m_bEnabled;
		if(pbFuncAndCategoryNames)
			*pbFuncAndCategoryNames = pProcess->m_bFuncAndCategoryNames;
		if(pbFileNameAndLineNo)
			*pbFileNameAndLineNo = pProcess->m_bFileNameAndLineNo;

		bRet = TRUE;
	}
	return bRet;
}

BOOL __stdcall AtlTraceGetModule(HINSTANCE hInst, UINT *pnLevel, ATLTRACESTATUS *pStatus)
{
	CAtlAllocatorLock lock(&g_Allocator);
	if(!lock.Locked())
		return FALSE;

	UINT nModule;
	BOOL bRet = FALSE;
	if(g_Allocator.FindModule(hInst, &nModule))
	{
		CAtlTraceModule *pModule = g_Allocator.GetModule(nModule);
		ATLASSERT(pModule);
		if(pModule)
		{
			if(pnLevel)
				*pnLevel = pModule->m_nLevel;

			if(pStatus)
				switch(pModule->m_eStatus)
				{
				default:
				case CAtlTraceSettings::Inherit:
					*pStatus = ATLTRACESTATUS_INHERIT;
					break;
				case CAtlTraceSettings::Enabled:
					*pStatus = ATLTRACESTATUS_ENABLED;
					break;
				case CAtlTraceSettings::Disabled:
					*pStatus = ATLTRACESTATUS_DISABLED;
					break;
				}
			bRet = TRUE;
		}
	}
	return bRet;
}

BOOL __stdcall AtlTraceGetCategory(HINSTANCE hInst, UINT nCategory, UINT *pnLevel,
								   ATLTRACESTATUS *pStatus)
{
	CAtlAllocatorLock lock(&g_Allocator);
	if(!lock.Locked())
		return FALSE;

	UINT nModule;
	BOOL bRet = FALSE;
	if(g_Allocator.FindModule(hInst, &nModule))
	{
		CAtlTraceCategory *pCategory = g_Allocator.GetCategory(nModule, nCategory);
		ATLASSERT(pCategory);
		if(pCategory)
		{
			if(pnLevel)
				*pnLevel = pCategory->m_nLevel;

			if(pStatus)
				switch(pCategory->m_eStatus)
				{
				case CAtlTraceSettings::Inherit:
					*pStatus = ATLTRACESTATUS_INHERIT;
					break;
				case CAtlTraceSettings::Enabled:
					*pStatus = ATLTRACESTATUS_ENABLED;
					break;
				case CAtlTraceSettings::Disabled:
					*pStatus = ATLTRACESTATUS_DISABLED;
					break;
				}
			bRet = TRUE;
		}
	}
	return bRet;
}

void __stdcall AtlTraceGetUpdateEventNameA(CHAR *pszEventName)
{
	lstrcpyA(pszEventName, g_pszUpdateEventName);
}

void __stdcall AtlTraceGetUpdateEventNameU(WCHAR *pszEventName)
{
	USES_CONVERSION;
	lstrcpyW(pszEventName, A2W(g_pszUpdateEventName));
}

void __cdecl AtlTraceVA(HINSTANCE hInst, const char *pszFileName, int nLine,
						UINT nCategory, UINT nLevel, const CHAR *pszFormat, va_list ptr)
{
	CAtlAllocatorLock lock(&g_Allocator);

	const CAtlTraceCategory *pCategory;
	CAtlTraceModule::fnCrtDbgReport_t pfnCrtDbgReport = NULL;
	static const int nCount = 1024;
	CHAR szBuf[nCount] = {'\0'};
	int nLen = 0;

	if(lock.Locked())
		if(ShouldTraceOutput(hInst, nCategory, nLevel, &pCategory, &pfnCrtDbgReport))
		{
			if(g_Allocator.GetProcess()->m_bFileNameAndLineNo)
				nLen += _snprintf(szBuf + nLen, nCount - nLen, "%s(%d) : ", pszFileName, nLine);

			if(pCategory && g_Allocator.GetProcess()->m_bFuncAndCategoryNames)
				nLen += _snprintf(szBuf + nLen, nCount - nLen, "%S: ", pCategory->Name());
		}
		else
			return;

	_vsnprintf(szBuf + nLen, nCount - nLen, pszFormat, ptr);

	if(pfnCrtDbgReport)
		pfnCrtDbgReport(_CRT_WARN, NULL, 0, NULL, "%s", szBuf);
	else
		OutputDebugStringA(szBuf);
}

void __cdecl AtlTraceVU(HINSTANCE hInst, const char *pszFileName, int nLine,
						UINT nCategory, UINT nLevel, const WCHAR *pszFormat, va_list ptr)
{
	CAtlAllocatorLock lock(&g_Allocator);

	const CAtlTraceCategory *pCategory;
	CAtlTraceModule::fnCrtDbgReport_t pfnCrtDbgReport = NULL;
	const int nCount = 1024;
	WCHAR szBuf[nCount] = {L'\0'};
	int nLen = 0;

	if(lock.Locked())
		if(ShouldTraceOutput(hInst, nCategory, nLevel, &pCategory, &pfnCrtDbgReport))
		{
			if(g_Allocator.GetProcess()->m_bFileNameAndLineNo)
				nLen += _snwprintf(szBuf + nLen, nCount - nLen, L"%S(%d) : ", pszFileName, nLine);

			if(pCategory && g_Allocator.GetProcess()->m_bFuncAndCategoryNames)
				nLen += _snwprintf(szBuf + nLen, nCount - nLen, L"%s: ", pCategory->Name());
		}
		else
			return;

	_vsnwprintf(szBuf + nLen, nCount - nLen, pszFormat, ptr);

	if(pfnCrtDbgReport)
		pfnCrtDbgReport(_CRT_WARN, NULL, 0, NULL, "%S", szBuf);
	else
		OutputDebugStringW(szBuf);
}


// REVIEW: Necessary?
/*void __cdecl AtlTraceU(HINSTANCE hInst, UINT nCategory, UINT nLevel, const WCHAR *szFormat, ...)
{
	va_list ptr;
	va_start(ptr, szFormat);
	AtlTraceVU(hInst, nCategory, nLevel, szFormat, ptr);
	va_end(ptr);
}

void __cdecl AtlTraceA(HINSTANCE hInst, UINT nCategory, UINT nLevel, const CHAR *szFormat, ...)
{
	va_list ptr;
	va_start(ptr, szFormat);
	AtlTraceVA(hInst, nCategory, nLevel, szFormat, ptr);
	va_end(ptr);
}*/

BOOL __stdcall AtlTraceOpen(DWORD dwId)
{
	static DWORD dwLastId = 0xffffffff;

	if(dwLastId != dwId || !g_Allocator.IsValid())
	{
		char szBuf1[64], szBuf2[64];

		g_Allocator.Close(true);

		sprintf(szBuf1, g_pszKernelObjFmt, g_pszAllocFileMapName, dwId);
		sprintf(szBuf2, g_pszKernelObjFmt, g_pszAllocMutexName, dwId);

		if(!g_Allocator.Open(szBuf1, szBuf2))
			return FALSE;

		dwLastId = dwId;
	}
	return TRUE;
}

void __stdcall AtlTraceClose(void)
{
	g_Allocator.Close(true);
}

BOOL __stdcall AtlTraceGetProcessInfoA(ATLTRACEPROCESSINFOA *pProcessInfo)
{
	CAtlAllocatorLock lock(&g_Allocator);
	if(!lock.Locked())
		return FALSE;

	BOOL bRetVal = FALSE;
	
	ATLASSERT(pProcessInfo);
	CAtlTraceProcess *pProcess = g_Allocator.GetProcess();
	ATLASSERT(pProcess);

	if(pProcess)
	{
		USES_CONVERSION;
		lstrcpyA(pProcessInfo->szName, W2A(pProcess->Name()));
		lstrcpyA(pProcessInfo->szPath, W2A(pProcess->Path()));
		pProcessInfo->dwId = pProcess->Id();
		pProcessInfo->Settings.nLevel = pProcess->m_nLevel;
		pProcessInfo->Settings.bEnabled = pProcess->m_bEnabled;
		pProcessInfo->Settings.bFuncAndCategoryNames = pProcess->m_bFuncAndCategoryNames;
		pProcessInfo->Settings.bFileNameAndLineNo = pProcess->m_bFileNameAndLineNo;

		bRetVal = TRUE;
	}
	return bRetVal;
}

BOOL __stdcall AtlTraceGetProcessInfoU(ATLTRACEPROCESSINFOU *pProcessInfo)
{
	CAtlAllocatorLock lock(&g_Allocator);
	if(!lock.Locked())
		return FALSE;

	BOOL bRetVal = FALSE;
	
	ATLASSERT(pProcessInfo);
	CAtlTraceProcess *pProcess = g_Allocator.GetProcess();
	ATLASSERT(pProcess);

	if(pProcess)
	{
		lstrcpyW(pProcessInfo->szName, pProcess->Name());
		lstrcpyW(pProcessInfo->szPath, pProcess->Path());
		pProcessInfo->dwId = pProcess->Id();
		pProcessInfo->Settings.nLevel = pProcess->m_nLevel;
		pProcessInfo->Settings.bEnabled = pProcess->m_bEnabled;
		pProcessInfo->Settings.bFuncAndCategoryNames = pProcess->m_bFuncAndCategoryNames;
		pProcessInfo->Settings.bFileNameAndLineNo = pProcess->m_bFileNameAndLineNo;

		bRetVal = TRUE;
	}
	return bRetVal;
}

int __stdcall AtlTraceGetModuleInfoA(ATLTRACEMODULEINFOA *pModuleInfo)
{
	CAtlAllocatorLock lock(&g_Allocator);
	if(!lock.Locked())
		return 0;

	UINT nCount = g_Allocator.GetModuleCount();
	if(pModuleInfo)
	{
		USES_CONVERSION;
		for(UINT i = 0; i < nCount; i++)
		{
			// REVIEW: conversion in loop
			
			CAtlTraceModule *pModule = g_Allocator.GetModule(i);
			lstrcpyA(pModuleInfo[i].szName, W2A(pModule->Name()));
			lstrcpyA(pModuleInfo[i].szPath, W2A(pModule->Path()));
			pModuleInfo[i].hInst = pModule->GetInstance();
			pModuleInfo[i].Settings.nLevel = pModule->m_nLevel;
			switch(pModule->m_eStatus)
			{
			default:
			case CAtlTraceSettings::Inherit:
				pModuleInfo[i].Settings.eStatus = ATLTRACESTATUS_INHERIT;
				break;
			case CAtlTraceSettings::Enabled:
				pModuleInfo[i].Settings.eStatus = ATLTRACESTATUS_ENABLED;
				break;
			case CAtlTraceSettings::Disabled:
				pModuleInfo[i].Settings.eStatus = ATLTRACESTATUS_DISABLED;
				break;
			}
		}
	}
	return nCount;
}

int __stdcall AtlTraceGetModuleInfoU(ATLTRACEMODULEINFOU *pModuleInfo)
{
	CAtlAllocatorLock lock(&g_Allocator);
	if(!lock.Locked())
		return 0;

	UINT nCount = g_Allocator.GetModuleCount();
	if(pModuleInfo)
	{
		for(UINT i = 0; i < nCount; i++)
		{
			CAtlTraceModule *pModule = g_Allocator.GetModule(i);
			lstrcpyW(pModuleInfo[i].szName, pModule->Name());
			lstrcpyW(pModuleInfo[i].szPath, pModule->Path());
			pModuleInfo[i].hInst = pModule->GetInstance();
			pModuleInfo[i].Settings.nLevel = pModule->m_nLevel;
			switch(pModule->m_eStatus)
			{
			default:
			case CAtlTraceSettings::Inherit:
				pModuleInfo[i].Settings.eStatus = ATLTRACESTATUS_INHERIT;
				break;
			case CAtlTraceSettings::Enabled:
				pModuleInfo[i].Settings.eStatus = ATLTRACESTATUS_ENABLED;
				break;
			case CAtlTraceSettings::Disabled:
				pModuleInfo[i].Settings.eStatus = ATLTRACESTATUS_DISABLED;
				break;
			}
		}
	}
	return nCount;
}

int __stdcall AtlTraceGetCategoryInfoA(HINSTANCE hInst, ATLTRACECATEGORYINFOA *pCategoryInfo)
{
	CAtlAllocatorLock lock(&g_Allocator);
	if(!lock.Locked())
		return 0;

	UINT nModule, nCount = 0;
	
	if(g_Allocator.FindModule(hInst, &nModule))
	{
		if(!pCategoryInfo)
			return g_Allocator.GetCategoryCount(nModule);

		CAtlTraceModule *pModule = g_Allocator.GetModule(nModule);
		ATLASSERT(pModule);
		if(pModule &&
			g_Allocator.IsValidCategoryIndex(pModule->m_nFirstCategory))
		{
			// REVIEW: conversion in loop
			USES_CONVERSION;
			UINT nOldIndex, nIndex = pModule->m_nFirstCategory;
			CAtlTraceCategory *pCategory;
			do
			{
				pCategory = g_Allocator.GetCategoryByIndex(nIndex);

				lstrcpyA(pCategoryInfo[nCount].szName, W2A(pCategory->Name()));
				pCategoryInfo[nCount].hInst = hInst;
				pCategoryInfo[nCount].nCategory = pCategory->m_nCategory;
				pCategoryInfo[nCount].Settings.nLevel = pCategory->m_nLevel;

				switch(pCategory->m_eStatus)
				{
				default:
				case CAtlTraceSettings::Inherit:
					pCategoryInfo[nCount].Settings.eStatus = ATLTRACESTATUS_INHERIT;
					break;
				case CAtlTraceSettings::Enabled:
					pCategoryInfo[nCount].Settings.eStatus = ATLTRACESTATUS_ENABLED;
					break;
				case CAtlTraceSettings::Disabled:
					pCategoryInfo[nCount].Settings.eStatus = ATLTRACESTATUS_DISABLED;
					break;
				}

				nCount++;
				nOldIndex = nIndex;
				nIndex = pCategory->m_nNext;
			}
			while(nOldIndex != nIndex);
		}
	}
	return nCount;
}

int __stdcall AtlTraceGetCategoryInfoU(HINSTANCE hInst, ATLTRACECATEGORYINFOU *pCategoryInfo)
{
	CAtlAllocatorLock lock(&g_Allocator);
	if(!lock.Locked())
		return 0;

	UINT nModule, nCount = 0;

	if(g_Allocator.FindModule(hInst, &nModule))
	{
		if(!pCategoryInfo)
			nCount = g_Allocator.GetCategoryCount(nModule);
		else
		{
			CAtlTraceModule *pModule = g_Allocator.GetModule(nModule);
			ATLASSERT(pModule);

			if(g_Allocator.IsValidCategoryIndex(pModule->m_nFirstCategory))
			{
				UINT nOldIndex, nIndex = pModule->m_nFirstCategory;
				CAtlTraceCategory *pCategory;
				do
				{
					pCategory = g_Allocator.GetCategoryByIndex(nIndex);

					lstrcpyW(pCategoryInfo[nCount].szName, pCategory->Name());
					pCategoryInfo[nCount].hInst = hInst;
					pCategoryInfo[nCount].nCategory = pCategory->m_nCategory;
					pCategoryInfo[nCount].Settings.nLevel = pCategory->m_nLevel;
					switch(pCategory->m_eStatus)
					{
					default:
					case CAtlTraceSettings::Inherit:
						pCategoryInfo[nCount].Settings.eStatus = ATLTRACESTATUS_INHERIT;
						break;
					case CAtlTraceSettings::Enabled:
						pCategoryInfo[nCount].Settings.eStatus = ATLTRACESTATUS_ENABLED;
						break;
					case CAtlTraceSettings::Disabled:
						pCategoryInfo[nCount].Settings.eStatus = ATLTRACESTATUS_DISABLED;
						break;
					}

					nCount++;
					nOldIndex = nIndex;
					nIndex = pCategory->m_nNext;
				}
				while(nOldIndex != nIndex);
			}
		}
	}
	return nCount;
}

static bool ShouldTraceOutput(HINSTANCE hInst,
							  UINT nCategory,
							  UINT nLevel,
							  const CAtlTraceCategory **ppCategory,
							  CAtlTraceModule::fnCrtDbgReport_t *pfnCrtDbgReport)
{
	bool bFound = false;

	ATLASSERT(ppCategory && pfnCrtDbgReport);
	*ppCategory = NULL;
	*pfnCrtDbgReport = NULL;

	CAtlTraceProcess *pProcess = g_Allocator.GetProcess();
	ATLASSERT(pProcess);

	UINT nModule;
	if(g_Allocator.FindModule(hInst, &nModule))
	{
		CAtlTraceModule *pModule = g_Allocator.GetModule(nModule);
		ATLASSERT(pModule);
		if(pModule)
		{
			*pfnCrtDbgReport = pModule->CrtDbgReport();

			if(g_Allocator.IsValidCategoryIndex(pModule->m_nFirstCategory))
			{
				UINT nOldIndex, nIndex = pModule->m_nFirstCategory;
				CAtlTraceCategory *pCategory;
				do
				{
					pCategory = g_Allocator.GetCategoryByIndex(nIndex);
					if(pCategory->m_nCategory == nCategory)
					{
						bFound = true;
						bool bOut = false;

						if(pProcess->m_bEnabled &&
							pModule->m_eStatus == CAtlTraceSettings::Inherit &&
							pCategory->m_eStatus == CAtlTraceSettings::Inherit &&
							nLevel <= pProcess->m_nLevel)
						{
							bOut = true;
						}
						else
							if(pModule->m_eStatus == CAtlTraceSettings::Enabled &&
								pCategory->m_eStatus == CAtlTraceSettings::Inherit &&
								nLevel <= pModule->m_nLevel)
							{
								bOut = true;
							}
							else
								if(pCategory->m_eStatus == CAtlTraceSettings::Enabled &&
									nLevel <= pCategory->m_nLevel)
								{
									bOut = true;
								}

						if(bOut)
						{
							*ppCategory = pProcess->m_bFuncAndCategoryNames ? pCategory : NULL;
							return true;
						}
						break;
					}
					nOldIndex = nIndex;
					nIndex = pCategory->m_nNext;
				}
				while(nOldIndex != nIndex);
			}
		}
		else
			return false;
	}

	// If there is no such category (or if there's just *nothing*) go ahead
	// and spit out the request (i.e., using the tool is the only way to *prevent*
	// traces from occurring
	if(!bFound)
		return true;
	return false;
}																							

};  // namespace ATL