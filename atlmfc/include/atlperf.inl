// This is a part of the Active Template Library.
// Copyright (C) 1996-2000 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLPERF_INL__
#define __ATLPERF_INL__

#pragma once

#ifndef __ATLPERF_H__
	#error atlperf.inl requires atlperf.h to be included first
#endif

namespace ATL
{

__declspec(selectany) LPCTSTR c_szAtlPerfCounter = _T("Counter");
__declspec(selectany) LPCTSTR c_szAtlPerfFirstCounter = _T("First Counter");
__declspec(selectany) LPCTSTR c_szAtlPerfLastCounter = _T("Last Counter");
__declspec(selectany) LPCTSTR c_szAtlPerfHelp = _T("Help");
__declspec(selectany) LPCTSTR c_szAtlPerfFirstHelp = _T("First Help");
__declspec(selectany) LPCTSTR c_szAtlPerfLastHelp = _T("Last Help");

__declspec(selectany) LPCWSTR c_szAtlPerfGlobal = L"Global";
__declspec(selectany) LPCTSTR c_szAtlPerfLibrary = _T("Library");
__declspec(selectany) LPCTSTR c_szAtlPerfOpen = _T("Open");
__declspec(selectany) LPCTSTR c_szAtlPerfCollect = _T("Collect");
__declspec(selectany) LPCTSTR c_szAtlPerfClose = _T("Close");
__declspec(selectany) LPCTSTR c_szAtlPerfLanguages = _T("Languages");
__declspec(selectany) LPCTSTR c_szAtlPerfMap = _T("Map");
__declspec(selectany) LPCTSTR c_szAtlPerfServicesKey = _T("SYSTEM\\CurrentControlSet\\Services");
__declspec(selectany) LPCTSTR c_szAtlPerfPerformanceKey = _T("SYSTEM\\CurrentControlSet\\Services\\%s\\Performance");
__declspec(selectany) LPCTSTR c_szAtlPerfPerfLibKey = _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Perflib");
__declspec(selectany) LPCTSTR c_szAtlPerfPerfLibLangKey = _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Perflib\\%3.3x");

#define ATLPERF_DWORD_ALIGN(n) ((n+3) & ~3)

inline CPerfMon::~CPerfMon()
{
	UnInitialize();
}

inline HRESULT CPerfMon::CreateMap(LANGID language, HINSTANCE hResInstance, UINT* pSampleRes)
{
	language; // unused
	hResInstance; // unused
	pSampleRes; // unused
	return S_OK;
}

inline CPerfMapEntry* CPerfMon::_GetMapEntry(int nIndex)
{
	if (nIndex < m_map.GetSize())
		return &m_map[nIndex];
	return NULL;
}

inline CPerfObject* CPerfMon::_GetFirstObject(CAtlFileMappingBase* pBlock)
{
	// should never happen if Initialize succeeded
	// are you checking return codes?
	ATLASSERT(pBlock->GetData() != NULL);

	return reinterpret_cast<CPerfObject*>(LPBYTE(pBlock->GetData()) + m_nHeaderSize);
}

inline CPerfObject* CPerfMon::_GetNextObject(CPerfObject* pInstance)
{
	return reinterpret_cast<CPerfObject*>(LPBYTE(pInstance) + pInstance->m_nAllocSize);
}

inline CAtlFileMappingBase* CPerfMon::_GetNextBlock(CAtlFileMappingBase* pBlock)
{
	// calling _GetNextBlock(NULL) will return the first block
	DWORD dwNextBlockIndex = 0;
	if (pBlock)
	{
		dwNextBlockIndex= *LPDWORD(LPBYTE(pBlock->GetData()) + m_nSchemaSize) +1;
		if (DWORD(m_aMem.GetSize()) == dwNextBlockIndex)
			return NULL;
	}
	return m_aMem[dwNextBlockIndex];
}

inline CAtlFileMappingBase* CPerfMon::_AllocNewBlock(CAtlFileMappingBase* pPrev, BOOL* pbExisted /* == NULL */)
{
	// initialize a security descriptor to give everyone access to objects we create
	CSecurityDescriptor sd;
	sd.InitializeFromThreadToken();
	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), sd, FALSE };

	CAutoPtr<CAtlFileMappingBase> pTempMem;
	ATLTRY(pTempMem.Attach(new CAtlFileMappingBase));
	if (pTempMem == NULL)
		return NULL;

	// create a unique name for the shared mem segment based on the index
	DWORD dwNextBlockIndex;
	if (pPrev != NULL)
		dwNextBlockIndex = *LPDWORD(LPBYTE(pPrev->GetData()) + m_nSchemaSize) +1;
	else
	{
		// use the system allocation granularity (65536 currently. may be different in the future)
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		m_nAllocSize = si.dwAllocationGranularity;

		dwNextBlockIndex = 0;
	}

	CString strName;
	strName.Format(_T("%s-%3.3d"), GetAppName(), dwNextBlockIndex);

	BOOL bExisted = FALSE;
	HRESULT hr = pTempMem->MapSharedMem(m_nAllocSize, CString(strName), &bExisted, &sa);
	if (FAILED(hr))
		return NULL;

	// save the index of this block
	// don't for first block since we don't know m_nSchemaSize yet
	if (dwNextBlockIndex)
		*LPDWORD(LPBYTE(pTempMem->GetData()) + m_nSchemaSize) = dwNextBlockIndex;

	if (!bExisted)
		memset(pTempMem->GetData(), 0, m_nAllocSize);

	if (pbExisted)
		*pbExisted = bExisted;

	CAtlFileMappingBase* p = pTempMem;
	if (!m_aMem.Add(p))
		return NULL;

	OnBlockAlloc(p);

	return pTempMem.Detach();
}

inline HRESULT CPerfMon::LoadMap()
{
	HRESULT hr;

	hr = ClearMap();
	if (FAILED(hr))
		return hr;

	DWORD* pData = LPDWORD(m_aMem[0]->GetData());

	*pData++; // skip blob size
	DWORD dwNumItems = *pData++; // number of items

	for (DWORD i=0; i<dwNumItems; i++)
	{
		DWORD dwIsObject = *pData++;
		DWORD dwPerfId = *pData++;
		DWORD dwDetailLevel = *pData++;

		if (dwIsObject)
		{
			DWORD dwDefaultCounter = *pData++;
			DWORD dwInstanceLess = *pData++;
			DWORD dwStructSize = *pData++;
			DWORD dwMaxInstanceNameLen = *pData++;

			hr = AddObjectDefinition(
				dwPerfId,
				NULL,
				NULL,
				dwDetailLevel,
				INT(dwDefaultCounter),
				BOOL(dwInstanceLess),
				UINT(dwStructSize),
				UINT(dwMaxInstanceNameLen));
			if (FAILED(hr))
			{
				ClearMap();
				return hr;
			}
		}
		else
		{
			DWORD dwCounterType = *pData++;
			DWORD dwMaxCounterSize = *pData++;
			DWORD dwDataOffset = *pData++;
			DWORD dwDefaultScale = *pData++;

			hr = AddCounterDefinition(
				dwPerfId,
				NULL,
				NULL,
				dwDetailLevel,
				dwCounterType,
				ULONG(dwMaxCounterSize),
				UINT(dwDataOffset),
				INT(dwDefaultScale));
			if (FAILED(hr))
			{
				ClearMap();
				return hr;
			}
		}

		DWORD dwNameId = *pData++;
		DWORD dwHelpId = *pData++;
		CPerfMapEntry& entry = m_map[m_map.GetSize()-1];
		entry.m_nNameId = (UINT) dwNameId;
		entry.m_nHelpId = (UINT) dwHelpId;
	}

	return S_OK;
}

inline HRESULT CPerfMon::SaveMap()
{
	int nDWORDs = 2 + 9*m_map.GetSize();
	CAutoVectorPtr<DWORD> blob;

	if (!blob.Allocate(nDWORDs))
		return E_OUTOFMEMORY;

	int nIndex = 0;
	blob[nIndex++] = nDWORDs * sizeof(DWORD); // blob size
	blob[nIndex++] = m_map.GetSize(); // number of items

	for (int i=0; i<m_map.GetSize(); i++)
	{
		CPerfMapEntry& entry = m_map[i];
		blob[nIndex++] = (DWORD) entry.m_bIsObject;
		blob[nIndex++] = (DWORD) entry.m_dwPerfId;
		blob[nIndex++] = (DWORD) entry.m_dwDetailLevel;
		if (entry.m_bIsObject)
		{
			blob[nIndex++] = (DWORD) entry.m_nDefaultCounter;
			blob[nIndex++] = (DWORD) entry.m_nInstanceLess;
			blob[nIndex++] = (DWORD) entry.m_nStructSize;
			blob[nIndex++] = (DWORD) entry.m_nMaxInstanceNameLen;
		}
		else
		{
			blob[nIndex++] = (DWORD) entry.m_dwCounterType;
			blob[nIndex++] = (DWORD) entry.m_nMaxCounterSize;
			blob[nIndex++] = (DWORD) entry.m_nDataOffset;
			blob[nIndex++] = (DWORD) entry.m_nDefaultScale;
		}
		blob[nIndex++] = (DWORD) entry.m_nNameId;
		blob[nIndex++] = (DWORD) entry.m_nHelpId;
	}

	CRegKey rkApp;
	CString str;
	DWORD dwErr;

	str.Format(c_szAtlPerfPerformanceKey, GetAppName());
	dwErr = rkApp.Open(HKEY_LOCAL_MACHINE, str);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	rkApp.SetBinaryValue(c_szAtlPerfMap, LPDWORD(blob), blob[0]);

	return S_OK;
}

