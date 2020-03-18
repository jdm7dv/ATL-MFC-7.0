// This is a part of the Active Template Library.
// Copyright (C) 1996-2000 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#pragma once

#include <atlsoap.h>
#include <atlutil.h>
#include <atlsrvres.h>
#include <atlsecurity.h>

//
// You can change the local group that is used for authorizing
// site administrators by #define'ing ATL_DEFAULT_AUTH group
// to something else before including this header file. For
// example:
// #define ATL_DEFAULT_AUTHGRP CSid(_T("My Heros"))
//     Verify that the logged on user is a members of
//     the local group 'My Heros' before allowing them to
//     administrate this site.
//
// #define ATL_DEFAULT_AUTHGRP Sids::World
//     Allow everyone access
//
// #define ATL_DEFAULT_AUTHGRP Sids::Null
//     Allow noone access
//
#ifndef ATL_DEFAULT_AUTHGRP
	#define ATL_DEFAULT_AUTHGRP Sids::Admins()
#endif

//REVIEW: remove this after compiler problem with calling the
// provider when specializing member variables
struct _tempstruct
{
	CComPtr<IThreadPoolConfig>    m_1;
	CComPtr<IMemoryCacheStats>    m_2;
	CComPtr<IStencilCacheControl> m_3;
	CComPtr<IDllCache>            m_4;
	CComPtr<IStencilCache>        m_5;
};

// If you #define ATL_NO_DEFAULT_AUTHORITY then there will be no authorization
// check before allowing access to management functions. You can also #define
// ATL_NO_DEFAULT_AUTHORITY and then declare you own instance of _Authority
// before #includin'ing atlextmgmt.h to use a different authorization scheme.
#ifndef ATL_NO_DEFAULT_AUTHORITY
	__declspec(selectany) CDefaultAuth _Authority;
#endif

#ifndef IDR_THREADMGR_SRF
	#define IDR_THREADMGR_SRF "THREADMGR.SRF"
#endif

#ifndef IDR_STENCILMGR_SRF
	#define IDR_STENCILMGR_SRF "STENCILMGR.SRF"
#endif

#ifndef IDR_DLLMGR_SRF
	#define IDR_DLLMGR_SRF "DLLMGR.SRF"
#endif

#if defined(_ATL_THREADPOOL_MANAGEMENT) || defined(_ATL_STENCILCACHE_MANAGEMENT) || defined(_ATL_DLLCACHE_MANAGEMENT)
#ifndef NO_ATL_MGMT_STENCIL_WARNING
	#pragma message("*************** Please Note ***************")
	#pragma message("Your usage of atlextmgmt.h requires you to include management")
	#pragma message("stencil resources in your module's resource file.")
	#pragma message("Please make sure you include atlsrv.rc in your resource file.\r\n")
#endif
#endif

