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
#include "Allocate.h"

#include "AtlTraceModuleManager.h"

bool CAtlAllocator::Init(const CHAR *pszFileName,
						 const CHAR *pszMutexName,
						 DWORD dwMaxSize)
{
	Close();

	ATLASSERT(!m_hMap && !m_pBufferStart && !m_hMutex);

	m_hMutex = CreateMutexA(NULL, TRUE, pszMutexName);
	if(!m_hMutex)
		return false;
	DWORD dwErr = GetLastError();

	__try
	{
		m_hMap = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL,
			PAGE_READWRITE | SEC_RESERVE, 0, dwMaxSize, pszFileName);
		if(!m_hMap)
			__leave;

		m_pBufferStart = (BYTE *)
			MapViewOfFile(m_hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if(!m_pBufferStart)
			__leave;

		SYSTEM_INFO si;
		GetSystemInfo(&si);

		if(dwErr == ERROR_ALREADY_EXISTS)
		{
			WaitForSingleObject(m_hMutex, INFINITE);

			m_pProcess = reinterpret_cast<CAtlTraceProcess *>(m_pBufferStart);

			// Looks like it's already mapped into this process space.
			// Let's do some checking...
			if(IsBadReadPtr(m_pProcess, sizeof(CAtlTraceProcess)) ||
				IsBadReadPtr(m_pProcess->Base(), sizeof(CAtlTraceProcess)) ||
				0 != memcmp(m_pBufferStart, m_pProcess->Base(), m_pProcess->m_dwFrontAlloc))
			{
				// something's not right
				__leave;
			}

			// sure looks valid
			m_pProcess->IncRef();
			m_pProcess = static_cast<CAtlTraceProcess *>(m_pProcess->Base());

			UnmapViewOfFile(m_pBufferStart);
			m_pBufferStart = reinterpret_cast<BYTE *>(m_pProcess);
		}
		else
		{
			// This is just in case sizeof(CAtlTraceProcess) is
			// ever > dwPageSize (doubtful that could ever
			// happen, but it's easy enough to avoid here)
			DWORD dwCurrAlloc = si.dwPageSize;
			while(dwCurrAlloc < sizeof(CAtlTraceProcess))
				dwCurrAlloc += si.dwPageSize;

			if(!VirtualAlloc(m_pBufferStart, dwCurrAlloc, MEM_COMMIT, PAGE_READWRITE))
				__leave;

			m_pProcess = new(m_pBufferStart) CAtlTraceProcess(dwMaxSize);
			m_pProcess->m_dwFrontAlloc = dwCurrAlloc;
			m_pProcess->m_dwCurrFront = sizeof(CAtlTraceProcess);
		}
		m_dwPageSize = si.dwPageSize;
		m_bValid = true;
	}
	__finally
	{
		if(!m_bValid)
		{
			if(m_pBufferStart)
			{
				UnmapViewOfFile(m_pBufferStart);
				m_pBufferStart = NULL;
			}

			if(m_hMap)
			{
				CloseHandle(m_hMap);
				m_hMap = NULL;
			}
			
			ReleaseMutex(m_hMutex);
			CloseHandle(m_hMutex);
			m_hMutex = NULL;
		}
		else
			ReleaseMutex(m_hMutex);
	}
	return m_bValid;
}
	
bool CAtlAllocator::Open(const CHAR *pszFileName, const CHAR *pszMutexName)
{
	Close();

	m_hMutex = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, pszMutexName);
	if(!m_hMutex)
		return false;

	WaitForSingleObject(m_hMutex, INFINITE);

	__try
	{
		m_hMap = OpenFileMappingA(FILE_MAP_WRITE, FALSE, pszFileName);
		if(!m_hMap)
			__leave;

		m_pBufferStart = (BYTE *)
			MapViewOfFile(m_hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if(!m_pBufferStart)
			__leave;

		m_pProcess = reinterpret_cast<CAtlTraceProcess *>(m_pBufferStart);
		m_pProcess->IncRef();

		SYSTEM_INFO si;
		GetSystemInfo(&si);
		m_dwPageSize = si.dwPageSize;

		m_bValid = true;
	}
	__finally
	{
		ReleaseMutex(m_hMutex);
		if(!m_bValid)
		{
			if(m_pBufferStart)
			{
				UnmapViewOfFile(m_pBufferStart);
				m_pBufferStart = NULL;
			}
			if(m_hMap)
			{
				CloseHandle(m_hMap);
				m_hMap = NULL;
			}

			m_pProcess = NULL;

			CloseHandle(m_hMutex);
			m_hMutex = NULL;
		}
	}
	return m_bValid;
}