inline CPerfMapEntry* CPerfMon::_FindObjectInfo(DWORD dwObjectId)
{
	for (int i=0; i<m_map.GetSize(); i += m_map[i].m_nNumCounters+1)
	{
		CPerfMapEntry* pObjectEntry = &m_map[i];
		if (pObjectEntry->m_dwPerfId == dwObjectId)
			return pObjectEntry;
	}

	return NULL;
}

inline CPerfMapEntry* CPerfMon::_FindCounterInfo(CPerfMapEntry* pObjectEntry, DWORD dwCounterId)
{
	ATLASSERT(pObjectEntry != NULL);

	for (DWORD i=0; i<pObjectEntry->m_nNumCounters; i++)
	{
		CPerfMapEntry* pCounter = pObjectEntry+i+1;
		if (pCounter->m_dwPerfId == dwCounterId)
			return pCounter;
	}

	return NULL;
}

inline CPerfMapEntry* CPerfMon::_FindCounterInfo(DWORD dwObjectId, DWORD dwCounterId)
{
	CPerfMapEntry* pObjectEntry = _FindObjectInfo(dwObjectId);
	if (pObjectEntry != NULL)
		return _FindCounterInfo(pObjectEntry, dwCounterId);

	return NULL;
}

inline BOOL CPerfMon::_WantObjectType(LPWSTR lpwszValue, DWORD dwPerfId)
{
	if (lstrcmpiW(c_szAtlPerfGlobal, lpwszValue) == 0)
		return TRUE;

	CString strList(lpwszValue);
	int nStart = 0;

	while (1)
	{
		CString strNum = strList.Tokenize(_T(" "), nStart);
		if (strNum.IsEmpty())
			return FALSE;

		if (_ttoi(strNum) == int(dwPerfId))
			return TRUE;
	}
}

inline LPBYTE CPerfMon::_AllocData(LPBYTE& pData, ULONG nBytesAvail, ULONG* pnBytesUsed, ULONG nBytesNeeded)
{
	if (nBytesAvail < *pnBytesUsed + nBytesNeeded)
		return NULL;

	LPBYTE p = pData;
	pData += nBytesNeeded;
	*pnBytesUsed += nBytesNeeded;

	return p;
}

inline void CPerfMon::_FillObjectType(PERF_OBJECT_TYPE* pObjectType, CPerfMapEntry* pObjectEntry)
{
    pObjectType->DefinitionLength = sizeof(PERF_OBJECT_TYPE) + sizeof(PERF_COUNTER_DEFINITION) * pObjectEntry->m_nNumCounters;
    pObjectType->TotalByteLength = pObjectType->DefinitionLength; // we will add the instance definitions/counter blocks as we go
    pObjectType->HeaderLength = sizeof(PERF_OBJECT_TYPE);
    pObjectType->ObjectNameTitleIndex = pObjectEntry->m_nNameId;
    pObjectType->ObjectNameTitle = NULL;
    pObjectType->ObjectHelpTitleIndex = pObjectEntry->m_nHelpId;
    pObjectType->ObjectHelpTitle = NULL;
    pObjectType->DetailLevel = pObjectEntry->m_dwDetailLevel;
    pObjectType->NumCounters = pObjectEntry->m_nNumCounters;
    pObjectType->DefaultCounter = pObjectEntry->m_nDefaultCounter;
	if (pObjectEntry->m_nInstanceLess == PERF_NO_INSTANCES)
		pObjectType->NumInstances = PERF_NO_INSTANCES;
	else
		pObjectType->NumInstances = 0; // this will be calculated as we go
    pObjectType->CodePage = 0;
    pObjectType->PerfTime.QuadPart = 0;
    pObjectType->PerfFreq.QuadPart = 0;
}

inline void CPerfMon::_FillCounterDef(
	PERF_COUNTER_DEFINITION* pCounterDef,
	CPerfMapEntry* pCounterEntry,
	ULONG& nCBSize
	)
{
	pCounterDef->ByteLength = sizeof(PERF_COUNTER_DEFINITION);
	pCounterDef->CounterNameTitleIndex = pCounterEntry->m_nNameId;
	pCounterDef->CounterNameTitle = NULL;
	pCounterDef->CounterHelpTitleIndex = pCounterEntry->m_nHelpId;
	pCounterDef->CounterHelpTitle = NULL;
	pCounterDef->DefaultScale = pCounterEntry->m_nDefaultScale;
	pCounterDef->DetailLevel = pCounterEntry->m_dwDetailLevel;
	pCounterDef->CounterType = pCounterEntry->m_dwCounterType;
	switch (pCounterEntry->m_dwCounterType & 0x00000300)
	{
	case PERF_SIZE_DWORD:
		pCounterDef->CounterSize = sizeof(DWORD);
		break;
	case PERF_SIZE_LARGE:
		pCounterDef->CounterSize = sizeof(__int64);
		break;
	case PERF_SIZE_ZERO:
		pCounterDef->CounterSize = 0;
		break;
	case PERF_SIZE_VARIABLE_LEN:
		ATLASSERT((pCounterEntry->m_dwCounterType & 0x00000C00) == PERF_TYPE_TEXT);
		if ((pCounterEntry->m_dwCounterType & 0x00010000) == PERF_TEXT_UNICODE)
			pCounterDef->CounterSize = ATLPERF_DWORD_ALIGN(pCounterEntry->m_nMaxCounterSize * sizeof(WCHAR));
		else
			pCounterDef->CounterSize = ATLPERF_DWORD_ALIGN(pCounterEntry->m_nMaxCounterSize * sizeof(char));
		break;
	}
	pCounterDef->CounterOffset = sizeof(PERF_COUNTER_BLOCK) + nCBSize;
	nCBSize += pCounterDef->CounterSize;
}