namespace ATL {

[emitidl(restricted)];

#define ATL_COLOR_TR1			RGB(0xd2, 0xff, 0xff)
#define ATL_COLOR_TR2			RGB(0xd2, 0xff, 0xd2)
#define ATL_COLOR_BODYBG		RGB(0xec, 0xf9, 0xec)

// _AtlRedirectToPage builds up a redirect URL from the
// current request plus a Handler= specification and 
// redirects the user's browser to that page.
inline HTTP_CODE _AtlRedirectToPage(
	IHttpServerContext *pContext,
	CHttpRequest& request,
	CHttpResponse& response,
	const char *szHandler) throw()
{
	ATLASSERT(pContext);
	const char *pProto = "http://";
	size_t len = strlen(pProto);
	char buff[ATL_URL_MAX_URL_LENGTH];
	DWORD dwLen = ATL_URL_MAX_URL_LENGTH-len;
	strcpy(buff, pProto);
	pContext->GetServerVariable("SERVER_NAME", (char*)(buff + len), &dwLen);
	len += dwLen-1; // The length returned in dwLen includes the NULL
	strcpy((char*)(buff + len), "/");
	len++;

	dwLen = ATL_URL_MAX_URL_LENGTH - len;
	request.GetUrl((char*)(buff + len), &dwLen); 
	len += dwLen-1; // The length returned in dwLen includes the NULL

	strcpy((char*)(buff + len), szHandler);;
	response.Redirect(buff);

	return HTTP_SUCCESS_NO_PROCESS;

}

#ifdef _ATL_THREADPOOL_MANAGEMENT
///////////////////////////////////////////////////////////////////////
// Thread pool management

[ uuid("44e9962a-5207-4d2a-a466-5f08a76e0e5d"), object ]
__interface IThreadPoolMgr
{
    [id(0)] STDMETHOD(SetSize)([in] int nNumThreads);
    [id(1)] STDMETHOD(GetSize)([out,retval] int *pnNumThreads);

};

#pragma warning(push)
#pragma warning(disable:4199)
[
	soap_handler(
					name=      "ThreadPoolManager", 
					namespace= "http://www.microsoft.com/vc/atlserver/soap/ThreadPoolManager",
					protocol=  "soap"
				),
	request_handler(
					name= "ThreadPoolManager",
					sdl=  "GenThreadPoolManagerSDL"
					)
]
class CThreadPoolManager :
	public IThreadPoolMgr
{
#pragma warning(pop)
public:
	[soap_method]
    STDMETHOD(SetSize)(int nNumThreads)
	{
		if (!m_spThreadPoolConfig)
			return E_UNEXPECTED;

		return m_spThreadPoolConfig->SetSize(nNumThreads);
	}

	[soap_method]
    STDMETHOD(GetSize)(int *pnNumThreads)
	{	
		if (!m_spThreadPoolConfig)
			return E_UNEXPECTED;

		return m_spThreadPoolConfig->GetSize(pnNumThreads);

	}

	HTTP_CODE InitializeTPMgr(IServiceProvider *pProvider)
	{
		ATLASSERT(pProvider); // should never be NULL
		if (!pProvider)
			return HTTP_ERROR(500, ISE_SUBERR_UNEXPECTED);

		if (m_spThreadPoolConfig)
			return HTTP_SUCCESS; // already initialized

		pProvider->QueryService(__uuidof(IThreadPoolConfig), &m_spThreadPoolConfig);
		return m_spThreadPoolConfig ? HTTP_SUCCESS : HTTP_ERROR(500, ISE_SUBERR_UNEXPECTED);
		
	}

	// override HandleRequest to Initialize our m_spServiceProvider
	// and to handle authorizing the client.
	HTTP_CODE HandleRequest(AtlServerRequest *pRequestInfo, IServiceProvider *pProvider)
	{
		HTTP_CODE hcErr = InitializeTPMgr(pProvider);
		if (hcErr != HTTP_SUCCESS)
			return hcErr;
		
		// Make sure caller is authorized on this system
__if_exists(_Authority)
{
		hcErr = HTTP_FAIL;
		ATLTRY(hcErr = _Authority.IsAuthorized(pRequestInfo, ATL_DEFAULT_AUTHGRP));
}
		if (hcErr == HTTP_SUCCESS)
		{
			hcErr = __super::HandleRequest(pRequestInfo, pProvider);
		}
		return hcErr;
	}

	CComPtr<IServiceProvider> m_spServiceProvider;
	CComPtr<IThreadPoolConfig> m_spThreadPoolConfig;
};


#define INVALID_COMMAND_ID -1
#define MAX_COMMAND_ID 64
                       
[request_handler(name="ThreadMgrSrf")]
class CThreadMgrStencil
{
	static CComObjectGlobal<CThreadPoolManager> m_tpmgr;
	long m_nColor;
	CString m_strUrl;
public:
	CThreadMgrStencil() :
		m_nColor(ATL_COLOR_TR1)
	{

	}

	[tag_name("GetSize")]
	HTTP_CODE GetSize()
	{
		int nSize = 0;
		HRESULT hr = m_tpmgr.GetSize(&nSize);
		if (SUCCEEDED(hr))
		{
			m_HttpResponse << nSize;
		}
		else
			m_HttpResponse << "size not found";
		
		return HTTP_SUCCESS;
	}

	[tag_name("GetTRColor")]
	HTTP_CODE GetTRColor()
	{
		m_nColor = (m_nColor == ATL_COLOR_TR1) ? ATL_COLOR_TR2 : ATL_COLOR_TR1;
		TCHAR cr[8];
		if (RGBToHtml(m_nColor, cr, sizeof(cr)))
			m_HttpResponse << cr;
		
		return HTTP_SUCCESS;
	}

	[tag_name("GetBodyColor")]
	HTTP_CODE GetBodyColor()
	{
		TCHAR cr[8];
		if (RGBToHtml(ATL_COLOR_BODYBG, cr, sizeof(cr)))
			m_HttpResponse << cr;
		return HTTP_SUCCESS;
	}


