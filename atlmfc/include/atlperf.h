// This is a part of the Active Template Library.
// Copyright (C) 1996-2000 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLPERF_H__
#define __ATLPERF_H__

#pragma once

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#include <atlbase.h>
#include <atlstr.h>
#include <atlfile.h>
#include <atlsync.h>
#include <winperf.h>

#ifndef _ATL_PERF_NOXML
#include <atlenc.h>
#include <oaidl.h>
#include <xmldomdid.h>
#include <xmldsodid.h>
#include <msxmldid.h>
#include <msxml.h>
#endif

namespace ATL
{

#ifndef ATLPERF_DEFAULT_MAXINSTNAMELENGTH
#define ATLPERF_DEFAULT_MAXINSTNAMELENGTH 64
#endif

// base class for user-defined perf objects
struct CPerfObject
{
	ULONG m_nAllocSize;
	DWORD m_dwObjectId;
	DWORD m_dwInstance;
	ULONG m_nRefCount;
	ULONG m_nInstanceNameOffset; // byte offset from beginning of PerfObject to LPWSTR szInstanceName
};

struct CPerfMapEntry
{
	DWORD m_dwPerfId;
	LPTSTR m_szName;
	LPTSTR m_szHelp;
	DWORD m_dwDetailLevel;
	BOOL m_bIsObject;

	// OBJECT INFO
	ULONG m_nNumCounters;
	LONG m_nDefaultCounter;
	LONG m_nInstanceLess; // PERF_NO_INSTANCES if instanceless
	
	// the size of the struct not counting the name and string counters
	ULONG m_nStructSize;
	
	 // in characters including the null terminator
	ULONG m_nMaxInstanceNameLen;

	ULONG m_nAllocSize;

	// COUNTER INFO
	DWORD m_dwCounterType;
	LONG m_nDefaultScale;

	// the maximum size of the string counter data in characters, including the null terminator
	// ignored if not a string counter
	ULONG m_nMaxCounterSize;

	ULONG m_nDataOffset;

	// the ids that correspond to the name and help strings stored in the registry
	UINT m_nNameId;
	UINT m_nHelpId;
};

inline bool operator==(const CPerfMapEntry& entry1, const CPerfMapEntry& entry2)
{
	return ((entry1.m_dwPerfId == entry2.m_dwPerfId) && (entry1.m_bIsObject == entry2.m_bIsObject));
}

class CPerfMon
{
public:
	~CPerfMon() throw();

	// PerfMon entry point helpers
	DWORD Open(LPWSTR lpDeviceNames) throw();
	DWORD Collect(LPWSTR lpwszValue, LPVOID* lppData, LPDWORD lpcbBytes, LPDWORD lpcObjectTypes) throw();
	DWORD Close() throw();

#ifdef _ATL_PERF_REGISTER
	// registration
	HRESULT Register(
		LPCTSTR szOpenFunc,
		LPCTSTR szCollectFunc,
		LPCTSTR szCloseFunc,
		HINSTANCE hDllInstance = _AtlBaseModule.GetModuleInstance()) throw();
	HRESULT RegisterStrings(
		LANGID wLanguage = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
		HINSTANCE hResInstance = _AtlBaseModule.GetResourceInstance()) throw();
	HRESULT RegisterAllStrings(HINSTANCE hResInstance = _AtlBaseModule.GetResourceInstance()) throw();
	HRESULT Unregister() throw();

	static BOOL CALLBACK EnumResLangProc(HINSTANCE hModule, LPCTSTR lpszType, LPCTSTR lpszName, LANGID wIDLanguage, LPARAM lParam);
#endif

	HRESULT Initialize() throw();
	HRESULT UnInitialize() throw();
	HRESULT CreateInstance(
		DWORD dwObjectType,
		DWORD dwInstance,
		LPWSTR szInstanceName,
		CPerfObject** ppInstance) throw();
	HRESULT CreateInstanceByName(
		DWORD dwObjectType,
		LPWSTR szInstanceName,
		CPerfObject** ppInstance) throw();
	HRESULT ReleaseInstance(CPerfObject* pInstance) throw();
	HRESULT Lock(DWORD dwTimeout = INFINITE) throw();
	HRESULT UnLock() throw();