inline HRESULT CPerfMon::_CollectObjectType(
	CPerfMapEntry* pObjectEntry,
	LPBYTE pData,
	ULONG nBytesAvail,
	ULONG* pnBytesUsed
	)
{
	ATLASSERT(m_aMem.GetSize() != 0);

	*pnBytesUsed = 0;

	// write the object definition out
	PERF_OBJECT_TYPE* pObjectType = _AllocStruct(pData, nBytesAvail, pnBytesUsed, (PERF_OBJECT_TYPE*) NULL);
	if (pObjectType == NULL)
		return E_OUTOFMEMORY;

	_FillObjectType(pObjectType, pObjectEntry);

	// save a pointer to the first counter entry and counter definition.
	// we'll need them when we create the PERF_COUNTER_BLOCK data
	CPerfMapEntry* pCounterEntries = pObjectEntry + 1;
	PERF_COUNTER_DEFINITION* pCounterDefs = reinterpret_cast<PERF_COUNTER_DEFINITION*>(pData);
	ULONG nCBSize = 0; // counter block size

	// write the counter definitions out
	CPerfMapEntry* pCounterEntry = pObjectEntry;
	for (DWORD i=0; i<pObjectEntry->m_nNumCounters; i++)
	{
		pCounterEntry++;

		PERF_COUNTER_DEFINITION* pCounterDef = _AllocStruct(pData, nBytesAvail, pnBytesUsed, (PERF_COUNTER_DEFINITION*) NULL);
		if (pCounterDef == NULL)
			return E_OUTOFMEMORY;

		_FillCounterDef(pCounterDef, pCounterEntry, nCBSize);
	}

	// search for objects of the appropriate type and write out their instance/counter data
	CAtlFileMappingBase* pCurrentBlock = m_aMem[0];
	CPerfObject* pInstance = _GetFirstObject(pCurrentBlock);
	while (pInstance && pInstance->m_nAllocSize != 0)
	{
		if (pInstance->m_dwObjectId == pObjectEntry->m_dwPerfId)
		{
			PERF_INSTANCE_DEFINITION* pInstanceDef = NULL;

			if (pObjectEntry->m_nInstanceLess == PERF_NO_INSTANCES)
				pObjectType->NumInstances = PERF_NO_INSTANCES;
			else
			{
				pObjectType->NumInstances++;

				// create an instance definition
				pInstanceDef = _AllocStruct(pData, nBytesAvail, pnBytesUsed, (PERF_INSTANCE_DEFINITION*) NULL);
				if (pInstanceDef == NULL)
					return E_OUTOFMEMORY;

				pInstanceDef->ParentObjectTitleIndex = 0;
				pInstanceDef->ParentObjectInstance = 0;
				pInstanceDef->UniqueID = (DWORD) PERF_NO_UNIQUE_ID;

				// handle the instance name
				LPCWSTR szInstNameSrc = LPCWSTR(LPBYTE(pInstance)+pInstance->m_nInstanceNameOffset);
				pInstanceDef->NameLength = (ULONG)(wcslen(szInstNameSrc)+1)*sizeof(WCHAR);
				LPWSTR szInstNameDest = (LPWSTR) _AllocData(pData, nBytesAvail, pnBytesUsed, ATLPERF_DWORD_ALIGN(pInstanceDef->NameLength));
				if (szInstNameDest == NULL)
					return E_OUTOFMEMORY;

				memcpy(szInstNameDest, szInstNameSrc, pInstanceDef->NameLength);
				pInstanceDef->NameOffset = ULONG(LPBYTE(szInstNameDest) - LPBYTE(pInstanceDef));

				pInstanceDef->ByteLength = sizeof(PERF_INSTANCE_DEFINITION) + ATLPERF_DWORD_ALIGN(pInstanceDef->NameLength);
			}

			// create the counter block
			PERF_COUNTER_BLOCK* pCounterBlock = _AllocStruct(pData, nBytesAvail, pnBytesUsed, (PERF_COUNTER_BLOCK*) NULL);
			if (pCounterBlock == NULL)
				return E_OUTOFMEMORY;

			pCounterBlock->ByteLength = sizeof(PERF_COUNTER_BLOCK) + nCBSize;

			LPBYTE pCounterData = _AllocData(pData, nBytesAvail, pnBytesUsed, nCBSize);
			if (pCounterData == NULL)
				return E_OUTOFMEMORY;

			for (ULONG i=0; i<pObjectType->NumCounters; i++)
			{
				switch (pCounterEntry->m_dwCounterType & 0x00000300)
				{
				case PERF_SIZE_DWORD:
					*LPDWORD(pCounterData+pCounterDefs[i].CounterOffset-sizeof(PERF_COUNTER_BLOCK)) =
						*LPDWORD(LPBYTE(pInstance)+pCounterEntries[i].m_nDataOffset);
					break;
				case PERF_SIZE_LARGE:
					*PULONGLONG(pCounterData+pCounterDefs[i].CounterOffset-sizeof(PERF_COUNTER_BLOCK)) =
						*PULONGLONG(LPBYTE(pInstance)+pCounterEntries[i].m_nDataOffset);
					break;
				case PERF_SIZE_VARIABLE_LEN:
					{
						LPDWORD pOffset = LPDWORD(LPBYTE(pInstance)+pObjectEntry->m_nDataOffset);
						LPBYTE pSrc = LPBYTE(pInstance)+*pOffset;
						LPBYTE pDest = pCounterData+pCounterDefs[i].CounterOffset-sizeof(PERF_COUNTER_BLOCK);
						if ((pCounterEntry->m_dwCounterType & 0x00010000) == PERF_TEXT_UNICODE)
						{
							ULONG nLen = (ULONG)wcslen(LPCWSTR(pSrc));
							nLen = min(nLen, pCounterEntries[i].m_nMaxCounterSize-1);
							wcsncpy(LPWSTR(pDest), LPCWSTR(pSrc), nLen);
							((LPWSTR) pDest)[nLen] = 0;
						}
						else
						{
							ULONG nLen = (ULONG)strlen(LPCSTR(pSrc));
							nLen = min(nLen, pCounterEntries[i].m_nMaxCounterSize-1);
							strncpy(LPSTR(pDest), LPCSTR(pSrc), nLen);
							((LPSTR) pDest)[nLen] = 0;
						}
					}
					break;
				}
			}

			if (pInstanceDef != NULL)
				pObjectType->TotalByteLength += pInstanceDef->ByteLength;
			pObjectType->TotalByteLength += sizeof(PERF_COUNTER_BLOCK) + nCBSize;
		}

		pInstance = _GetNextObject(pInstance);
		if (pInstance->m_nAllocSize == (ULONG) -1)
		{
			pCurrentBlock = _GetNextBlock(pCurrentBlock);
			if (pCurrentBlock == NULL)
				pInstance = NULL;
			else
				pInstance = _GetFirstObject(pCurrentBlock);
		}
	}

	return S_OK;
}

inline DWORD CPerfMon::Open(LPWSTR lpDeviceNames)
{
	lpDeviceNames; // unused
	return Initialize();
}

inline DWORD CPerfMon::Collect(
	LPWSTR lpwszValue,
	LPVOID* lppData,
	LPDWORD lpcbBytes,
	LPDWORD lpcObjectTypes
	)
{
	if (m_aMem.GetSize() == 0 || m_aMem[0]->GetData() == NULL || m_lock.m_h == NULL)
	{
		*lpcbBytes = 0;
		*lpcObjectTypes = 0;
		return ERROR_SUCCESS;
	}

	// get a lock so that other threads don't corrupt the data we're collecting
	CPerfLock lock(this);
	if (FAILED(lock.GetStatus()))
	{
		*lpcbBytes = 0;
		*lpcObjectTypes = 0;
		return ERROR_SUCCESS;
	}

	LPBYTE pData = LPBYTE(*lppData);
	ULONG nBytesLeft = *lpcbBytes;
	ULONG nBytesUsed;
	*lpcbBytes = 0;

	for (int i=0; i<m_map.GetSize(); i += m_map[i].m_nNumCounters+1)
	{
		CPerfMapEntry* pObjectEntry = &m_map[i];
		if (_WantObjectType(lpwszValue, pObjectEntry->m_nNameId))
		{
			if (FAILED(_CollectObjectType(pObjectEntry, pData, nBytesLeft, &nBytesUsed)))
			{
				*lpcbBytes = 0;
				*lpcObjectTypes = 0;
				return ERROR_SUCCESS;
			}
			else
			{
				(*lpcObjectTypes)++;
				(*lpcbBytes) += nBytesUsed;
				nBytesLeft -= nBytesUsed;
				pData += nBytesUsed;
			}
		}
	}

	*lppData = pData;
	return ERROR_SUCCESS;
}

inline DWORD CPerfMon::Close()
{
	return UnInitialize();
}

#ifdef _ATL_PERF_REGISTER
inline HRESULT CPerfMon::_AppendRegStrings(
	CRegKey& rkLang,
	LPCTSTR szValue,
	CSimpleArray<CString>& astrStrings,
	ULONG nNewStringSize,
	ULONG iNextIndex
	)
{
	// load the existing strings, append the new data, and resave the strings
	ULONG nChars = 0;
	DWORD dwErr;
	
	dwErr = rkLang.QueryMultiStringValue(szValue, NULL, &nChars);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	nChars += nNewStringSize;

	CString str;
	LPTSTR pszBuf = str.GetBuffer(nChars);
	dwErr = rkLang.QueryMultiStringValue(szValue, pszBuf, &nChars);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	LPTSTR pszTemp = pszBuf+nChars-1;

	for (int iString = 0; iString < astrStrings.GetSize(); iString++)
	{
		INT nFormatChars = _stprintf(pszTemp, _T("%d"), iNextIndex);
		iNextIndex += 2;
		pszTemp += nFormatChars + 1;
		_tcscpy(pszTemp, astrStrings[iString]);
		pszTemp += astrStrings[iString].GetLength() + 1;
	}

	*pszTemp++ = '\0'; // must have 2 null terminators at end of multi_sz

	dwErr = rkLang.SetMultiStringValue(szValue, pszBuf);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	return S_OK;
}

inline HRESULT CPerfMon::_RemoveRegStrings(
	CRegKey& rkLang,
	LPCTSTR szValue,
	ULONG iFirstIndex,
	ULONG iLastIndex
	)
{
	// load the existing strings, remove the data, and resave the strings
	DWORD nChars = 0;
	DWORD dwErr;
	
	dwErr = rkLang.QueryMultiStringValue(szValue, NULL, &nChars);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	CString str;
	LPTSTR pszBuf = str.GetBuffer(nChars);
	dwErr = rkLang.QueryMultiStringValue(szValue, pszBuf, &nChars);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	LPCTSTR pszRead = pszBuf;
	LPTSTR pszWrite = pszBuf;
	while (*pszRead != '\0')
	{
		ULONG iIndex = _ttoi(pszRead);
		int nLen = (int) _tcslen(pszRead) + 1; // get the length of the index and null
		nLen += (int) _tcslen(pszRead+nLen) + 1; // add the length of the description and null
		if (iIndex < iFirstIndex || iIndex > iLastIndex)
		{
			memmove(pszWrite, pszRead, nLen*sizeof(TCHAR));
			pszWrite += nLen;
		}
		pszRead += nLen;
	}
	*pszWrite++ = '\0'; // must have 2 null terminators at end of multi_sz

	dwErr = rkLang.SetMultiStringValue(szValue, pszBuf);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	return S_OK;
}