	HTTP_CODE ValidateAndExchange() throw()
	{
		// Initialize the thread pool manager instance. Internally
		// the initialize function will only intialize it's data structures
		// once.
		HTTP_CODE hcErr = m_tpmgr.InitializeTPMgr(m_spServiceProvider);
		if (hcErr != HTTP_SUCCESS)
			return hcErr;

__if_exists(_Authority)
{
			// Make sure caller is authorized on this system
			hcErr = HTTP_FAIL;
			ATLTRY(hcErr = _Authority.IsAuthorized(m_pRequestInfo, ATL_DEFAULT_AUTHGRP));
			if (hcErr != HTTP_SUCCESS)
				return hcErr;
}

		// load the stencil to render from a resource
		hcErr = LoadStencilResource(m_hInstHandler, IDR_THREADMGR_SRF);
		if (hcErr)
			return hcErr;

		m_HttpResponse.SetContentType("text/html");

		CString strHandler, strOptParam;
		int nCmdToExec = INVALID_COMMAND_ID;

		// check to see if we have a "Method" form variable
		DWORD dwErr = m_HttpRequest.QueryParams.Exchange("Method", &strHandler);
		if (dwErr != VALIDATION_S_OK)
			return HTTP_SUCCESS; // nothing to do.

		if (strHandler != _T("ExecuteCommand"))
			return HTTP_SUCCESS; // Wrong handler was passed

		// get the value of the command parameter so we can execute it
		dwErr = m_HttpRequest.QueryParams.Validate("command", &nCmdToExec, 0, MAX_COMMAND_ID);
		if (dwErr != VALIDATION_S_OK)
			return HTTP_SUCCESS; // Wrong command id, nothing to do.

		// get the optional parameter if it's there.
		m_HttpRequest.QueryParams.Validate("DynValue", &strOptParam, 0, MAX_COMMAND_ID);

		hcErr = ExecCommand(nCmdToExec, strOptParam);
		return hcErr;
	}