	// map building routines
	HRESULT AddObjectDefinition(
		DWORD dwPerfId,
		LPCTSTR szObjectName,
		LPCTSTR szHelpString,
		DWORD dwDetailLevel,
		INT nDefaultCounter,
		BOOL bInstanceLess,
		UINT nStructSize,
		UINT nMaxInstanceNameLen = ATLPERF_DEFAULT_MAXINSTNAMELENGTH) throw();
	HRESULT AddCounterDefinition(
		DWORD dwPerfId,
		LPCTSTR szObjectName,
		LPCTSTR szHelpString,
		DWORD dwDetailLevel,
		DWORD dwCounterType,
		ULONG nMaxCounterSize,
		UINT nOffset,
		INT nDefaultScale) throw();
	HRESULT ClearMap() throw();

#ifndef _ATL_PERF_NOXML
	HRESULT PersistToXML(IStream *pStream, BOOL bFirst=TRUE, BOOL bLast=TRUE);
	HRESULT LoadFromXML(IStream *pStream);
#endif

protected:
	virtual LPCTSTR GetAppName() const throw() = 0;
	virtual HRESULT CreateMap(WORD wLanguage, HINSTANCE hResInstance, UINT* pSampleRes = NULL) throw();
	virtual void OnBlockAlloc(CAtlFileMappingBase* /*pNewBlock*/) { }

	// implementation helpers
	CPerfMapEntry* _GetMapEntry(int nIndex) throw();
	LPBYTE _AllocData(LPBYTE& pData, ULONG nBytesAvail, ULONG* pnBytesUsed, ULONG nBytesNeeded) throw();
	template<typename T> T* _AllocStruct(LPBYTE& pData, ULONG nBytesAvail, ULONG* pnBytesUsed, T*) throw()
	{
		return reinterpret_cast<T*>(_AllocData(pData, nBytesAvail, pnBytesUsed, sizeof(T)));
	}

	CPerfObject* _GetFirstObject(CAtlFileMappingBase* pBlock) throw();
	CPerfObject* _GetNextObject(CPerfObject* pInstance) throw();
	CAtlFileMappingBase* _GetNextBlock(CAtlFileMappingBase* pBlock) throw();
	CAtlFileMappingBase* _AllocNewBlock(CAtlFileMappingBase* pPrev, BOOL* pbExisted = NULL) throw();
	CPerfMapEntry* _FindObjectInfo(DWORD dwObjectId) throw();
	CPerfMapEntry* _FindCounterInfo(CPerfMapEntry* pObjectEntry, DWORD dwCounterId) throw();
	CPerfMapEntry* _FindCounterInfo(DWORD dwObjectId, DWORD dwCounterId) throw();
	BOOL _WantObjectType(LPWSTR lpwszValue, DWORD dwPerfId) throw();
	void _FillObjectType(PERF_OBJECT_TYPE* pObjectType, CPerfMapEntry* pObjectEntry) throw();
	void _FillCounterDef(
		PERF_COUNTER_DEFINITION* pCounterDef,
		CPerfMapEntry* pCounterEntry,
		ULONG& nCBSize) throw();
	HRESULT _CollectObjectType(
		CPerfMapEntry* pObjectEntry,
		LPBYTE pData,
		ULONG nBytesAvail,
		ULONG* pnBytesUsed) throw();
	HRESULT LoadMap() throw();
	HRESULT SaveMap() throw();
	HRESULT _GetAttribute(
		IXMLDOMNode *pNode, 
		LPCWSTR szAttrName, 
		BSTR *pbstrVal);

#ifdef _ATL_PERF_REGISTER
	HRESULT _AppendRegStrings(
		CRegKey& rkLang,
		LPCTSTR szValue,
		CSimpleArray<CString>& astrStrings,
		ULONG nNewStringSize,
		ULONG iNextIndex) throw();
	HRESULT _RemoveRegStrings(
		CRegKey& rkLang,
		LPCTSTR szValue,
		ULONG iFirstIndex,
		ULONG iLastIndex) throw();
#endif
private:
	CSimpleArray<CPerfMapEntry> m_map;
	CSimpleArray<CAtlFileMappingBase*> m_aMem;
	CMutex m_lock;
	ULONG m_nAllocSize;
	ULONG m_nHeaderSize;
	ULONG m_nSchemaSize;
	ULONG m_nNumObjectTypes;
};

class CPerfLock
{
public:
	CPerfLock(CPerfMon* pPerfMon, DWORD dwTimeout = INFINITE) throw()
	{
		ATLASSERT(pPerfMon != NULL);
		m_pPerfMon = pPerfMon;
		m_hrStatus = m_pPerfMon->Lock(dwTimeout);
	}