inline HRESULT CPerfMon::Register(
	LPCTSTR szOpenFunc,
	LPCTSTR szCollectFunc,
	LPCTSTR szCloseFunc,
	HINSTANCE hDllInstance /* == _pModule->GetModuleInstance() */
	)
{
	CString str;
	DWORD dwErr;
	HRESULT hr;

	hr = CreateMap(LANGIDFROMLCID(GetThreadLocale()), NULL);
	if (FAILED(hr))
		return hr;

	CString strAppKey;
	strAppKey.Format(c_szAtlPerfPerformanceKey, GetAppName());

	// if we're already registered, unregister so we can redo registration
	Unregister();

	// register the PerfMon counter and help strings
	CRegKey rkPerfLib;

	dwErr = rkPerfLib.Open(HKEY_LOCAL_MACHINE, c_szAtlPerfPerfLibKey);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	// figure out the counter range
	DWORD dwLastCounter;
	DWORD dwLastHelp;

	dwErr = rkPerfLib.QueryDWORDValue(c_szAtlPerfLastCounter, dwLastCounter);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	dwErr = rkPerfLib.QueryDWORDValue(c_szAtlPerfLastHelp, dwLastHelp);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	DWORD dwFirstCounter = dwLastCounter + 2;
	DWORD dwFirstHelp = dwLastHelp + 2;
	dwLastCounter += 2*m_map.GetSize();
	dwLastHelp += 2*m_map.GetSize();

	dwErr = rkPerfLib.SetDWORDValue(c_szAtlPerfLastCounter, dwLastCounter);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	dwErr = rkPerfLib.SetDWORDValue(c_szAtlPerfLastHelp, dwLastHelp);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	for (int i=0; i<m_map.GetSize(); i++)
	{
		CPerfMapEntry& entry = m_map[i];

		entry.m_nNameId = dwFirstCounter + i*2;
		entry.m_nHelpId = dwFirstHelp + i*2;
	}

	// register the app entry points
	CRegKey rkApp;

	dwErr = rkApp.Create(HKEY_LOCAL_MACHINE, strAppKey);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	if (GetModuleFileName(hDllInstance, str.GetBuffer(MAX_PATH), MAX_PATH) == 0)
		return AtlHresultFromLastError();
	str.ReleaseBuffer();

	dwErr = rkApp.SetStringValue(c_szAtlPerfLibrary, str);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	dwErr = rkApp.SetStringValue(c_szAtlPerfOpen, szOpenFunc);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	dwErr = rkApp.SetStringValue(c_szAtlPerfCollect, szCollectFunc);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	dwErr = rkApp.SetStringValue(c_szAtlPerfClose, szCloseFunc);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	// register the used counter range
	dwErr = rkApp.SetDWORDValue(c_szAtlPerfFirstCounter, dwFirstCounter);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	dwErr = rkApp.SetDWORDValue(c_szAtlPerfLastCounter, dwLastCounter);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	dwErr = rkApp.SetDWORDValue(c_szAtlPerfFirstHelp, dwFirstHelp);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	dwErr = rkApp.SetDWORDValue(c_szAtlPerfLastHelp, dwLastHelp);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	dwErr = rkApp.SetStringValue(c_szAtlPerfLanguages, _T(""));
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	hr = SaveMap();
	if (FAILED(hr))
		return hr;

	return S_OK;
}

inline HRESULT CPerfMon::RegisterStrings(
	LANGID language /* = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL) */,
	HINSTANCE hResInstance /* = _pModule->GetResourceInstance() */
	)
{
	CString str;
	DWORD dwErr;
	HRESULT hr;
	CRegKey rkLang;
	CRegKey rkApp;

	LANGID wPrimaryLanguage = (LANGID) PRIMARYLANGID(language);

	if (language == MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL))
	{
		language = LANGIDFROMLCID(GetThreadLocale());
		wPrimaryLanguage = (LANGID) PRIMARYLANGID(language);
	}

	hr = CreateMap(language, hResInstance);
	if (FAILED(hr))
		return hr;

	str.Format(c_szAtlPerfPerfLibLangKey, wPrimaryLanguage);
	dwErr = rkLang.Open(HKEY_LOCAL_MACHINE, str);
	if (dwErr == ERROR_FILE_NOT_FOUND)
		return S_FALSE; // the language isn't installed on the system
	else if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	// load list of language strings already registered
	str.Format(c_szAtlPerfPerformanceKey, GetAppName());
	dwErr = rkApp.Create(HKEY_LOCAL_MACHINE, str);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	DWORD dwLangsLen = 0;
	CString strLangs;

	dwErr = rkApp.QueryStringValue(c_szAtlPerfLanguages, NULL, &dwLangsLen);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	LPTSTR pszLangs = strLangs.GetBuffer(dwLangsLen+4); // reserve room for adding new language
	dwErr = rkApp.QueryStringValue(c_szAtlPerfLanguages, pszLangs, &dwLangsLen);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);
	dwLangsLen--; // don't count '\0'

	// see if this language has already been registered and if so, return
	TCHAR szNewLang[5];
	_stprintf(szNewLang, _T("%3.3x "), wPrimaryLanguage);
	if (strLangs.Find(szNewLang) != -1)
		return S_OK;

	// load the strings we want to append and figure out how much extra space is needed for them
	// (including up to 5-digit index values and 2 null separators)
	CSimpleArray<CString> astrCounters;
	CSimpleArray<CString> astrHelp;
	ULONG nNewCounterSize = 0;
	ULONG nNewHelpSize = 0;

	for (int i=0; i<m_map.GetSize(); i++)
	{
		CPerfMapEntry& object = m_map[i];

		astrCounters.Add(object.m_szName);
		nNewCounterSize += lstrlen(object.m_szName) + 7;

		astrHelp.Add(object.m_szHelp);
		nNewHelpSize += lstrlen(object.m_szHelp) + 7;
	}

	DWORD dwNextCounter;
	DWORD dwNextHelp;

	dwErr = rkApp.QueryDWORDValue(c_szAtlPerfFirstCounter, dwNextCounter);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	dwErr = rkApp.QueryDWORDValue(c_szAtlPerfFirstHelp, dwNextHelp);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	hr = _AppendRegStrings(rkLang, c_szAtlPerfCounter, astrCounters, nNewCounterSize, dwNextCounter);
	if (FAILED(hr))
		return hr;

	hr = _AppendRegStrings(rkLang, c_szAtlPerfHelp, astrHelp, nNewHelpSize, dwNextHelp);
	if (FAILED(hr))
		return hr;

	// add the language to the list of installed languages
	_tcscpy(pszLangs+dwLangsLen, szNewLang);
	strLangs.ReleaseBuffer(dwLangsLen+4);

	dwErr = rkApp.SetStringValue(c_szAtlPerfLanguages, strLangs);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	return S_OK;
}

inline BOOL CPerfMon::EnumResLangProc(
	HINSTANCE hModule,
	LPCTSTR lpszType,
	LPCTSTR lpszName,
	LANGID wIDLanguage,
	LPARAM lParam
	)
{
	hModule; // unused
	lpszType; // unused
	lpszName; // unused

	CSimpleArray<LANGID>* pLangs = reinterpret_cast<CSimpleArray<LANGID>*>(lParam);
	pLangs->Add(wIDLanguage);
	return TRUE;
}

inline HRESULT CPerfMon::RegisterAllStrings(
	HINSTANCE hResInstance /* = _pModule->GetResourceInstance() */
	)
{
	HRESULT hrReturn = S_FALSE;
	HRESULT hr;

	UINT nRes;
	hr = CreateMap(0, NULL, &nRes);
	if (FAILED(hr))
		return hr;

	if (nRes == 0)
	{
		hr = RegisterStrings(0, hResInstance);
		if (FAILED(hr))
			return hr;
		if (hr == S_OK)
			hrReturn = S_OK;
	}
	else
	{
		CSimpleArray<LANGID> langs;
		if (!EnumResourceLanguages(hResInstance, RT_STRING, MAKEINTRESOURCE((nRes>>4)+1), EnumResLangProc, reinterpret_cast<LPARAM>(&langs)))
			return AtlHresultFromLastError();

		for (int i=0; i<langs.GetSize(); i++)
		{
			hr = RegisterStrings(langs[i], hResInstance);
			if (FAILED(hr))
				return hr;
			if (hr == S_OK)
				hrReturn = S_OK;
		}
	}

	return hrReturn;
}

