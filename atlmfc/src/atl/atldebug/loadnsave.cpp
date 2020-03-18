// This is a part of the Active Template Library.
// Copyright (C) 1996-2000 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the	
// Active Template Library product.

#include "stdafx.h"
#include "Common.h"
#include "AtlTraceModuleManager.h"

namespace ATL
{
void NotifyTool();

static bool SetSettings(CAtlTraceSettings *pTraceSettings, UINT nLevel, UINT nStatus)
{
	ATLASSERT(pTraceSettings);
	if(!pTraceSettings)
		return false;

	pTraceSettings->m_nLevel = nLevel;
	switch(nStatus)
	{
	case 0:
		pTraceSettings->m_eStatus = CAtlTraceSettings::Inherit;
		break;
	case 1:
		pTraceSettings->m_eStatus = CAtlTraceSettings::Enabled;
		break;
	case 2:
	default:
		pTraceSettings->m_eStatus = CAtlTraceSettings::Disabled;
		break;
	}
	return true;
}

static bool GetSettings(const CAtlTraceSettings &rTraceSettings, UINT *pnStatus)
{
	ATLASSERT(pnStatus);
	if(!pnStatus)
		return false;

	switch(rTraceSettings.m_eStatus)
	{
	case CAtlTraceSettings::Inherit:
		*pnStatus = 0;
		break;
	case CAtlTraceSettings::Enabled:
		*pnStatus = 1;
		break;
	case CAtlTraceSettings::Disabled:
	default:
		*pnStatus = 2;
		break;
	}
	return true;
}

BOOL __stdcall AtlTraceLoadSettingsA(const CHAR *pszFileName, BOOL bForceLoad)
{
	CAtlAllocatorLock lock(&g_Allocator);
	if(!lock.Locked())
		return FALSE;

	CHAR szFileName[MAX_PATH];
	if(!pszFileName)
	{
		::GetModuleFileNameA(NULL, szFileName, MAX_PATH);
		CHAR *pszExe = strstr(szFileName, ".exe");
		if(pszExe)
		{
			lstrcpyA(pszExe, ".trc");
			pszFileName = szFileName;
		}
	}

	HANDLE hFile = ::CreateFileA(pszFileName, 0, FILE_SHARE_WRITE, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if(INVALID_HANDLE_VALUE != hFile)
	{
		// file exists
		::CloseHandle(hFile);

		CHAR szSection[MAX_PATH], szKey[MAX_PATH], szValue[MAX_PATH];
		CHAR szName[MAX_PATH];
		UINT nModules, nCategories, nStatus, nLevel;
		UINT nModule, nCategory;
		CAtlTraceProcess *pProcess;
		CAtlTraceModule *pModule;
		CAtlTraceCategory *pCategory;
		CHAR *pszProcess = "Process";
		CHAR cEnabled, cFuncAndCategoryNames, cFileNameAndLineInfo;

		pProcess = g_Allocator.GetProcess();
		ATLASSERT(pProcess);
		if(!pProcess)
			return FALSE;

		if(!pProcess->m_bLoaded || bForceLoad)
		{
			pProcess->m_bLoaded = true;

			::GetPrivateProfileStringA(pszProcess, "Info", "", szValue, MAX_PATH, pszFileName);
			if(5 != sscanf(szValue, "ModuleCount:%u, Level:%u, Enabled:%c, "
				"FuncAndCategoryNames:%c, FileNameAndLineNo:%c", &nModules, &pProcess->m_nLevel, &cEnabled,
				&cFuncAndCategoryNames, &cFileNameAndLineInfo))
			{
				return FALSE;
			}
			pProcess->m_bEnabled = cEnabled != 'f';
			pProcess->m_bFuncAndCategoryNames = cFuncAndCategoryNames != 'f';
			pProcess->m_bFileNameAndLineNo = cFileNameAndLineInfo != 'f';

			// REVIEW: conversion in loop
			USES_CONVERSION;
			for(UINT i = 0; i < nModules; i++)
			{
				sprintf(szKey, "Module%d", i);
				::GetPrivateProfileStringA(pszProcess, szKey, "", szSection, MAX_PATH, pszFileName);

				::GetPrivateProfileStringA(szSection, "Name", "", szName, MAX_PATH, pszFileName);
				if(!g_Allocator.FindModule(A2W(szName), &nModule))
					continue;

				pModule = g_Allocator.GetModule(nModule);
				ATLASSERT(pModule);
				if(!pModule)
					return FALSE;

				::GetPrivateProfileStringA(szSection, "Settings", "", szValue, MAX_PATH, pszFileName);
				if(3 != sscanf(szValue, "CategoryCount:%u, Level:%u, Status:%u", &nCategories, &nLevel, &nStatus))
					return FALSE;

				SetSettings(pModule, nLevel, nStatus);

				for(UINT j = 0; j < nCategories; j++)
				{
					sprintf(szKey, "Category%d", j);
					::GetPrivateProfileStringA(szSection, szKey, "", szValue, MAX_PATH, pszFileName);

					if(4 != sscanf(szValue, "Category:%u, Level:%u, Status:%u, Name:%s", &nCategory, &nLevel, &nStatus, szName))
						return FALSE;

					pCategory = g_Allocator.GetCategory(nModule, nCategory);
					if(pCategory && !lstrcmpA(W2A(pCategory->Name()), szName))
						SetSettings(pCategory, nLevel, nStatus);
				}
			}
			NotifyTool();
		}
	}
	return TRUE;
}

BOOL __stdcall AtlTraceSaveSettingsA(const CHAR *pszFileName)
{
	CAtlAllocatorLock lock(&g_Allocator);
	if(!lock.Locked())
		return FALSE;

	ATLASSERT(pszFileName);
	if(!pszFileName)
		return FALSE;

	BOOL bRetVal = FALSE;

	CHAR szKey[MAX_PATH], szValue[MAX_PATH];
	UINT nCategories, nStatus;
	UINT nCategory, nOldCategory;
	CAtlTraceProcess *pProcess;
	CAtlTraceModule *pModule;
	CAtlTraceCategory *pCategory;
	LPCSTR pszProcess = "Process";

	pProcess = g_Allocator.GetProcess();
	ATLASSERT(pProcess);
	if(!pProcess)
		return FALSE;

	bRetVal = TRUE;

	sprintf(szValue, "ModuleCount:%u, Level:%u, Enabled:%c, "
		"FuncAndCategoryNames:%c, FileNameAndLineNo:%c", pProcess->ModuleCount(), pProcess->m_nLevel,
		pProcess->m_bEnabled ? 't' : 'f', pProcess->m_bFuncAndCategoryNames ? 't' : 'f',
		pProcess->m_bFileNameAndLineNo ? 't' : 'f');
	::WritePrivateProfileStringA(pszProcess, "Info", szValue, pszFileName);

	// REVIEW: conversion in loop
	USES_CONVERSION;
	for(UINT i = 0; i <  pProcess->ModuleCount(); i++)
	{
		pModule = g_Allocator.GetModule(i);
		ATLASSERT(pModule);
		if(!pModule)
			return FALSE;

		sprintf(szKey, "Module%d", i);
		::WritePrivateProfileStringA(pszProcess, szKey, W2A(pModule->Name()), pszFileName);
		GetSettings(*pModule, &nStatus);

		nCategories = g_Allocator.GetCategoryCount(i);

		::WritePrivateProfileStringA(W2A(pModule->Name()), "Name", W2A(pModule->Path()), pszFileName);
		sprintf(szValue, "CategoryCount:%u, Level:%u, Status:%u", nCategories, pModule->m_nLevel, nStatus);
		::WritePrivateProfileStringA(W2A(pModule->Name()), "Settings", szValue, pszFileName);

		if(g_Allocator.IsValidCategoryIndex(pModule->m_nFirstCategory))
		{
			int j = 0;
			nCategory = pModule->m_nFirstCategory;
			do
			{
				pCategory = g_Allocator.GetCategoryByIndex(nCategory);

				GetSettings(*pCategory, &nStatus);

				sprintf(szKey, "Category%d", j++);
				sprintf(szValue, "Category:%u, Level:%u, Status:%u, Name:%S",
					pCategory->m_nCategory, pCategory->m_nLevel, nStatus, pCategory->Name());
				::WritePrivateProfileStringA(W2A(pModule->Name()), szKey, szValue, pszFileName);

				nOldCategory = nCategory;
				nCategory = pCategory->m_nNext;
			}
			while(nOldCategory != nCategory);
		}
	}
	return bRetVal;
}

BOOL __stdcall AtlTraceLoadSettingsU(const WCHAR *pszFileName, BOOL bForceLoad)
{
	CAtlAllocatorLock lock(&g_Allocator);
	if(!lock.Locked())
		return FALSE;

	WCHAR szFileName[MAX_PATH];
	if(!pszFileName)
	{
		::GetModuleFileNameW(NULL, szFileName, MAX_PATH);
		WCHAR *pszExe = wcsstr(szFileName, L".exe");
		if(pszExe)
		{
			lstrcpyW(pszExe, L".trc");
			pszFileName = szFileName;
		}
	}

	HANDLE hFile = ::CreateFileW(pszFileName, 0, FILE_SHARE_WRITE, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if(INVALID_HANDLE_VALUE != hFile)
	{
		// file exists
		::CloseHandle(hFile);

		WCHAR szSection[MAX_PATH], szKey[MAX_PATH], szValue[MAX_PATH];
		WCHAR szName[MAX_PATH];
		UINT nModules, nCategories, nStatus, nLevel;
		UINT nModule, nCategory;
		CAtlTraceProcess *pProcess;
		CAtlTraceModule *pModule;
		CAtlTraceCategory *pCategory;
		LPCWSTR pszProcess = L"Process";
		WCHAR cEnabled, cFuncAndCategoryNames, cFileNameAndLineInfo;

		pProcess = g_Allocator.GetProcess();
		ATLASSERT(pProcess);
		if(!pProcess)
			return FALSE;

		if(!pProcess->m_bLoaded || bForceLoad)
		{
			pProcess->m_bLoaded = true;

			::GetPrivateProfileStringW(pszProcess, L"Info", L"", szValue, MAX_PATH, pszFileName);
			if(5 != swscanf(szValue, L"ModuleCount:%u, Level:%u, Enabled:%c, "
				L"FuncAndCategoryNames:%c, FileNameAndLineNo:%c", &nModules, &pProcess->m_nLevel, &cEnabled,
				&cFuncAndCategoryNames, &cFileNameAndLineInfo))
			{
				return FALSE;
			}
			pProcess->m_bEnabled = cEnabled != L'f';
			pProcess->m_bFuncAndCategoryNames = cFuncAndCategoryNames != L'f';
			pProcess->m_bFileNameAndLineNo = cFileNameAndLineInfo != L'f';

			for(UINT i = 0; i < nModules; i++)
			{
				swprintf(szKey, L"Module%d", i);
				::GetPrivateProfileStringW(pszProcess, szKey, L"", szSection, MAX_PATH, pszFileName);

				::GetPrivateProfileStringW(szSection, L"Name", L"", szName, MAX_PATH, pszFileName);
				if(!g_Allocator.FindModule(szName, &nModule))
					continue;

				pModule = g_Allocator.GetModule(nModule);
				ATLASSERT(pModule);
				if(!pModule)
					return FALSE;

				::GetPrivateProfileStringW(szSection, L"Settings", L"", szValue, MAX_PATH, pszFileName);
				if(3 != swscanf(szValue, L"CategoryCount:%u, Level:%u, Status:%u", &nCategories, &nLevel, &nStatus))
					return FALSE;

				SetSettings(pModule, nLevel, nStatus);

				for(UINT j = 0; j < nCategories; j++)
				{
					swprintf(szKey, L"Category%d", j);
					::GetPrivateProfileStringW(szSection, szKey, L"", szValue, MAX_PATH, pszFileName);

					if(4 != swscanf(szValue, L"Category:%u, Level:%u, Status:%u, Name:%s", &nCategory, &nLevel, &nStatus, szName))
						return FALSE;

					pCategory = g_Allocator.GetCategory(nModule, nCategory);
					if(pCategory && !lstrcmpW(szName, pCategory->Name()))
						SetSettings(pCategory, nLevel, nStatus);
				}
			}
			NotifyTool();
		}
	}
	return TRUE;
}

BOOL __stdcall AtlTraceSaveSettingsU(const WCHAR *pszFileName)
{
	CAtlAllocatorLock lock(&g_Allocator);
	if(!lock.Locked())
		return FALSE;

	ATLASSERT(pszFileName);
	if(!pszFileName)
		return FALSE;

	BOOL bRetVal = FALSE;

	WCHAR szKey[MAX_PATH], szValue[MAX_PATH];
	UINT nCategories, nStatus;
	UINT nCategory, nOldCategory;
	CAtlTraceProcess *pProcess;
	CAtlTraceModule *pModule;
	CAtlTraceCategory *pCategory;
	LPCWSTR pszProcess = L"Process";

	pProcess = g_Allocator.GetProcess();
	ATLASSERT(pProcess);
	if(!pProcess)
		return FALSE;

	bRetVal = TRUE;

	swprintf(szValue, L"ModuleCount:%u, Level:%u, Enabled:%c, "
		L"FuncAndCategoryNames:%c, FileNameAndLineNo:%c", pProcess->ModuleCount(), pProcess->m_nLevel,
		pProcess->m_bEnabled ? L't' : L'f', pProcess->m_bFuncAndCategoryNames ? L't' : L'f',
		pProcess->m_bFileNameAndLineNo ? L't' : L'f');
	::WritePrivateProfileStringW(pszProcess, L"Info", szValue, pszFileName);

	for(UINT i = 0; i <  pProcess->ModuleCount(); i++)
	{
		pModule = g_Allocator.GetModule(i);
		ATLASSERT(pModule);
		if(!pModule)
			return FALSE;

		swprintf(szKey, L"Module%d", i);
		::WritePrivateProfileStringW(pszProcess, szKey, pModule->Name(), pszFileName);
		GetSettings(*pModule, &nStatus);

		nCategories = g_Allocator.GetCategoryCount(i);

		::WritePrivateProfileStringW(pModule->Name(), L"Name", pModule->Path(), pszFileName);
		swprintf(szValue, L"CategoryCount:%u, Level:%u, Status:%u", nCategories, pModule->m_nLevel, nStatus);
		::WritePrivateProfileStringW(pModule->Name(), L"Settings", szValue, pszFileName);

		if(g_Allocator.IsValidCategoryIndex(pModule->m_nFirstCategory))
		{
			int j = 0;
			nCategory = pModule->m_nFirstCategory;
			do
			{
				pCategory = g_Allocator.GetCategoryByIndex(nCategory);

				GetSettings(*pCategory, &nStatus);

				swprintf(szKey, L"Category%d", j++);
				swprintf(szValue, L"Category:%u, Level:%u, Status:%u, Name:%s",
					pCategory->m_nCategory, pCategory->m_nLevel, nStatus, pCategory->Name());
				::WritePrivateProfileStringW(pModule->Name(), szKey, szValue, pszFileName);

				nOldCategory = nCategory;
				nCategory = pCategory->m_nNext;
			}
			while(nOldCategory != nCategory);
		}
	}
	return bRetVal;
}

}; // namespace ATL