	HTTP_CODE ExecCommand(int nCmdToExec, CString& strOptParam)
	{
		switch (nCmdToExec)
		{
		case 0:
			TCHAR *pStop = NULL;
			int nValue = _tcstoul(strOptParam, &pStop, 10);
			m_tpmgr.SetSize(nValue);
		break;
		};

		return _AtlRedirectToPage(
							m_spServerContext,
							m_HttpRequest,
							m_HttpResponse,
							"?Handler=ThreadMgrSrf"
							);
	}
};

__declspec(selectany) CComObjectGlobal<CThreadPoolManager> CThreadMgrStencil::m_tpmgr;

#endif // _ATL_THREADPOOL_MANAGEMENT

#ifdef _ATL_STENCILCACHE_MANAGEMENT
//////////////////////////////////////////////////////////////////////
// Stencil cache management

[ uuid("3813895C-4C4C-41df-95F4-12220140B164"), object ]
__interface IStencilCacheMgr
{
	// data access
	[id(0)] STDMETHOD(GetCurrentEntryCount)([out,retval] DWORD *pdwSize);
	[id(1)] STDMETHOD(GetHitCount)([out,retval] DWORD *pdwSize);
	[id(2)] STDMETHOD(GetMissCount)([out,retval] DWORD *pdwSize);
	[id(3)] STDMETHOD(GetCurrentAllocSize)([out,retval] DWORD *pdwSize);
	[id(4)] STDMETHOD(GetMaxAllocSize)([out,retval] DWORD *pdwSize);
	[id(5)] STDMETHOD(GetMaxEntryCount)([out,retval] DWORD *pdwSize);
	[id(6)] STDMETHOD(GetDefaultLifespan)([out,retval] unsigned __int64 *pdwdwLifespan);

	// commands
	[id(7)] STDMETHOD(ClearStats)();
	[id(8)] STDMETHOD(RemoveStencil)([in] __int64 hStencil);
	[id(9)] STDMETHOD(RemoveStencilByName)([in] BSTR szStencil);
	[id(10)] STDMETHOD(RemoveAllStencils)();
	[id(11)] STDMETHOD(SetDefaultLifespan)([in] unsigned __int64 dwdwLifespan);
};

#pragma warning(push)
#pragma warning(disable:4199)
[
	soap_handler(	name=		"StencilCacheManager", 
					namespace=	"http://www.microsoft.com/vc/atlserver/soap/StencilCacheManager",
					protocol=	"soap"
				),
	request_handler(
					name=		"StencilCacheManager",
					sdl=		"GenStencilCacheManagerSDL")
]
class CStencilCacheManager :
	public IStencilCacheMgr
{
#pragma warning(pop)
public:
	[ soap_method ]
	STDMETHOD(GetCurrentEntryCount)(DWORD *pdwSize)
	{
		ATLASSERT(m_spMemCacheStats);
		return m_spMemCacheStats->GetCurrentEntryCount(pdwSize);		
	}

	[ soap_method ]
	STDMETHOD(ClearStats)()
	{
		ATLASSERT(m_spMemCacheStats);
		return m_spMemCacheStats->ClearStats();		
	}

	[ soap_method ]
	STDMETHOD(GetHitCount)(DWORD *pdwSize)
	{
		ATLASSERT(m_spMemCacheStats);
		return m_spMemCacheStats->GetHitCount(pdwSize);		
	}

	[ soap_method ]
	STDMETHOD(GetMissCount)(DWORD *pdwSize)
	{
		ATLASSERT(m_spMemCacheStats);
		return m_spMemCacheStats->GetMissCount(pdwSize);		
	}

	[ soap_method ]
	STDMETHOD(GetCurrentAllocSize)(DWORD *pdwSize)
	{
		ATLASSERT(m_spMemCacheStats);
		return m_spMemCacheStats->GetCurrentAllocSize(pdwSize);		
	}

	[ soap_method ]
	STDMETHOD(GetMaxAllocSize)(DWORD *pdwSize)
	{
		ATLASSERT(m_spMemCacheStats);
		return m_spMemCacheStats->GetMaxAllocSize(pdwSize);		
	}

	[ soap_method ]
	STDMETHOD(GetMaxEntryCount)(DWORD *pdwSize)
	{
		ATLASSERT(m_spMemCacheStats);
		return m_spMemCacheStats->GetMaxEntryCount(pdwSize);		
	}

	[ soap_method ]
	STDMETHOD(RemoveStencil)(__int64 hStencil)
	{
		ATLASSERT(m_spStencilCacheControl);
		return m_spStencilCacheControl->RemoveStencil((const HANDLE)hStencil);		
	}

	[ soap_method ]
	STDMETHOD(RemoveStencilByName)(BSTR szStencil)
	{
		ATLASSERT(m_spStencilCacheControl);
		return m_spStencilCacheControl->RemoveStencilByName(CW2A(szStencil));
	}

	[ soap_method ]
	STDMETHOD(RemoveAllStencils)()
	{
		ATLASSERT(m_spStencilCacheControl);
		return m_spStencilCacheControl->RemoveAllStencils();		
	}

	// we show lifespan in milli-seconds in the UI so we have to
	// do the conversion to 100ns intervals here.
	[ soap_method ]
	STDMETHOD(SetDefaultLifespan)(unsigned __int64 dwdwLifespan)
	{
		ATLASSERT(m_spStencilCacheControl);
		// convert to 100ns intervals
		return m_spStencilCacheControl->SetDefaultLifespan(dwdwLifespan * CFileTime::Millisecond);
	}

	[ soap_method ]
	STDMETHOD(GetDefaultLifespan)(unsigned __int64 *pdwdwLifespan)
	{
		ATLASSERT(m_spStencilCacheControl);
		*pdwdwLifespan = 0;
		unsigned __int64 dwls = 0;
		HRESULT hr = m_spStencilCacheControl->GetDefaultLifespan(&dwls);

		// convert to milli seconds
		if (SUCCEEDED(hr))
		{
			dwls /= CFileTime::Millisecond;
			*pdwdwLifespan = dwls;
		}
			
		return hr;
	}

	HTTP_CODE InitializeSCMgr(IServiceProvider *pProvider)
	{

		ATLASSERT(pProvider); // should never be NULL
		if (!pProvider)
			return HTTP_ERROR(500, ISE_SUBERR_UNEXPECTED);


		if (m_spMemCacheStats && m_spStencilCacheControl)
			return HTTP_SUCCESS; // already initialized

		CComPtr<IStencilCache> spStencilCache;
		pProvider->QueryService(__uuidof(IStencilCache), &spStencilCache);
		if (spStencilCache)
		{
			if (!m_spMemCacheStats)
			{
				spStencilCache->QueryInterface(__uuidof(IMemoryCacheStats), 
					(void**)&m_spMemCacheStats);
			}
			if (!m_spStencilCacheControl)
			{
				spStencilCache->QueryInterface(__uuidof(IStencilCacheControl),
					(void**)&m_spStencilCacheControl);
			}
		}

		return (m_spMemCacheStats && m_spStencilCacheControl)
			? HTTP_SUCCESS : HTTP_ERROR(500, ISE_SUBERR_UNEXPECTED);
	}

	HTTP_CODE HandleRequest(AtlServerRequest *pRequestInfo, IServiceProvider *pProvider)
	{
		HTTP_CODE hcErr = InitializeSCMgr(pProvider);
		if (hcErr != HTTP_SUCCESS)
			return hcErr;

__if_exists(_Authority)
{
		// Make sure caller is authorized on this system
		hcErr = HTTP_FAIL;
		ATLTRY(hcErr = _Authority.IsAuthorized(pRequestInfo, ATL_DEFAULT_AUTHGRP));
}
		if (hcErr == HTTP_SUCCESS)
		{
			hcErr = __super::HandleRequest(pRequestInfo, pProvider);
		}
		return hcErr;
	}

	CComPtr<IMemoryCacheStats> m_spMemCacheStats;
	CComPtr<IStencilCacheControl> m_spStencilCacheControl;
};

typedef HRESULT (__stdcall CStencilCacheManager::*PFNGETDATA)(__int64 *pdwSize);

struct CCache_data
{
	PFNGETDATA m_pfn;
	char m_sz[128];
};

#define INVALID_DATA_PTR ((DWORD_PTR) -1)
#define INVALID_COMMAND_ID -1
#define MAX_COMMAND_ID 64
#define ATL_STENCILCACHECMD_CLEARALLSTATS		0
#define ATL_STENCILCACHECMD_REMOVESTENCIL		1
#define ATL_STENCILCACHECMD_REMOVEALLSTENCILS	2
#define ATL_STENCILCACHECMD_SETDEFLIFESPAN		3

[request_handler(name="StencilMgrSrf")]
class CStencilMgr
{
	CCache_data *m_pData;
	long m_nColor;
public:
	CStencilMgr()
	{
		m_pData = (CCache_data*)INVALID_DATA_PTR;
		m_nColor=ATL_COLOR_TR1;
	}