inline HRESULT CPerfMon::Unregister()
{
	CString str;
	HRESULT hr;
	DWORD dwErr;

	// register the PerfMon counter and help strings
	CRegKey rkPerfLib;
	CRegKey rkLang;
	CRegKey rkApp;

	dwErr = rkPerfLib.Open(HKEY_LOCAL_MACHINE, c_szAtlPerfPerfLibKey);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	str.Format(c_szAtlPerfPerformanceKey, GetAppName());

	dwErr = rkApp.Open(HKEY_LOCAL_MACHINE, str);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	DWORD dwLastCounter;
	DWORD dwLastHelp;
	DWORD dwFirstAppCounter;
	DWORD dwFirstAppHelp;
	DWORD dwLastAppCounter;
	DWORD dwLastAppHelp;

	dwErr = rkPerfLib.QueryDWORDValue(c_szAtlPerfLastCounter, dwLastCounter);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	dwErr = rkPerfLib.QueryDWORDValue(c_szAtlPerfLastHelp, dwLastHelp);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	dwErr = rkApp.QueryDWORDValue(c_szAtlPerfFirstCounter, dwFirstAppCounter);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	dwErr = rkApp.QueryDWORDValue(c_szAtlPerfFirstHelp, dwFirstAppHelp);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	dwErr = rkApp.QueryDWORDValue(c_szAtlPerfLastCounter, dwLastAppCounter);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	dwErr = rkApp.QueryDWORDValue(c_szAtlPerfLastHelp, dwLastAppHelp);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	// iterate through the installed languages and delete them all
	DWORD nChars = 0;
	dwErr = rkApp.QueryStringValue(c_szAtlPerfLanguages, NULL, &nChars);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	CString strLangs;
	dwErr = rkApp.QueryStringValue(c_szAtlPerfLanguages, strLangs.GetBuffer(nChars), &nChars);
	strLangs.ReleaseBuffer();
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	int nIndex = 0;
	CString strLang = strLangs.Tokenize(_T(" "), nIndex);
	while (!strLang.IsEmpty())
	{
		dwErr = rkLang.Open(HKEY_LOCAL_MACHINE, CString(c_szAtlPerfPerfLibKey) + _T("\\") + strLang);
		if (dwErr != ERROR_SUCCESS)
			return AtlHresultFromWin32(dwErr);

		hr = _RemoveRegStrings(rkLang, c_szAtlPerfCounter, dwFirstAppCounter, dwLastAppCounter);
		if (FAILED(hr))
			return hr;

		hr = _RemoveRegStrings(rkLang, c_szAtlPerfHelp, dwFirstAppHelp, dwLastAppHelp);
		if (FAILED(hr))
			return hr;

		strLang = strLangs.Tokenize(_T(" "), nIndex);
	}

	// rewind the Last Help/Last Counter values if possible
	if (dwLastCounter == dwLastAppCounter)
	{
		dwErr = rkPerfLib.SetDWORDValue(c_szAtlPerfLastCounter, dwFirstAppCounter-2);
		if (dwErr != ERROR_SUCCESS)
			return AtlHresultFromWin32(dwErr);
	}

	if (dwLastHelp == dwLastAppHelp)
	{
		dwErr = rkPerfLib.SetDWORDValue(c_szAtlPerfLastHelp, dwFirstAppHelp-2);
		if (dwErr != ERROR_SUCCESS)
			return AtlHresultFromWin32(dwErr);
	}

	// delete the app key
	CRegKey rkServices;

	rkApp.Close();
	dwErr = rkServices.Open(HKEY_LOCAL_MACHINE, c_szAtlPerfServicesKey);
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	dwErr = rkServices.RecurseDeleteKey(GetAppName());
	if (dwErr != ERROR_SUCCESS)
		return AtlHresultFromWin32(dwErr);

	return S_OK;
}
#endif

inline HRESULT CPerfMon::Initialize()
{
	CMutex tempLock;
	CAutoPtr<CAtlFileMappingBase> pTempMem;
	CString strAppName(GetAppName());
	HRESULT hr;

	ATLASSERT(m_aMem.GetSize() == 0);

	// initialize a security descriptor to give everyone access to objects we create
	CSecurityDescriptor sd;
	sd.InitializeFromThreadToken();
	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), sd, FALSE };

	// create a mutex to handle syncronizing access to the shared memory area
	tempLock.Create(&sa, FALSE, strAppName + _T("Lock"));
	if (tempLock.m_h == NULL)
		return AtlHresultFromLastError();

	// create a shared memory area to share data between the app being measured and the client doing the measuring
	{
		CMutexLock lock(tempLock);

		BOOL bExisted = FALSE;

		pTempMem.Attach(_AllocNewBlock(NULL, &bExisted));
		if (!pTempMem)
			return E_OUTOFMEMORY;

		if (!bExisted)
		{
			// copy the map from the registry to the shared memory
			CRegKey rkApp;
			DWORD dwErr;
			CString strAppKey;

			strAppKey.Format(c_szAtlPerfPerformanceKey, GetAppName());
			dwErr = rkApp.Open(HKEY_LOCAL_MACHINE, strAppKey, KEY_READ);
			if (dwErr != ERROR_SUCCESS)
			{
				m_aMem.RemoveAll();
				return AtlHresultFromWin32(dwErr);
			}

			ULONG nBytes = m_nAllocSize;
			dwErr = rkApp.QueryBinaryValue(c_szAtlPerfMap, pTempMem->GetData(), &nBytes);
			if (dwErr != ERROR_SUCCESS)
			{
				m_aMem.RemoveAll();
				return AtlHresultFromWin32(dwErr);
			}
		}

		hr = LoadMap();
		if (FAILED(hr))
		{
			m_aMem.RemoveAll();
			return hr;
		}

		m_nSchemaSize = *LPDWORD(pTempMem->GetData());
		m_nHeaderSize = m_nSchemaSize + sizeof(DWORD);
	}

	// now that everything has succeeded, move things from temps to members
	pTempMem.Detach();
	m_lock.Attach(tempLock.Detach());

	return S_OK;
}

inline HRESULT CPerfMon::UnInitialize()
{
	HRESULT hr = S_OK;

	if (m_lock.m_h != NULL)
		m_lock.Close();
	for (int i=0; i<m_aMem.GetSize(); i++)
	{
		CAtlFileMappingBase* p = m_aMem[i];
		HRESULT _hr = p->Unmap();
		if (FAILED(_hr))
			hr = _hr;

		delete p;
	}
	m_aMem.RemoveAll();
	ClearMap();

	return hr;
}

inline HRESULT CPerfMon::CreateInstance(
	DWORD dwObjectType,
	DWORD dwInstance,
	LPWSTR szInstanceName,
	CPerfObject** ppInstance
	)
{
	CPerfObject* pEmptyBlock = NULL;

	if (ppInstance == NULL)
		return E_POINTER;
	else if (m_aMem.GetSize() == 0 || m_aMem[0]->GetData() == NULL || m_lock.m_h == NULL)
		return E_UNEXPECTED; // Initialize must succeed before calling CreateInstance

	*ppInstance = NULL;

	CPerfMapEntry* pObjectEntry = _FindObjectInfo(dwObjectType);
	if (pObjectEntry == NULL)
		return E_INVALIDARG;
	else if (pObjectEntry->m_nInstanceLess == PERF_NO_INSTANCES &&
			(dwInstance != 0 || szInstanceName != NULL))
		return E_INVALIDARG;

	CPerfLock lock(this);
	if (FAILED(lock.GetStatus()))
		return lock.GetStatus();

	CAtlFileMappingBase* pCurrentBlock = m_aMem[0];
	CPerfObject* pInstance = _GetFirstObject(pCurrentBlock);
	ULONG nMaxInstance = 0;
	ULONG nUsedSpace = 0;
	while (pInstance->m_nAllocSize != 0)
	{
		nUsedSpace += pInstance->m_nAllocSize;

		if (pInstance->m_dwObjectId == dwObjectType)
		{
			nMaxInstance = max(nMaxInstance, pInstance->m_dwInstance);
			if (pInstance->m_dwInstance == dwInstance &&
				(pObjectEntry->m_nInstanceLess == PERF_NO_INSTANCES || dwInstance != 0))
			{
				*ppInstance = pInstance;
				pInstance->m_nRefCount++;
				return S_OK;
			}
		}

		if (pInstance->m_nAllocSize == pObjectEntry->m_nAllocSize && pInstance->m_dwObjectId == 0)
			pEmptyBlock = pInstance;

		pInstance = _GetNextObject(pInstance);

		if (pInstance->m_nAllocSize == 0 &&
			m_nHeaderSize + nUsedSpace + pObjectEntry->m_nAllocSize + sizeof(CPerfObject) > m_nAllocSize)
		{
			// we've reached the end of the block and have no room to allocate an object of this
			// type. cap the block with a sentinel
			pInstance->m_nAllocSize = (ULONG) -1;
		}

		// check for an end-of-shared-mem sentinel
		if (pInstance->m_nAllocSize == (ULONG) -1)
		{
			nUsedSpace = 0;
			CAtlFileMappingBase* pNextBlock = _GetNextBlock(pCurrentBlock);
			if (pNextBlock == NULL)
			{
				// we've reached the last block of shared mem.
				// the instance hasn't been found, so either use a
				// previously freed instance block (pEmptyBlock) or allocate a new
				// shared mem block to hold the new instance
				if (pEmptyBlock == NULL)
				{
					pNextBlock = _AllocNewBlock(pCurrentBlock);
					if (pNextBlock == NULL)
						return E_OUTOFMEMORY;
				}
				else
					break;
			}
			pCurrentBlock = pNextBlock;
			pInstance = _GetFirstObject(pCurrentBlock);
		}
	}

	// allocate a new object
	if (pEmptyBlock != NULL)
		pInstance = pEmptyBlock;
	else
		pInstance->m_nAllocSize = pObjectEntry->m_nAllocSize;

	pInstance->m_dwObjectId = pObjectEntry->m_dwPerfId;
	if (dwInstance == 0 && pObjectEntry->m_nInstanceLess != PERF_NO_INSTANCES)
		pInstance->m_dwInstance = nMaxInstance + 1;
	else
		pInstance->m_dwInstance = dwInstance;

	pInstance->m_nRefCount = 1;

	// copy the instance name, truncate if necessary
	if (pObjectEntry->m_nInstanceLess != PERF_NO_INSTANCES)
	{
		ULONG nNameLen = (ULONG)min(wcslen(szInstanceName), pObjectEntry->m_nMaxInstanceNameLen-1);
		ULONG nNameBytes = (nNameLen+1) * sizeof(WCHAR);
		pInstance->m_nInstanceNameOffset = pInstance->m_nAllocSize-nNameBytes;
		CopyMemory(LPBYTE(pInstance)+pInstance->m_nInstanceNameOffset, szInstanceName, nNameBytes);
		(LPWSTR(LPBYTE(pInstance)+pInstance->m_nInstanceNameOffset))[nNameLen] = 0;
	}

	*ppInstance = pInstance;

	return S_OK;
}