void CAtlAllocator::Close(bool bForceUnmap)
{
	if(m_bValid)
	{
		WaitForSingleObject(m_hMutex, INFINITE);

		if(m_pProcess->DecRef() == 0 || bForceUnmap)
			UnmapViewOfFile(m_pBufferStart);
		m_pBufferStart = NULL;

		CloseHandle(m_hMap);
		m_hMap = NULL;

		m_bValid = false;

		ReleaseMutex(m_hMutex);
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
	}
}

bool CAtlAllocator::Lock(DWORD dwTimeOut)
{
	return WAIT_TIMEOUT != WaitForSingleObject(m_hMutex, dwTimeOut);
}

void CAtlAllocator::Unlock()
{
	ReleaseMutex(m_hMutex);
}

CAtlTraceModule *CAtlAllocator::GetModule(unsigned nModule) const
{
	if(nModule == m_pProcess->ModuleCount())
		return NULL;

	ATLASSERT(nModule < m_pProcess->ModuleCount());
	if(nModule < m_pProcess->ModuleCount())
	{
		BYTE *pb = reinterpret_cast<BYTE *>(m_pProcess) + sizeof(CAtlTraceProcess);
		return reinterpret_cast<CAtlTraceModule *>(pb) + nModule;
	}
	else
		return NULL;
}

CAtlTraceCategory *CAtlAllocator::GetCategory(unsigned nModule, unsigned nCategory) const
{
	ATLASSERT(nModule < m_pProcess->ModuleCount());

	if(nModule < m_pProcess->ModuleCount())
	{
		BYTE *pb = reinterpret_cast<BYTE *>(m_pProcess) + sizeof(CAtlTraceProcess);
		CAtlTraceModule *pModule = reinterpret_cast<CAtlTraceModule *>(pb) + nModule;

		if(IsValidCategoryIndex(pModule->m_nFirstCategory))
		{
			unsigned nOldIndex, nIndex = pModule->m_nFirstCategory;
			CAtlTraceCategory *pCategory;
			do
			{
				pCategory = GetCategoryByIndex(nIndex);
				if(pCategory->m_nCategory == nCategory)
					return pCategory;

				nOldIndex = nIndex;
				nIndex = pCategory->m_nNext;
			}
			while(nOldIndex != nIndex);
		}
	}
	return NULL;
}

bool CAtlAllocator::IsValidCategoryIndex(unsigned nIndex) const
{
	return nIndex < m_pProcess->CategoryCount();
}

CAtlTraceCategory *CAtlAllocator::GetCategoryByIndex(unsigned nIndex) const
{
	if(nIndex == m_pProcess->CategoryCount())
		return NULL;
	
	ATLASSERT(nIndex < m_pProcess->CategoryCount() || nIndex == (UINT) ~0);
	CAtlTraceCategory *pCategory = NULL;
	if(nIndex < m_pProcess->CategoryCount())
	{
		BYTE *pb = reinterpret_cast<BYTE *>(m_pProcess) + m_pProcess->MaxSize();
		pCategory = reinterpret_cast<CAtlTraceCategory *>(pb) - nIndex - 1;
	}
	return pCategory;
}

unsigned CAtlAllocator::GetCategoryCount(unsigned nModule) const
{
	unsigned nCount = 0;
	CAtlTraceModule *pModule = GetModule(nModule);
	ATLASSERT(pModule);
	if(pModule && IsValidCategoryIndex(pModule->m_nFirstCategory))
	{
		unsigned nOldIndex, nIndex = pModule->m_nFirstCategory;
		CAtlTraceCategory *pCategory;
		do
		{
			pCategory = GetCategoryByIndex(nIndex);
			nCount++;
			nOldIndex = nIndex;
			nIndex = pCategory->m_nNext;
		}
		while(nOldIndex != nIndex);
	}
	return nCount;
}

unsigned CAtlAllocator::GetCategoryCount(const CAtlTraceModule &rModule) const
{
	unsigned nCount = 0;
	if(IsValidCategoryIndex(rModule.m_nFirstCategory))
	{
		unsigned nOldIndex, nIndex = rModule.m_nFirstCategory;
		CAtlTraceCategory *pCategory;
		do
		{
			pCategory = GetCategoryByIndex(nIndex);
			nCount++;
			nOldIndex = nIndex;
			nIndex = pCategory->m_nNext;
		}
		while(nOldIndex != nIndex);
	}
	return nCount;
}

unsigned CAtlAllocator::GetModuleCount() const
{
	ATLASSERT(m_pProcess);
	return m_pProcess->ModuleCount();
}

bool CAtlAllocator::FindModule(HINSTANCE hInst, unsigned *pnModule) const
{
	if(pnModule)
		for(unsigned i = 0; i < m_pProcess->ModuleCount(); i++)
			if(hInst == GetModule(i)->GetInstance())
			{
				*pnModule = i;
				return true;
			}

	return false;
}