	static CComObjectGlobal<CStencilCacheManager> m_cachemgr;
	static CCache_data* GetCacheData()
	{
		static CCache_data cache_data[] = 
		{
			{(PFNGETDATA)&CStencilCacheManager::GetCurrentEntryCount, "Current Cache Entry Count(stencils)"},
			{(PFNGETDATA)&CStencilCacheManager::GetHitCount, "Cache Hit Count(stencils)"},
			{(PFNGETDATA)&CStencilCacheManager::GetMissCount, "Cache Miss Count(stencils)"},
			{(PFNGETDATA)&CStencilCacheManager::GetCurrentAllocSize, "Cache memory allocation(bytes)"},
			{(PFNGETDATA)&CStencilCacheManager::GetMaxAllocSize, "Cache maximum allocation size(bytes)"},
			{(PFNGETDATA)&CStencilCacheManager::GetMaxEntryCount, "Cache maximum entry count(stencils)"},
			{(PFNGETDATA)&CStencilCacheManager::GetDefaultLifespan, "Default stencil lifespan(ms)"},
			{NULL, NULL}
		};
		return cache_data;
	}

	HTTP_CODE ValidateAndExchange() throw()
	{
		HTTP_CODE hcErr = m_cachemgr.InitializeSCMgr(m_spServiceProvider);
		if (hcErr != HTTP_SUCCESS)
				return hcErr;

__if_exists(_Authority)
{
		// Make sure caller is authorized on this system
		hcErr = HTTP_FAIL;
		ATLTRY(hcErr = _Authority.IsAuthorized(m_pRequestInfo, ATL_DEFAULT_AUTHGRP));
		if (hcErr != HTTP_SUCCESS)
			return hcErr;
}
		hcErr = LoadStencilResource(m_hInstHandler, IDR_STENCILMGR_SRF);
		if (hcErr)
			return hcErr;

		m_HttpResponse.SetContentType("text/html");

		// check to see if we have a "Handler" form variable
		CString strHandler, strOptParam;
		int nCmdToExec;

		DWORD dwErr = m_HttpRequest.QueryParams.Exchange("Method", &strHandler);
		if (dwErr != VALIDATION_S_OK)
			return HTTP_SUCCESS; // nothing to do.

		if (strHandler != _T("ExecuteCommand"))
			return HTTP_SUCCESS; // Wrong handler was passed

		// get the value of the command parameter so we can execute it
		dwErr = m_HttpRequest.QueryParams.Validate("command", &nCmdToExec, 0, MAX_COMMAND_ID);
		if (dwErr != VALIDATION_S_OK)
			return HTTP_SUCCESS; // Wrong command id, nothing to do.

		// get the optional parameter if it's there.
		m_HttpRequest.QueryParams.Validate("DynValue", &strOptParam, 0, MAX_COMMAND_ID);

		hcErr = ExecCommand(nCmdToExec, strOptParam);
		return hcErr;
	}