inline HRESULT CPerfMon::CreateInstanceByName(
		DWORD dwObjectType,
		LPWSTR szInstanceName,
		CPerfObject** ppInstance
		)
{
	CPerfObject* pEmptyBlock = NULL;

	if (ppInstance == NULL)
		return E_POINTER;
	else if (m_aMem.GetSize() == 0 || m_aMem[0]->GetData() == NULL || m_lock.m_h == NULL)
		return E_UNEXPECTED; // Initialize must succeed before calling CreateInstance

	*ppInstance = NULL;

	CPerfMapEntry* pObjectEntry = _FindObjectInfo(dwObjectType);
	if (pObjectEntry == NULL)
		return E_INVALIDARG;
	else if (pObjectEntry->m_nInstanceLess == PERF_NO_INSTANCES)
		return E_UNEXPECTED; // instanceless objects have no name

	CPerfLock lock(this);
	if (FAILED(lock.GetStatus()))
		return lock.GetStatus();

	CAtlFileMappingBase* pCurrentBlock = m_aMem[0];
	CPerfObject* pInstance = _GetFirstObject(pCurrentBlock);
	ULONG nMaxInstance = 0;
	ULONG nUsedSpace = 0;
	while (pInstance->m_nAllocSize != 0)
	{
		nUsedSpace += pInstance->m_nAllocSize;

		if (pInstance->m_dwObjectId == dwObjectType)
		{
			nMaxInstance = max(nMaxInstance, pInstance->m_dwInstance);

			LPWSTR szInstName = (LPWSTR(LPBYTE(pInstance)+pInstance->m_nInstanceNameOffset));
			if (wcsncmp(szInstName, szInstanceName, pObjectEntry->m_nMaxInstanceNameLen-1) == 0)
			{
				*ppInstance = pInstance;
				pInstance->m_nRefCount++;
				return S_OK;
			}
		}

		if (pInstance->m_nAllocSize == pObjectEntry->m_nAllocSize && pInstance->m_dwObjectId == 0)
			pEmptyBlock = pInstance;

		pInstance = _GetNextObject(pInstance);

		if (pInstance->m_nAllocSize == 0 &&
			m_nHeaderSize + nUsedSpace + pObjectEntry->m_nAllocSize + sizeof(CPerfObject) > m_nAllocSize)
		{
			// we've reached the end of the block and have no room to allocate an object of this
			// type. cap the block with a sentinel
			pInstance->m_nAllocSize = (ULONG) -1;
		}

		// check for an end-of-shared-mem sentinel
		if (pInstance->m_nAllocSize == (ULONG) -1)
		{
			nUsedSpace = 0;
			CAtlFileMappingBase* pNextBlock = _GetNextBlock(pCurrentBlock);
			if (pNextBlock == NULL)
			{
				// we've reached the last block of shared mem.
				// the instance hasn't been found, so either use a
				// previously freed instance block (pEmptyBlock) or allocate a new
				// shared mem block to hold the new instance
				if (pEmptyBlock == NULL)
				{
					pNextBlock = _AllocNewBlock(pCurrentBlock);
					if (pNextBlock == NULL)
						return E_OUTOFMEMORY;
				}
				else
					break;
			}
			pCurrentBlock = pNextBlock;
			pInstance = _GetFirstObject(pCurrentBlock);
		}
	}

	// allocate a new object
	if (pEmptyBlock != NULL)
		pInstance = pEmptyBlock;
	else
		pInstance->m_nAllocSize = pObjectEntry->m_nAllocSize;

	pInstance->m_dwObjectId = pObjectEntry->m_dwPerfId;
	pInstance->m_dwInstance = nMaxInstance + 1;
	pInstance->m_nRefCount = 1;

	// copy the instance name, truncate if necessary
	ULONG nNameLen = (ULONG)min(wcslen(szInstanceName), pObjectEntry->m_nMaxInstanceNameLen-1);
	ULONG nNameBytes = (nNameLen+1) * sizeof(WCHAR);
	pInstance->m_nInstanceNameOffset = pInstance->m_nAllocSize-nNameBytes;
	CopyMemory(LPBYTE(pInstance)+pInstance->m_nInstanceNameOffset, szInstanceName, nNameBytes);
	(LPWSTR(LPBYTE(pInstance)+pInstance->m_nInstanceNameOffset))[nNameLen] = 0;

	*ppInstance = pInstance;

	return S_OK;
}

inline HRESULT CPerfMon::ReleaseInstance(CPerfObject* pInstance)
{
	CPerfLock lock(this);
	if (FAILED(lock.GetStatus()))
		return lock.GetStatus();

	if (--pInstance->m_nRefCount == 0)
	{
		pInstance->m_dwInstance = 0;
		pInstance->m_dwObjectId = 0;
	}

	return S_OK;
}

inline HRESULT CPerfMon::Lock(DWORD dwTimeout /* == INFINITE */)
{
	if (m_lock.m_h == NULL)
		return E_UNEXPECTED;

	DWORD dwRes = WaitForSingleObject(m_lock.m_h, dwTimeout);
	if (dwRes == WAIT_ABANDONED || dwRes == WAIT_OBJECT_0)
		return S_OK;
	else if (dwRes == WAIT_TIMEOUT)
		return HRESULT_FROM_WIN32(ERROR_TIMEOUT);
	else
		return AtlHresultFromLastError();
}

inline HRESULT CPerfMon::UnLock()
{
	m_lock.Release();
	return S_OK;
}

// map building routines
inline HRESULT CPerfMon::AddObjectDefinition(
	DWORD dwPerfId,
	LPCTSTR szObjectName,
	LPCTSTR szHelpString,
	DWORD dwDetailLevel,
	INT nDefaultCounter,
	BOOL bInstanceLess,
	UINT nStructSize,
	UINT nMaxInstanceNameLen)
{
	// must have one and only one of these
	ATLASSERT(!bInstanceLess ^ !nMaxInstanceNameLen);

	CPerfMapEntry entry;

	entry.m_dwPerfId = dwPerfId;
	entry.m_szName = NULL;
	entry.m_szHelp = NULL;
	if (szObjectName)
	{
		ATLTRY(entry.m_szName = new TCHAR[lstrlen(szObjectName)+1]);
		if (entry.m_szName == NULL)
			return E_OUTOFMEMORY;
		lstrcpy(entry.m_szName, szObjectName);
	}
	if (szHelpString)
	{
		ATLTRY(entry.m_szHelp = new TCHAR[lstrlen(szHelpString)+1]);
		if (entry.m_szHelp == NULL)
		{
			delete [] entry.m_szName;
			return E_OUTOFMEMORY;
		}
		lstrcpy(entry.m_szHelp, szHelpString);
	}
	entry.m_dwDetailLevel = dwDetailLevel;
	entry.m_bIsObject = TRUE;

	// OBJECT INFO
	entry.m_nNumCounters = 0;
	entry.m_nDefaultCounter = nDefaultCounter;
	entry.m_nInstanceLess = bInstanceLess ? PERF_NO_INSTANCES : 0;
	entry.m_nStructSize = nStructSize;
	entry.m_nMaxInstanceNameLen = nMaxInstanceNameLen;
	entry.m_nAllocSize = nStructSize + nMaxInstanceNameLen*sizeof(WCHAR);

	// COUNTER INFO
	entry.m_dwCounterType = 0;
	entry.m_nDefaultScale = 0;
	entry.m_nMaxCounterSize = 0;
	entry.m_nDataOffset = 0;

	entry.m_nNameId = 0;
	entry.m_nHelpId = 0;

	if (!m_map.Add(entry))
	{
		delete [] entry.m_szName;
		delete [] entry.m_szHelp;
		return E_OUTOFMEMORY;
	}

	if (m_map.GetSize() == 1)
		m_nNumObjectTypes = 1;
	else
		m_nNumObjectTypes++;

	return S_OK;
}