	~CPerfLock() throw()
	{
		if (SUCCEEDED(m_hrStatus))
			m_pPerfMon->UnLock();
	}

	HRESULT GetStatus() const throw()
	{
		return m_hrStatus;
	}

private:
	CPerfMon* m_pPerfMon;
	HRESULT m_hrStatus;
};

// empty definition just for ease of use with code wizards, etc.
#define BEGIN_PERFREG_MAP()

// empty definition just for ease of use with code wizards, etc.
#define END_PERFREG_MAP()

#if !defined(_ATL_PERF_REGISTER) | defined(_ATL_PERF_NOEXPORT)
#define PERFREG_ENTRY(className)
#endif

#ifdef _ATL_PERF_REGISTER
#define BEGIN_PERF_MAP(AppName) \
	private: \
		LPCTSTR GetAppName() const throw() { return AppName; } \
		HRESULT CreateMap(WORD wLanguage, HINSTANCE hResInstance, UINT* pSampleRes = NULL) throw() \
		{ \
			wLanguage; \
			hResInstance; \
			if (pSampleRes) \
				*pSampleRes = 0; \
			CString strName; \
			CString strHelp; \
			HRESULT hr; \
			ClearMap();

#define DEFINE_PERF_OBJECT_EX(perfid, namestring, helpstring, detail, instanceless, structsize, maxinstnamelen, defcounter) \
			hr = AddObjectDefinition(perfid, namestring, helpstring, detail, defcounter, instanceless, (ULONG) structsize, maxinstnamelen); \
			if (FAILED(hr)) \
				return hr;

#define DEFINE_PERF_OBJECT_RES_EX(perfid, namestringres, helpstringres, detail, instanceless, structsize, maxinstnamelen, defcounter) \
			if (pSampleRes) \
				*pSampleRes = namestringres; \
			if (hResInstance && !strName.LoadString(hResInstance, namestringres, wLanguage)) \
				return E_FAIL; \
			if (hResInstance && !strHelp.LoadString(hResInstance, helpstringres, wLanguage)) \
				return E_FAIL; \
			DEFINE_PERF_OBJECT_EX(perfid, strName, strHelp, detail, instanceless, structsize, maxinstnamelen, defcounter)

#define DEFINE_PERF_COUNTER_EX(perfid, namestring, helpstring, detail, countertype, maxcountersize, offset, defscale) \
			hr = AddCounterDefinition(perfid, namestring, helpstring, detail, countertype, maxcountersize, (ULONG) offset, defscale); \
			if (FAILED(hr)) \
				return hr;

#define DEFINE_PERF_COUNTER_RES_EX(perfid, namestringres, helpstringres, detail, countertype, maxcountersize, offset, defscale) \
			if (hResInstance && !strName.LoadString(hResInstance, namestringres, wLanguage)) \
				return E_FAIL; \
			if (hResInstance && !strHelp.LoadString(hResInstance, helpstringres, wLanguage)) \
				return E_FAIL; \
			DEFINE_PERF_COUNTER_EX(perfid, strName, strHelp, detail, countertype, maxcountersize, offset, defscale)

#define END_PERF_MAP() \
			return S_OK; \
		}

// define _ATL_PERF_NOEXPORT if you don't want to use the PERFREG map and don't want these
// functions exported from your DLL
#ifndef _ATL_PERF_NOEXPORT

// Perf register map stuff
// this is for ease of integration with the module attribute and for the 
// perfmon wizard

#pragma data_seg(push)
#pragma data_seg("ATLP$A")
__declspec(selectany) CPerfMon * __pperfA = NULL;
#pragma data_seg("ATLP$Z") 
__declspec(selectany) CPerfMon * __pperfZ = NULL;
#pragma data_seg("ATLP$C")
#pragma data_seg(pop)

ATL_NOINLINE inline HRESULT RegisterPerfMon(HINSTANCE hDllInstance = _AtlBaseModule.GetModuleInstance()) throw() 
{
	CPerfMon **ppPerf = &__pperfA; 
	HRESULT hr = S_OK; 
	while (ppPerf != &__pperfZ) 
	{ 
		if (*ppPerf != NULL) 
		{ 
			hr = (*ppPerf)->Register(_T("OpenPerfMon"), _T("CollectPerfMon"), _T("ClosePerfMon"), hDllInstance);
			if (FAILED(hr)) 
				return hr; 
			hr = (*ppPerf)->RegisterAllStrings(hDllInstance);
			if (FAILED(hr)) 
				return hr; 
		} 
		ppPerf++; 
	} 
	return S_OK; 
} 

ATL_NOINLINE inline HRESULT UnregisterPerfMon() throw() 
{ 
	CPerfMon **ppPerf = &__pperfA; 
	HRESULT hr = S_OK; 
	while (ppPerf != &__pperfZ) 
	{ 
		if (*ppPerf != NULL) 
		{ 
			hr = (*ppPerf)->Unregister(); 
			if (FAILED(hr)) 
				return hr; 
		} 
		ppPerf++; 
	} 
	return S_OK; 
} 

extern "C" ATL_NOINLINE inline DWORD __declspec(dllexport) WINAPI OpenPerfMon(LPWSTR lpDeviceNames) throw() 
{ 
	CPerfMon **ppPerf = &__pperfA; 
	DWORD dwErr = 0; 
	while (ppPerf != &__pperfZ) 
	{ 
		if (*ppPerf != NULL) 
		{ 
			dwErr = (*ppPerf)->Open(lpDeviceNames); 
			if (dwErr != 0) 
				return dwErr; 
		} 
		ppPerf++; 
	} 
	return 0; 
} 

extern "C" ATL_NOINLINE inline DWORD __declspec(dllexport) WINAPI CollectPerfMon(LPWSTR lpwszValue, LPVOID* lppData, 
	LPDWORD lpcbBytes, LPDWORD lpcObjectTypes) throw() 
{ 
	CPerfMon **ppPerf = &__pperfA; 
	DWORD dwErr = 0; 
	while (ppPerf != &__pperfZ) 
	{ 
		if (*ppPerf != NULL) 
		{ 
			dwErr = (*ppPerf)->Collect(lpwszValue, lppData, lpcbBytes, lpcObjectTypes); 
			if (dwErr != 0) 
				return dwErr; 
		} 
		ppPerf++; 
	} 
	return 0; 
} 

extern "C" ATL_NOINLINE inline DWORD __declspec(dllexport) WINAPI ClosePerfMon() throw() 
{ 
	CPerfMon **ppPerf = &__pperfA; 
	DWORD dwErr = 0; 
	while (ppPerf != &__pperfZ) 
	{ 
		if (*ppPerf != NULL) 
		{ 
			dwErr = (*ppPerf)->Close(); 
			if (dwErr != 0) 
				return dwErr; 
		} 
		ppPerf++; 
	} 
	return 0; 
} 

// REVIEW: Is this naughty?  It seems okay to do if _ATL_PERF_REGISTER is defined
#pragma comment(linker, "/EXPORT:OpenPerfMon=_OpenPerfMon@4")
#pragma comment(linker, "/EXPORT:CollectPerfMon=_CollectPerfMon@16")
#pragma comment(linker, "/EXPORT:ClosePerfMon=_ClosePerfMon@0")

// this class handles integrating the registration with CComModule
class _CAtlPerfSetFuncPtr
{
public:
	_CAtlPerfSetFuncPtr()
	{
		_pPerfRegFunc = RegisterPerfMon;
		_pPerfUnRegFunc = UnregisterPerfMon;
	}
};

extern "C" { __declspec(selectany) _CAtlPerfSetFuncPtr g_atlperfinit; }
//#pragma comment(linker, "/INCLUDE:?g_atlperfinit@ATL@@3V_CAtlPerfSetFuncPtr@1@A")
#pragma comment(linker, "/INCLUDE:_g_atlperfinit")

// _UsingPerfMap is just an empty stub function to check against
#define PERFREG_ENTRY(className) \
className __perf_##className; \
__declspec(allocate("ATLP$C")) CPerfMon * __pperf_##className = static_cast<CPerfMon*>(&__perf_##className); 

#endif // _ATL_PERF_NOEXPORT

#else // _ATL_PERF_REGISTER

#define BEGIN_PERF_MAP(AppName) \
	private: \
		LPCTSTR GetAppName() const throw() { return AppName; }

#define DEFINE_PERF_OBJECT_EX(perfid, namestring, helpstring, detail, instanceless, structsize, maxinstnamelen, defcounter)
#define DEFINE_PERF_OBJECT_RES_EX(perfid, namestringres, helpstringres, detail, instanceless, structsize, maxinstnamelen, defcounter)
#define DEFINE_PERF_COUNTER_EX(perfid, namestring, helpstring, detail, countertype, maxcountersize, offset, defscale)
#define DEFINE_PERF_COUNTER_RES_EX(perfid, namestringres, helpstringres, detail, countertype, maxcountersize, offset, defscale)

#define END_PERF_MAP()

#endif // _ATL_PERF_REGISTER

#define DEFINE_PERF_OBJECT(perfid, namestring, helpstring, structsize) \
	DEFINE_PERF_OBJECT_EX(perfid, namestring, helpstring, PERF_DETAIL_NOVICE, 0, structsize, ATLPERF_DEFAULT_MAXINSTNAMELENGTH, -1)
#define DEFINE_PERF_OBJECT_NO_INSTANCES(perfid, namestring, helpstring, structsize) \
	DEFINE_PERF_OBJECT_EX(perfid, namestring, helpstring, PERF_DETAIL_NOVICE, PERF_NO_INSTANCES, structsize, 0, -1)

#define DEFINE_PERF_COUNTER(perfid, namestring, helpstring, countertype, offset, defscale) \
	DEFINE_PERF_COUNTER_EX(perfid, namestring, helpstring, PERF_DETAIL_NOVICE, countertype, 0, offset, defscale)

#define DEFINE_PERF_OBJECT_RES(perfid, namestringres, helpstringres, structsize) \
	DEFINE_PERF_OBJECT_RES_EX(perfid, namestringres, helpstringres, PERF_DETAIL_NOVICE, 0, structsize, ATLPERF_DEFAULT_MAXINSTNAMELENGTH, -1)
#define DEFINE_PERF_OBJECT_NO_INSTANCES_RES(perfid, namestringres, helpstringres, structsize) \
	DEFINE_PERF_OBJECT_RES_EX(perfid, namestringres, helpstringres, PERF_DETAIL_NOVICE, PERF_NO_INSTANCES, structsize, 0, -1)

#define DEFINE_PERF_COUNTER_RES(perfid, namestringres, helpstringres, countertype, offset, defscale) \
	DEFINE_PERF_COUNTER_RES_EX(perfid, namestringres, helpstringres, PERF_DETAIL_NOVICE, countertype, 0, offset, defscale)

} // namespace ATL

#endif // __ATLPERF_H__
