// This is a part of the Active Template Library.
// Copyright (C) 1996-2000 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the	
// Active Template Library product.

#ifndef ATLDEBUG_TRACE_MANAGER
#define ATLDEBUG_TRACE_MANAGER

#include "Allocate.h"

// Names
class CAtlTraceModuleInfo
{
public:
	explicit CAtlTraceModuleInfo(HINSTANCE hInst);

	void Reset(HINSTANCE hInst);
	HINSTANCE GetInstance() const {return m_hInst;}

	const WCHAR *Path() const {return m_szPath;}
	const WCHAR *Name() const {return m_szName;}

private:
	WCHAR m_szPath[MAX_PATH], m_szName[_MAX_FNAME];
	HINSTANCE m_hInst;
};

class CAtlTraceSettings
{
public:
	CAtlTraceSettings() : m_nLevel(0), m_eStatus(Inherit){}
	
	UINT m_nLevel;
	typedef enum Status
	{
		Inherit, Enabled, Disabled
	} Status;

	Status m_eStatus;
};

// Categories
class CAtlTraceCategory : public CAtlTraceSettings
{
public:
	CAtlTraceCategory(UINT nCategory, const WCHAR *pszName);

	UINT m_nCategory;
	const WCHAR *Name() const {return m_szName;}

	void Reset(UINT nCategory, const WCHAR *pszName);

	UINT m_nNext;
	bool m_bUsed;

private:
	WCHAR m_szName[MAX_NAME_SIZE];
};

// Modules (DLLs)
class CAtlTraceModule : public CAtlTraceModuleInfo, public CAtlTraceSettings
{
public:
	typedef int (__cdecl *fnCrtDbgReport_t)(int,const CHAR *,int,const CHAR *,const CHAR *,...);

	explicit CAtlTraceModule(HINSTANCE hInst);

	void CrtDbgReport(fnCrtDbgReport_t pfnCrtDbgReport);
	fnCrtDbgReport_t CrtDbgReport() const {return m_pfnCrtDbgReport;}

	UINT m_nFirstCategory;
	bool m_bUsed;

private:
	fnCrtDbgReport_t m_pfnCrtDbgReport;
};

// Process Info
class CAtlTraceProcess : public CAtlTraceModuleInfo
{
public:
	explicit CAtlTraceProcess(DWORD_PTR dwMaxSize);
	void Save(FILE *file, UINT nTabs) const;
	bool Load(FILE *file);

	UINT IncRef() {return ++m_nRef;}
	UINT DecRef() {return --m_nRef;}

	DWORD Id() const {return m_dwId;}
	DWORD_PTR MaxSize() const {return m_dwMaxSize;}
	void *Base() const {return m_pvBase;}

	UINT ModuleCount() const {return m_nModuleCount;}
	UINT CategoryCount() const {return m_nCategoryCount;}
	UINT IncModuleCount() {return m_nModuleCount++;}
	UINT IncCategoryCount() {return m_nCategoryCount++;}

	DWORD_PTR m_dwFrontAlloc, m_dwBackAlloc, m_dwCurrFront, m_dwCurrBack;

	UINT m_nLevel;
	bool m_bLoaded, m_bEnabled, m_bFuncAndCategoryNames, m_bFileNameAndLineNo;

private:
	DWORD m_dwId;
	DWORD_PTR m_dwMaxSize;
	UINT m_nRef;
	void *m_pvBase;

	UINT m_nModuleCount, m_nCategoryCount;
};

#endif // ATLDEBUG_TRACE_MANAGER