inline HRESULT CPerfMon::AddCounterDefinition(
	DWORD dwPerfId,
	LPCTSTR szObjectName,
	LPCTSTR szHelpString,
	DWORD dwDetailLevel,
	DWORD dwCounterType,
	ULONG nMaxCounterSize,
	UINT nOffset,
	INT nDefaultScale)
{
	for (int i=m_map.GetSize()-1; i>=0; i--)
	{
		CPerfMapEntry& object = m_map[i];
		if (object.m_bIsObject)
		{
			CPerfMapEntry counter;

			counter.m_dwPerfId = dwPerfId;
			counter.m_szName = NULL;
			counter.m_szHelp = NULL;
			if (szObjectName)
			{
				ATLTRY(counter.m_szName = new TCHAR[lstrlen(szObjectName)+1]);
				if (counter.m_szName == NULL)
					return E_OUTOFMEMORY;
				lstrcpy(counter.m_szName, szObjectName);
			}
			if (szHelpString)
			{
				ATLTRY(counter.m_szHelp = new TCHAR[lstrlen(szHelpString)+1]);
				if (counter.m_szHelp == NULL)
				{
					delete [] counter.m_szName;
					return E_OUTOFMEMORY;
				}
				lstrcpy(counter.m_szHelp, szHelpString);
			}
			counter.m_dwDetailLevel = dwDetailLevel;
			counter.m_bIsObject = FALSE;

			// OBJECT INFO
			counter.m_nNumCounters = 0;
			counter.m_nDefaultCounter = 0;
			counter.m_nInstanceLess = 0;
			counter.m_nStructSize = 0;
			counter.m_nMaxInstanceNameLen = 0;
			counter.m_nAllocSize = 0;

			// COUNTER INFO
			counter.m_dwCounterType = dwCounterType;
			counter.m_nDefaultScale = nDefaultScale;
			counter.m_nMaxCounterSize = nMaxCounterSize;
			counter.m_nDataOffset = nOffset;

			object.m_nNumCounters++;
			if (counter.m_nMaxCounterSize > 0)
			{
				ATLASSERT(counter.m_dwCounterType & PERF_TYPE_TEXT);
				object.m_nAllocSize += counter.m_nMaxCounterSize * sizeof(WCHAR);
			}

			counter.m_nNameId = 0;
			counter.m_nHelpId = 0;

			if (!m_map.Add(counter))
			{
				delete [] counter.m_szName;
				delete [] counter.m_szHelp;
				return E_OUTOFMEMORY;
			}

			return S_OK;
		}
	}

	// found no object in map! must add object BEFORE adding counter!
	ATLASSERT(FALSE);
	return E_UNEXPECTED;
}

inline HRESULT CPerfMon::ClearMap()
{
	for (int i=0; i<m_map.GetSize(); i++)
	{
		CPerfMapEntry& entry = m_map[i];
		if (entry.m_szName)
			delete [] entry.m_szName;
		if (entry.m_szHelp)
			delete [] entry.m_szHelp;
	}

	m_map.RemoveAll();
	return S_OK;
}

#ifndef _ATL_PERF_NOXML

ATL_NOINLINE inline HRESULT CPerfMon::PersistToXML(IStream *pStream, BOOL bFirst/*=TRUE*/, BOOL bLast/*=TRUE*/)
{
	CPerfLock lock(this);
	if (FAILED(lock.GetStatus()))
		return ERROR_SUCCESS;

	CStringA strXML;
	HRESULT hr = S_OK;
	ULONG nLen = 0;
	
	if (bFirst)
	{
		strXML = "<?xml version=\"1.0\" ?>\r\n<perfPersist>\r\n";
		hr = pStream->Write(static_cast<LPCSTR>(strXML), strXML.GetLength(), &nLen);
		if (hr != S_OK)
			return hr;
	}

	strXML.Format("\t<perfmon name=\"%s\">\r\n", CT2A(GetAppName()));
	hr = pStream->Write(static_cast<LPCSTR>(strXML), strXML.GetLength(), &nLen);

	for (int i=0; i<m_map.GetSize(); i+= m_map[i].m_nNumCounters+1)
	{
		CPerfMapEntry *pObjectEntry = &m_map[i];
		CPerfMapEntry *pCounterEntry = pObjectEntry;
		CPerfMapEntry *pCounterEntries = pObjectEntry+1;

		CAtlFileMappingBase *pCurrentBlock = _GetNextBlock(NULL);
		CPerfObject *pInstance = _GetFirstObject(pCurrentBlock);

		strXML.Format("\t\t<perfObject perfid=\"%d\">\r\n", 
			pObjectEntry->m_dwPerfId, pObjectEntry->m_nNameId, pObjectEntry->m_nHelpId);

		hr = pStream->Write(static_cast<LPCSTR>(strXML), strXML.GetLength(), &nLen);
		if (hr != S_OK)
			return E_FAIL;

		while (pInstance && pInstance->m_nAllocSize)
		{
			if (pInstance->m_dwObjectId == pObjectEntry->m_dwPerfId)
			{
				if (pObjectEntry->m_nInstanceLess != PERF_NO_INSTANCES)
				{
					// handle the instance name
					LPCWSTR wszInstNameSrc = LPCWSTR(LPBYTE(pInstance)+pInstance->m_nInstanceNameOffset);
					int nInstLen = (int) wcslen(wszInstNameSrc);

					// convert to UTF8
					nLen = AtlUnicodeToUTF8(wszInstNameSrc, nInstLen, NULL, 0);
					LPSTR szUTF8 = (LPSTR)(malloc((nLen+1)*sizeof(char)));
					if (!szUTF8)
						return E_OUTOFMEMORY;
					nLen = AtlUnicodeToUTF8(wszInstNameSrc, nInstLen, szUTF8, nLen);
					szUTF8[nLen] = '\0';

					strXML.Format("\t\t\t<instance name=\"%s\">\r\n", szUTF8);
					free(szUTF8);
					hr = pStream->Write(static_cast<LPCSTR>(strXML), strXML.GetLength(), &nLen);
					if (hr != S_OK)
						return hr;
				}

				for (ULONG i=0; i<pObjectEntry->m_nNumCounters; i++)
				{
					switch (pCounterEntry->m_dwCounterType & 0x00000300)
					{
						case PERF_SIZE_DWORD:
						{
							strXML.Format("\t\t\t\t<counter type=\"perf_size_dword\" value=\"%d\" offset=\"%d\"/>\r\n",
								*LPDWORD(LPBYTE(pInstance)+pCounterEntries[i].m_nDataOffset), 
								pCounterEntries[i].m_nDataOffset);
							break;
						}
						case PERF_SIZE_LARGE:
						{
							strXML.Format("\t\t\t\t<counter type=\"perf_size_large\" value=\"%d\" offset=\"%d\"/>\r\n",
								*PULONGLONG(LPBYTE(pInstance)+pCounterEntries[i].m_nDataOffset),
								pCounterEntries[i].m_nDataOffset);
							break;
						}
						case PERF_SIZE_VARIABLE_LEN:
						{
							// TODO: output to stream
							LPSTR szUTF8=NULL;
							LPDWORD pOffset = LPDWORD(LPBYTE(pInstance)+pObjectEntry->m_nDataOffset);
							LPBYTE pSrc = LPBYTE(pInstance)+*pOffset;
							if ((pCounterEntry->m_dwCounterType & 0x00010000) == PERF_TEXT_UNICODE)
							{
								ULONG nTextLen = (ULONG)wcslen(LPCWSTR(pSrc));
								// convert to UTF8
								nLen = AtlUnicodeToUTF8(LPCWSTR(pSrc), nTextLen, NULL, 0);
								szUTF8 = (LPSTR)(malloc((nLen+1)*sizeof(char)));
								if (!szUTF8)
									return E_OUTOFMEMORY;

								nLen = AtlUnicodeToUTF8(LPCWSTR(pSrc), nTextLen, szUTF8, nLen);	
								szUTF8[nLen] = '\0';
							}
							else
							{
								ULONG nTextLen = (ULONG)strlen(LPCSTR(pSrc));
								szUTF8 = (LPSTR)(malloc((nTextLen+1)*sizeof(char)));
								if (!szUTF8)
									return E_OUTOFMEMORY;
								strcpy(szUTF8, LPCSTR(pSrc));
							}
							strXML.Format("\t\t\t\t<counter type=\"perf_size_variable_len\" value=\"%s\" offset=\"%d\"/>\r\n",
									szUTF8,
									*pOffset);
							free(szUTF8);
							break;
						}
						default:
							// error:
							return E_FAIL;
					}
					hr = pStream->Write(static_cast<LPCSTR>(strXML), strXML.GetLength(), &nLen);
					if (hr != S_OK)
						return hr;
				}
			}

			if (pObjectEntry->m_nInstanceLess != PERF_NO_INSTANCES)
			{
				hr = pStream->Write("\t\t\t</instance>\r\n", sizeof("\t\t\t</instance>\r\n")-1, &nLen);
				if (hr != S_OK)
					return hr;
			}

			pInstance = _GetNextObject(pInstance);
			if (pInstance->m_nAllocSize == (ULONG)-1)
			{
				pCurrentBlock = _GetNextBlock(pCurrentBlock);
				if (pCurrentBlock == NULL)
					pInstance = NULL;
				else
					pInstance = _GetFirstObject(pCurrentBlock);
			}
		}

		hr = pStream->Write("\t\t</perfObject>\r\n", sizeof("\t\t</perfObject>\r\n")-1, &nLen);
		if (hr != S_OK)
			return hr;
	}

	hr = pStream->Write("\t</perfmon>\r\n", sizeof("\t</perfmon>\r\n")-1, &nLen);
	if (hr != S_OK)
		return hr;

	if (hr == S_OK && bLast)
		hr = pStream->Write("</perfPersist>", sizeof("</perfPersist>")-1, &nLen);

	return hr;
}