	HTTP_CODE ExecCommand(int nCmdToExec, CString& strOptParam)
	{
		switch (nCmdToExec)
		{
		case ATL_STENCILCACHECMD_CLEARALLSTATS:
			m_cachemgr.ClearStats();
		break;

		case ATL_STENCILCACHECMD_REMOVESTENCIL:
			m_cachemgr.RemoveStencilByName(strOptParam.AllocSysString());
		break;

		case ATL_STENCILCACHECMD_REMOVEALLSTENCILS:
			m_cachemgr.RemoveAllStencils();
		break;

		case ATL_STENCILCACHECMD_SETDEFLIFESPAN:
			TCHAR *pStop = NULL;
			m_cachemgr.SetDefaultLifespan(_tcstoul(strOptParam, &pStop, 10));
		break;
		};

		return _AtlRedirectToPage(
									m_spServerContext,
									m_HttpRequest,
									m_HttpResponse,
									"?Handler=StencilMgrSrf"
								 );

	}

	[tag_name("GetNextStencilCacheStats")]
	HTTP_CODE GetNextStencilCacheStats()
	{
		if (m_pData == (CCache_data*)INVALID_DATA_PTR)
		{
			m_pData = GetCacheData();
			return HTTP_SUCCESS;
		}
		m_pData++;

		if (m_pData->m_pfn != NULL)
			return HTTP_SUCCESS;

		m_pData = (CCache_data*)INVALID_DATA_PTR;
		return HTTP_S_FALSE;

	}

	[tag_name("GetCacheValue")]
	HTTP_CODE GetCacheValue()
	{
		ATLASSERT(m_pData);
		ATLASSERT(m_pData != (CCache_data*)INVALID_DATA_PTR);
		m_HttpResponse << m_pData->m_sz;
		return HTTP_SUCCESS;
	}

	[tag_name("GetCacheQuantity")]
	HTTP_CODE GetCacheQuantity()
	{
		ATLASSERT(m_pData);
		ATLASSERT(m_pData != (CCache_data*)INVALID_DATA_PTR);
		__int64 dwValue = 0;
		PFNGETDATA pfn = m_pData->m_pfn;
		CStencilCacheManager *pMgr = (CStencilCacheManager*)&m_cachemgr;
		(pMgr->*pfn)(&dwValue);

		m_HttpResponse << dwValue;
		return HTTP_SUCCESS;
	}

	[tag_name("GetTRColor")]
	HTTP_CODE GetTRColor()
	{
		m_nColor = (m_nColor == ATL_COLOR_TR1) ? ATL_COLOR_TR2 : ATL_COLOR_TR1;
		TCHAR cr[8];
		if (RGBToHtml(m_nColor, cr, sizeof(cr)))
			m_HttpResponse << cr;
	
		return HTTP_SUCCESS;
	}

