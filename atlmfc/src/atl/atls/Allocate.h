// This is a part of the Active Template Library.
// Copyright (C) 1996-2000 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the	
// Active Template Library product.

#ifndef ATLDEBUG_SHAREDMEORY_ALLOCATOR_HEADER
#define ATLDEBUG_SHAREDMEORY_ALLOCATOR_HEADER

class CAtlTraceProcess;
class CAtlTraceModule;
class CAtlTraceCategory;

class CAtlAllocator
{
public:
	CAtlAllocator() :
		m_dwPageSize(0), 
		m_hMap(NULL), m_hMutex(NULL),
		m_bValid(false),
		m_pProcess(NULL){}

	~CAtlAllocator() {Close();}

	bool Init(const CHAR *pszFileMappingName, const CHAR *pszMutexName, DWORD dwMaxSize);
	bool Open(const CHAR *pszFileMappingName, const CHAR *pszMutexName);
	void Close(bool bForceUnmap = false);
	bool IsValid() const {return m_bValid;}

	CAtlTraceProcess *GetProcess() const {return m_pProcess;}
	CAtlTraceModule *GetModule(unsigned nModule) const;
	CAtlTraceCategory *GetCategory(unsigned nModule, unsigned nCategory) const;
	CAtlTraceCategory *GetCategoryByIndex(unsigned nIndex) const;

	bool IsValidCategoryIndex(unsigned nIndex) const;

	bool Lock(DWORD dwTimeOut);
	void Unlock();

	bool LoadSettings(const CHAR *pszFileName);
	bool LoadSettings(const WCHAR *pszFileName);
	bool SaveSettings(const CHAR *pszFileName);
	bool SaveSettings(const WCHAR *pszFileName);

	unsigned GetModuleCount() const;
	unsigned GetCategoryCount(unsigned nModule) const;
	unsigned GetCategoryCount(const CAtlTraceModule &rModule) const;

	bool FindModule(HINSTANCE hInst, unsigned *pnModule) const;
	bool FindModule(const WCHAR *pszModuleName, unsigned *pnModule) const;

	unsigned AddModule(HINSTANCE hInst);
	unsigned AddCategory(unsigned nModule, unsigned nCategory, const WCHAR *szCategoryName);

	bool RemoveModule(UINT nIndex);

	void CleanUp();

private:
	CAtlTraceProcess *m_pProcess;
	DWORD m_dwPageSize;
	HANDLE m_hMap, m_hMutex;
	bool m_bValid;
	BYTE *m_pBufferStart;
};

class CAtlAllocatorLock
{
public:
	CAtlAllocatorLock(CAtlAllocator *pAllocator) : m_pAllocator(pAllocator), m_bLocked(false)
	{
		ATLASSERT(m_pAllocator);
		if(m_pAllocator && m_pAllocator->IsValid())
			m_bLocked = m_pAllocator->Lock(2000);
	}
	~CAtlAllocatorLock(){if(m_bLocked) m_pAllocator->Unlock();}

	bool Locked() const {return m_bLocked;}

private:
	bool m_bLocked;
	CAtlAllocator *m_pAllocator;
};

#endif // ATLDEBUG_SHAREDMEORY_ALLOCATOR_HEADER