// This function is very lenient with inappropriate XML
ATL_NOINLINE inline HRESULT CPerfMon::LoadFromXML(IStream *pStream)
{	
	// Get a lock
	CPerfLock lock(this);
	if (FAILED(lock.GetStatus()))
		return ERROR_SUCCESS;

	CComPtr<IXMLDOMDocument> spdoc;

	// load the xml
	HRESULT hr = CoCreateInstance(__uuidof(DOMDocument), NULL, CLSCTX_INPROC, __uuidof(IXMLDOMDocument), (void **) &spdoc);
	if (FAILED(hr))
	{
		return hr;
	}

	spdoc->put_async(VARIANT_FALSE);

	CComPtr<IPersistStreamInit> spSI;
	hr = spdoc->QueryInterface(&spSI);
	if (hr != S_OK)
		return hr;
	hr = spSI->Load(pStream);
	if (hr != S_OK)
		return hr;

	// validate that it is a perfPersist stream
	CComPtr<IXMLDOMElement> spRoot;

	hr = spdoc->get_documentElement(&spRoot);
	if (hr != S_OK)
		return hr;

	CComBSTR bstrName;
	hr = spRoot->get_baseName(&bstrName);
	if (wcscmp(bstrName, L"perfPersist"))
		return S_FALSE;

	USES_CONVERSION;	
	// find the appropriate perfmon node

	// REVIEW : Does GetAppName() have to return a LPCTSTR.  I hate TCHARs
	CComPtr<IXMLDOMNode> spChild;
	hr = spRoot->get_firstChild(&spChild);
	while (hr == S_OK)
	{
		bstrName.Empty();
		hr = spChild->get_baseName(&bstrName);
		if (hr == S_OK)
		{
			if (!wcscmp(bstrName, L"perfmon"))
			{
				bstrName.Empty();
				hr = _GetAttribute(spChild, L"name", &bstrName);
				if (hr == S_OK)
				{
					if (!_tcscmp(W2T(bstrName), GetAppName()))
						break;
				}
			}
		}

		CComPtr<IXMLDOMNode> spNext;
		hr = spChild->get_nextSibling(&spNext);
		spChild.Attach(spNext.Detach());
	}

	// there is no perfmon node in the XML for the current CPerfMon class
	if (hr != S_OK)
		return S_FALSE;

	CComPtr<IXMLDOMNode> spPerfRoot;
	spPerfRoot.Attach(spChild.Detach());

	// iterate over the objects in the perfmon subtree
	// this is the loop that does the real work
	hr = spPerfRoot->get_firstChild(&spChild);
	DWORD dwInstance = 1;
	while (hr == S_OK)
	{
		// see if it's a perfObject
		bstrName.Empty();
		hr = spChild->get_baseName(&bstrName);
		if (hr != S_OK || wcscmp(bstrName, L"perfObject"))
			return S_FALSE;

		// get the perfid
		bstrName.Empty();
		hr = _GetAttribute(spChild, L"perfid", &bstrName);
		DWORD dwPerfId = _wtoi(bstrName);

		// iterate over children
		CComPtr<IXMLDOMNode> spInstChild;
		hr = spChild->get_firstChild(&spInstChild);
		while (hr == S_OK)
		{
			// see if it's a instance
			bstrName.Empty();
			hr = spInstChild->get_baseName(&bstrName);
			if (hr != S_OK || wcscmp(bstrName, L"instance"))
				return S_FALSE;

			// get the instance name
			bstrName.Empty();
			hr = _GetAttribute(spInstChild, L"name", &bstrName);
			if (hr != S_OK)
				return S_FALSE;

			// create the instance
			// REVIEW : take a loook at the dwInstance stuff--is it acceptable?
			CPerfObject *pInstance = NULL;
			hr = CreateInstance(dwPerfId, dwInstance++, bstrName, &pInstance);
			if (hr != S_OK)
				return S_FALSE;

			// iterate over the counters and set the data
			CComPtr<IXMLDOMNode> spCntrChild;
			hr = spInstChild->get_firstChild(&spCntrChild);
			while (hr == S_OK)
			{
				// get the base name
				bstrName.Empty();
				hr = spCntrChild->get_baseName(&bstrName);
				if (hr != S_OK || wcscmp(bstrName, L"counter"))
					return S_FALSE;

				// get the type
				bstrName.Empty();
				hr = _GetAttribute(spCntrChild, L"type", &bstrName);
				if (hr != S_OK)
					return S_FALSE;
				
				// 0 - DWORD
				// 1 - ULONGLONG
				// 2 - variable length (string)
				// 3 - unknown type
				DWORD dwType;
				if (!wcscmp(bstrName, L"perf_size_dword"))
					dwType = 0;
				else if (!wcscmp(bstrName, L"perf_size_large"))
					dwType = 1;
				else if (!wcscmp(bstrName, L"perf_size_variable_len"))
					dwType = 2;
				else
					return S_FALSE;

				// get the value
				bstrName.Empty();
				hr = _GetAttribute(spCntrChild, L"value", &bstrName);
				if (hr != S_OK)
					return S_FALSE;

				CComBSTR bstrOffset;
				hr = _GetAttribute(spCntrChild, L"offset", &bstrOffset);
				if (hr != S_OK)
					return S_FALSE;

				WCHAR *pStop = NULL;
				DWORD dwOffset = wcstoul(bstrOffset, &pStop, 10);

				if (dwType == 0) // add it as a DWORD
				{
					DWORD dwVal = wcstoul(bstrName, &pStop, 10);
					*LPDWORD(LPBYTE(pInstance)+dwOffset) = dwVal;
				}
				else if (dwType == 1) // add it is a ULONGLONG
				{
					ULONGLONG dwdwVal = _wcstoui64(bstrName, &pStop, 10);
					*PULONGLONG(LPBYTE(pInstance)+dwOffset) = dwdwVal;
				}
				else // dwType == 2 -- add it as a string
				{
					// TODO (jasjitg): right now screws up on UNICODE strings -- Jerry, can you look at this?
					LPSTR szStr = (LPSTR)(malloc((bstrName.Length()+1)*sizeof(char)));
					szStr = AtlW2AHelper(szStr, bstrName, bstrName.Length(), ATL::_AtlGetConversionACP());
					memcpy(pInstance+dwOffset, szStr, bstrName.Length());
					free(szStr);
				}

				CComPtr<IXMLDOMNode> spCntrNext;
				hr = spCntrChild->get_nextSibling(&spCntrNext);
				spCntrChild.Attach(spCntrNext.Detach());
			}

			CComPtr<IXMLDOMNode> spInstNext;
			hr = spInstChild->get_nextSibling(&spInstNext);
			spInstChild.Attach(spInstNext.Detach());
		}

		CComPtr<IXMLDOMNode> spNext;
		hr = spChild->get_nextSibling(&spNext);
		spChild.Attach(spNext.Detach());
	}

	return S_OK;
}

// a little utility function to retrieve a named attribute from a node
ATL_NOINLINE inline HRESULT CPerfMon::_GetAttribute(IXMLDOMNode *pNode, LPCWSTR szAttrName, BSTR *pbstrVal)
{
	*pbstrVal = NULL;
	CComPtr<IXMLDOMNamedNodeMap> spAttrs;

	HRESULT hr = pNode->get_attributes(&spAttrs);
	if (hr != S_OK)
		return hr;
	
	CComPtr<IXMLDOMNode> spAttr;
	
	hr = spAttrs->getNamedItem((BSTR) szAttrName, &spAttr);
	if (hr != S_OK)
		return hr;
	
	CComVariant varVal;
	hr = spAttr->get_nodeValue(&varVal);
	if (hr != S_OK)
		return hr;
	
	hr = varVal.ChangeType(VT_BSTR);
	if (hr != S_OK)
		return hr;

	*pbstrVal = varVal.bstrVal;
	varVal.vt = VT_EMPTY;

	return S_OK;
}

#endif // _ATL_PERF_NOXML

} // namespace ATL

#endif // __ATLPERF_INL__