	[tag_name("GetBodyColor")]
	HTTP_CODE GetBodyColor()
	{
		TCHAR cr[8];
		if (RGBToHtml(ATL_COLOR_BODYBG, cr, sizeof(cr)))
			m_HttpResponse << cr;
		return HTTP_SUCCESS;
	}
};



__declspec(selectany) CComObjectGlobal<CStencilCacheManager> CStencilMgr::m_cachemgr;
#endif // _ATL_STENCILCACHE_MANAGEMENT

//////////////////////////////////////////////////////////////////////
// DLL cache management
#ifdef _ATL_DLLCACHE_MANAGEMENT

// _DLL_CACHE_ENTRY is our own version of DLL_CACHE_ENTRY(atlcache.h) that
// uses a BSTR instead of a fixed length string for the szDllName for compatiblility
// with our SOAP implementation.
[export]
struct _DLL_CACHE_ENTRY
{
	DWORD hInstDll;
	DWORD dwRefs;
	BSTR szDllName;
};

[ uuid("A0C00AF8-CEA5-46b9-97ED-FDEE55B583EF"), object ]
__interface IDllCacheMgr
{
	[id(0)] STDMETHOD(GetEntries)([in] DWORD dwCount, [out] _DLL_CACHE_ENTRY *pEntries, [out, retval] DWORD *pdwCopied);
	[id(1)] STDMETHOD(GetEntryCount)([out, retval] DWORD *pdwCount);

};


#pragma warning(push)
#pragma warning(disable:4199)
[
	soap_handler(
					name=		"DllCacheManager", 
					namespace=	"http://www.microsoft.com/vc/atlserver/soap/DllCacheManager",
					protocol=	"soap"
				),
	request_handler(
					name=		"DllCacheManager",
					sdl=		"GenDllCacheManagerSDL"
					)
]
class CDllCacheManager :
	public IDllCacheMgr
{
#pragma warning(pop)
public:
	[ soap_method ]
	STDMETHOD(GetEntries)(DWORD dwCount, _DLL_CACHE_ENTRY *pEntries, DWORD *pdwCopied)
	{
		ATLASSERT(m_spDllCache);
		HRESULT hr = E_FAIL;
		if (!m_spDllCache)
			return E_UNEXPECTED;

		// assume there will never be enough entries in the dll cache
		// to overflow the stack
		DLL_CACHE_ENTRY *pe= new DLL_CACHE_ENTRY[dwCount];
		if (!pe)
			return E_OUTOFMEMORY;

		hr = m_spDllCache->GetEntries(dwCount, pe, pdwCopied);
		if (hr == S_OK && *pdwCopied != 0)
		{
			// SysAllocString our path strings
			for (DWORD i = 0; i<*pdwCopied; i++)
			{
				pEntries[i].hInstDll = (DWORD)(DWORD_PTR)pe[i].hInstDll;
				pEntries[i].dwRefs = pe[i].dwRefs;
				pEntries[i].szDllName = ::SysAllocString(CA2W(pe[i].szDllName));
			}
		}

		delete [] pe;
		return hr;
	}

	[ soap_method ]
	STDMETHOD(GetEntryCount)(DWORD *pdwCount)
	{
		ATLASSERT(m_spDllCache);
		if (!m_spDllCache)
			return E_UNEXPECTED;

		return m_spDllCache->GetEntries(0, NULL, pdwCount);
	}

	HTTP_CODE InitializeDCMgr(IServiceProvider *pProvider)
	{
		ATLASSERT(pProvider); // should never be NULL
		if (!pProvider)
			return HTTP_ERROR(500, ISE_SUBERR_UNEXPECTED);

		if (m_spDllCache)
			return HTTP_SUCCESS; // already initialized

		pProvider->QueryService(__uuidof(IDllCache), &m_spDllCache);
		return m_spDllCache ? HTTP_SUCCESS : HTTP_ERROR(500, ISE_SUBERR_UNEXPECTED);
	}

	HTTP_CODE HandleRequest(AtlServerRequest *pRequestInfo, IServiceProvider *pProvider)
	{
		HTTP_CODE hcErr = InitializeDCMgr(pProvider);
		if (hcErr != HTTP_SUCCESS)
			return hcErr;

__if_exists(_Authority)
{
		// Make sure caller is authorized on this system
		hcErr = HTTP_FAIL;
		ATLTRY(hcErr = _Authority.IsAuthorized(pRequestInfo, ATL_DEFAULT_AUTHGRP));
}
		if (hcErr == HTTP_SUCCESS)
		{
			hcErr = __super::HandleRequest(pRequestInfo, pProvider);
		}
		return hcErr;
	}

protected:
	CComPtr<IDllCache> m_spDllCache;
};

#define INVALID_INDEX -1

[
	request_handler(name="DllMgrSrf")
]
class CDllCacheMgr
{
	static CComObjectGlobal<CDllCacheManager> m_dllmgr;
	long m_nColor;
	
