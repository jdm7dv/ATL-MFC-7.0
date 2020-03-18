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

extern HINSTANCE g_hInst;

CAtlTraceModuleInfo::CAtlTraceModuleInfo(HINSTANCE hInst)
{
	Reset(hInst);
}

void CAtlTraceModuleInfo::Reset(HINSTANCE hInst)
{
	WCHAR szModulePath[MAX_PATH] = {L'\0'};
	if(0 != (GetVersion() & 0x80000000))
	{
		USES_CONVERSION;
		CHAR szTemp[MAX_PATH];
		if(GetModuleFileNameA(hInst, szTemp, MAX_PATH))
			wcscpy(szModulePath, A2W(szTemp));
	}
	else
		GetModuleFileNameW(hInst, szModulePath, MAX_PATH);

	wcsncpy(m_szPath, szModulePath, MAX_PATH);
	WCHAR *pszShortName = m_szPath + wcslen(m_szPath);
	while(pszShortName > m_szPath && *(pszShortName - 1) != L'\\')
		pszShortName--;
	wcsncpy(m_szName, pszShortName, MAX_NAME_SIZE);

	m_hInst = hInst;
}

// Categories
CAtlTraceCategory::CAtlTraceCategory(UINT nCategory, const WCHAR *pszName)
	: m_nCategory(nCategory), m_bUsed(true)
{
	Reset(nCategory, pszName);
}

void CAtlTraceCategory::Reset(UINT nCategory, const WCHAR *pszName)
{
	wcsncpy(m_szName, pszName, MAX_NAME_SIZE);
	m_nCategory = nCategory;
}

// Modules
CAtlTraceModule::CAtlTraceModule(HINSTANCE hInst)
: CAtlTraceModuleInfo(hInst), m_pfnCrtDbgReport(NULL), m_nFirstCategory((UINT) ~0), m_bUsed(true)
{
}

void CAtlTraceModule::CrtDbgReport(CAtlTraceModule::fnCrtDbgReport_t pfnCrtDbgReport)
{
#ifdef _DEBUG
	m_pfnCrtDbgReport = pfnCrtDbgReport ? pfnCrtDbgReport : _CrtDbgReport;
#else
	m_pfnCrtDbgReport = pfnCrtDbgReport ? pfnCrtDbgReport : NULL;
#endif
}

// Processes
CAtlTraceProcess::CAtlTraceProcess(DWORD_PTR dwMaxSize) :
	CAtlTraceModuleInfo(NULL),
	m_dwId(GetCurrentProcessId()), m_nRef(1), m_dwMaxSize(dwMaxSize),
	m_dwFrontAlloc(0), m_dwBackAlloc(0), m_dwCurrFront(0), m_dwCurrBack(0),
	m_nLevel(0), m_bLoaded(false), m_bEnabled(true), m_bFuncAndCategoryNames(false), m_bFileNameAndLineNo(false)
{
	m_pvBase = this;
}