bool CAtlAllocator::FindModule(const WCHAR *pszModulePath, unsigned *pnModule) const
{
	if(pnModule)
		for(unsigned i = 0; i < m_pProcess->ModuleCount(); i++)
			if(0 == wcscmp(GetModule(i)->Path(), pszModulePath))
			{
				*pnModule = i;
				return true;
			}

	return false;
}

unsigned CAtlAllocator::AddModule(HINSTANCE hInst)
{
	// bug bug bug - overlap onto the categories!!??
	
	CAtlTraceProcess *pProcess = GetProcess();
	while(pProcess->m_dwFrontAlloc - pProcess->m_dwCurrFront < sizeof(CAtlTraceModule))
	{
		if(!VirtualAlloc(reinterpret_cast<BYTE *>(pProcess) + pProcess->m_dwFrontAlloc,
			m_dwPageSize, MEM_COMMIT, PAGE_READWRITE))
		{
			return NULL;
		}
		pProcess->m_dwFrontAlloc += m_dwPageSize;
	}

	CAtlTraceModule *pModule = NULL;
	UINT i;
	for(i = 0; i < m_pProcess->ModuleCount(); i++)
	{
		pModule = GetModule(i);
		ATLASSERT(pModule);
		if(!pModule->m_bUsed)
		{
			pModule->m_bUsed = true;
			pModule->Reset(hInst);
			pModule->m_nFirstCategory = (UINT) ~0;
			break;
		}
	}
	if(i == m_pProcess->ModuleCount())
	{
		pModule = 
			new(reinterpret_cast<BYTE *>(pProcess) + pProcess->m_dwCurrFront) CAtlTraceModule(hInst);
		pModule->m_bUsed = true;

		m_pProcess->IncModuleCount();
		pProcess->m_dwCurrFront += sizeof(CAtlTraceModule);
	}
	return i;
}

unsigned CAtlAllocator::AddCategory(unsigned nModule, unsigned nCategory, const WCHAR *szCategoryName)
{
	CAtlTraceModule *pModule = GetModule(nModule);
	CAtlTraceCategory *pCategory = NULL;
	UINT i = (UINT) ~0;
	if(pModule)
	{
		CAtlTraceProcess *pProcess = GetProcess();
		while(pProcess->m_dwBackAlloc - pProcess->m_dwCurrBack < sizeof(CAtlTraceCategory))
		{
			if(!VirtualAlloc(
				reinterpret_cast<BYTE *>(pProcess) + pProcess->MaxSize() - pProcess->m_dwBackAlloc - m_dwPageSize,
				m_dwPageSize, MEM_COMMIT, PAGE_READWRITE))
			{
				return NULL;
			}
			pProcess->m_dwBackAlloc += m_dwPageSize;
		}

		for(i = 0; i < m_pProcess->CategoryCount(); i++)
		{
			pCategory = GetCategoryByIndex(i);
			ATLASSERT(pCategory);
			if(!pCategory->m_bUsed)
			{
				pCategory->Reset(nCategory, szCategoryName);
				pCategory->m_nNext = i;
				pCategory->m_bUsed = true;
				break;
			}
		}
		if(i == m_pProcess->CategoryCount())
		{
			pCategory = new(reinterpret_cast<BYTE *>(pProcess) + pProcess->MaxSize() - 
				pProcess->m_dwCurrBack - sizeof(CAtlTraceCategory))
				CAtlTraceCategory(nCategory, szCategoryName);

			pCategory->m_nNext = m_pProcess->IncCategoryCount();
			pProcess->m_dwCurrBack += sizeof(CAtlTraceCategory);
		}

		// Fix up the rest of the categories
		if(IsValidCategoryIndex(pModule->m_nFirstCategory))
		{
			UINT nOldIndex, nIndex = pModule->m_nFirstCategory;
			CAtlTraceCategory *pCategory;
			do
			{
				pCategory = GetCategoryByIndex(nIndex);
				nOldIndex = nIndex;
				nIndex = pCategory->m_nNext;
			}
			while(nOldIndex != nIndex);
			pCategory->m_nNext = i;
		}
		else
			pModule->m_nFirstCategory = i;
	}
	return i;
}

bool CAtlAllocator::RemoveModule(UINT nIndex)
{
	CAtlTraceModule *pModule = GetModule(nIndex);
	if(pModule)
	{
		if(IsValidCategoryIndex(pModule->m_nFirstCategory))
		{
			UINT nOldIndex, nIndex = pModule->m_nFirstCategory;
			CAtlTraceCategory *pCategory;
			do
			{
				pCategory = GetCategoryByIndex(nIndex);
				pCategory->m_bUsed = false;
				nOldIndex = nIndex;
				nIndex = pCategory->m_nNext;
			}
			while(nOldIndex != nIndex);
		}
		pModule->m_bUsed = false;
		return true;
	}
	return false;
}

void CAtlAllocator::CleanUp()
{
	Close();
}