	int m_nEnumCount;
	int m_nEnumIndex;
	_DLL_CACHE_ENTRY *m_pEntries;
public:
	CDllCacheMgr() : m_nColor(ATL_COLOR_TR1),
		m_nEnumCount(INVALID_INDEX),
		m_nEnumIndex(INVALID_INDEX),
		m_pEntries(NULL)
	{

	}

	[tag_name("GetTRColor")]
	HTTP_CODE GetTRColor()
	{
		m_nColor = (m_nColor == ATL_COLOR_TR1) ? ATL_COLOR_TR2 : ATL_COLOR_TR1;
		TCHAR cr[8];
		if (RGBToHtml(m_nColor, cr, sizeof(cr)))
			m_HttpResponse << cr;
		
		return HTTP_SUCCESS;
	}

	[tag_name("GetBodyColor")]
	HTTP_CODE GetBodyColor()
	{
		TCHAR cr[8];
		if (RGBToHtml(ATL_COLOR_BODYBG, cr, sizeof(cr)))
			m_HttpResponse << cr;
		return HTTP_SUCCESS;
	}


	[tag_name("GetNumEntries")]
	HTTP_CODE GetNumEntries()
	{
		DWORD dwEntries = 0;
		m_dllmgr.GetEntryCount(&dwEntries);
		m_HttpResponse << dwEntries;
		return HTTP_SUCCESS;
	}


	[tag_name("EnumEntries")]
	HTTP_CODE EnumEntries()
	{
		// we lock the cache while we enum entries so no entries
		// will be removed during the enumeration request.
		if (m_nEnumIndex == INVALID_INDEX)
		{
			// set up for the iteration
			m_dllmgr.GetEntryCount((DWORD*)&m_nEnumCount);
			if (!m_nEnumCount)
				return HTTP_S_FALSE; // nothing to enum

			m_pEntries = new _DLL_CACHE_ENTRY[m_nEnumCount];
			if (!m_pEntries)
				return HTTP_ERROR(500, ISE_SUBERR_OUTOFMEM);

			DWORD dwFetched = INVALID_INDEX;

			if (S_OK != m_dllmgr.GetEntries(m_nEnumCount, m_pEntries, &dwFetched))
				return HTTP_ERROR(500, ISE_SUBERR_UNEXPECTED);

			m_nEnumIndex = 0;
			return HTTP_SUCCESS;
		}

		m_nEnumIndex++;
		if (m_nEnumIndex < m_nEnumCount)
			return HTTP_SUCCESS; // continue iterating

		else
		{
			// done, clean up
			for (int i = 0; i< m_nEnumCount; i++)
			{
				::SysFreeString(m_pEntries[i].szDllName);
			}
			delete [] m_pEntries;
			m_pEntries = NULL;
			m_nEnumCount = INVALID_INDEX;
			m_nEnumIndex = INVALID_INDEX;
			return HTTP_S_FALSE; // terminate iterations.
		}
	}

	[tag_name("GetDllName")]
	HTTP_CODE GetDllName()
	{
		m_HttpResponse << m_pEntries[m_nEnumIndex].szDllName;
		return HTTP_SUCCESS;
	}

	[tag_name("GetDllReferences")]
	HTTP_CODE GetDllReferences()
	{
		m_HttpResponse << m_pEntries[m_nEnumIndex].dwRefs;
		return HTTP_SUCCESS;
	}

	HTTP_CODE ValidateAndExchange()
	{

		HTTP_CODE hcErr = m_dllmgr.InitializeDCMgr(m_spServiceProvider);
		if (hcErr != HTTP_SUCCESS)
			return hcErr;

__if_exists(_Authority)
{
		// Make sure caller is authorized on this system
		hcErr = HTTP_FAIL;
		ATLTRY(hcErr = _Authority.IsAuthorized(m_pRequestInfo, ATL_DEFAULT_AUTHGRP));

		if (hcErr != HTTP_SUCCESS)
			return hcErr;
}
		hcErr = LoadStencilResource(m_hInstHandler, IDR_DLLMGR_SRF);
		m_HttpResponse.SetContentType("text/html");
		return hcErr;

	}
};

__declspec(selectany) CComObjectGlobal<CDllCacheManager> CDllCacheMgr::m_dllmgr;

#endif // _ATL_DLLCACHE_MANAGEMENT

}; // ATL
