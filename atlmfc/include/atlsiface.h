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
#include <windows.h>
#include <atlcoll.h>
#include <httpext.h>
#include <atlserr.h>

namespace ATL{

// Forward declarations of custom data types used in 
// interfaces declared in this file.
struct AtlServerRequest;
class CIsapiWorker;
class IAtlMemMgr;
class CCookie;

// Forward declarations of all interfaces declared in this file.
__interface IWriteStream;
__interface IHttpFile;
__interface IHttpServerContext;
__interface IHttpRequestLookup;
__interface IRequestHandler;
__interface ITagReplacer;
__interface IIsapiExtension;
__interface IPageCacheControl;
__interface IRequestStats;
__interface IBrowserCaps;
__interface IBrowserCapsSvc;


// ATLS Interface declarations.

// Interface for writing to a stream.
__interface IWriteStream
{
	HRESULT WriteStream(LPCSTR szOut, int nLen, DWORD *pdwWritten);
	HRESULT FlushStream();
};

// This is an interface that provides for basic accessor
// functionality for files (see CHttpRequestFile).
__interface IHttpFile
{
	LPCSTR GetParamName();
	LPCSTR GetFileName();
	LPCSTR GetFullFileName();
	LPCSTR GetContentType();
	LPCSTR GetTempFileName();
	ULONGLONG GetFileSize();
};

// This interface encapsulates the capabilities of the web server and provides information about
// the current request being handled. See CServerContext for implementation.
__interface ATL_NO_VTABLE __declspec(uuid("813F3F00-3881-11d3-977B-00C04F8EE25E")) 
	IHttpServerContext : public IUnknown
{
	HRESULT  STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppv);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	LPCSTR GetRequestMethod();
	LPCSTR GetQueryString();
	LPCSTR GetPathInfo();
	LPCSTR GetPathTranslated();
	LPCSTR GetScriptPathTranslated();
	DWORD GetTotalBytes();
	DWORD GetAvailableBytes();
	BYTE *GetAvailableData();
	LPCSTR GetContentType();
	BOOL GetServerVariable(LPCSTR pszVariableName,
									LPSTR pvBuffer, DWORD *pdwSize);
	BOOL GetImpersonationToken(HANDLE * pToken);
	BOOL WriteClient(void *pvBuffer, DWORD *pdwBytes);
	BOOL AsyncWriteClient(void *pvBuffer, DWORD *pdwBytes);
	BOOL ReadClient(void *pvBuffer, DWORD *pdwSize);
	BOOL AsyncReadClient(void *pvBuffer, DWORD *pdwSize);
	BOOL SendRedirectResponse(LPCSTR pszRedirectUrl);
	BOOL SendResponseHeader(LPCSTR pszHeader, LPCSTR pszStatusCode,
							BOOL fKeepConn);
	BOOL DoneWithSession(DWORD dwStatusCode);
	BOOL RequestIOCompletion(PFN_HSE_IO_COMPLETION pfn, DWORD *pdwContext);
	BOOL TransmitFile(HANDLE hFile, PFN_HSE_IO_COMPLETION pfn, void *pContext, 
		LPCSTR szStatusCode, DWORD dwBytesToWrite, DWORD dwOffset,
		void *pvHead, DWORD dwHeadLen, void *pvTail,
		DWORD dwTailLen, DWORD dwFlags);
    BOOL AppendToLog(LPCSTR szMessage, DWORD* pdwLen);
	BOOL MapUrlToPathEx(LPCSTR szLogicalPath, DWORD dwLen, HSE_URL_MAPEX_INFO *pumInfo);
};

// This interface is designed to allow one map to chain to another map.
// The interface is implemented by the CHttpThunkMap and CHttpRequest classes.
// Pointers to this interface are passed around by CRequestHandlerT and CHtmlTagReplacer.
// dwType - the type of item being requested
__interface ATL_NO_VTABLE __declspec(uuid("A5990B44-FF74-4bfe-B66D-F9E7E9F42D42")) 
	IHttpRequestLookup : public IUnknown
//interface IHttpRequestLookup
{
	POSITION GetFirstQueryParam(LPCSTR *ppszName, LPCSTR *ppszValue) const;
	POSITION GetNextQueryParam(POSITION pos, LPCSTR *ppszName, LPCSTR *ppszValue) const;

	POSITION GetFirstFormVar(LPCSTR *ppszName, LPCSTR *ppszValue) const;
	POSITION GetNextFormVar(POSITION pos, LPCSTR *ppszName, LPCSTR *ppszValue) const;

	POSITION GetFirstFile(LPCSTR *ppszName, IHttpFile **ppFile) const;
	POSITION GetNextFile(POSITION pos, LPCSTR *ppszName, IHttpFile **ppFile) const;

	HRESULT GetServerContext(IHttpServerContext **ppOut);
};


__interface ATL_NO_VTABLE __declspec(uuid("D57F8D0C-751A-4223-92BC-0B29F65D2453")) 
IRequestHandler : public IUnknown
{
	HTTP_CODE GetFlags(DWORD *pdwStatus);
	HTTP_CODE InitializeHandler(AtlServerRequest *pRequestInfo, IServiceProvider *pProvider);
	HTTP_CODE InitializeChild(AtlServerRequest *pRequestInfo, IServiceProvider *pProvider, IHttpRequestLookup *pLookup);
	HTTP_CODE HandleRequest(AtlServerRequest *pRequestInfo, IServiceProvider *pProvider);
	void UninitializeHandler();
};

// This interface defines the methods necessary for server response file processing.
__interface ATL_NO_VTABLE __declspec(uuid("8FF5E90C-8CE0-43aa-96C4-3BF930837512")) 
	ITagReplacer : public IUnknown
{
public:
	HTTP_CODE FindReplacementOffset(LPCSTR szMethodName, DWORD *pdwMethodOffset, 
						LPCSTR szObjectName, DWORD *pdwObjOffset, DWORD *pdwMap, void **ppvParam, IAtlMemMgr *pMemMgr);
	HTTP_CODE RenderReplacement(DWORD dwFnOffset, DWORD dwObjOffset, DWORD dwMap, void *pvParam);
	HRESULT GetContext(REFIID riid, void** ppv);
	HTTP_CODE FreeParam(void *pvParam);
	IWriteStream *SetStream(IWriteStream *pStream);
};

__interface __declspec(uuid("79DD4A27-D820-4fa6-954D-E1DFC2C05978"))
	IIsapiExtension : public IUnknown
{
public:
	BOOL DispatchStencilCall(AtlServerRequest *pRequestInfo);
	void RequestComplete(AtlServerRequest *pRequestInfo, DWORD hStatus, DWORD dwSubStatus);
	BOOL OnThreadAttach();
	void OnThreadTerminate();
    BOOL QueueRequest(AtlServerRequest *pRequestInfo);
	CIsapiWorker *GetThreadWorker();
	BOOL SetThreadWorker(CIsapiWorker *pWorker);
	HTTP_CODE LoadRequestHandler(LPCSTR szDllPath, LPCSTR szHandlerName, IHttpServerContext *pServerContext,
		HINSTANCE *phInstance, IRequestHandler **ppHandler);
	HRESULT AddService(REFGUID guidService, REFIID riid, IUnknown *punk, HINSTANCE hInstance);
	HRESULT RemoveService(REFGUID guidService, REFIID riid);
	HTTP_CODE LoadDispatchFile(LPCSTR szFileName, AtlServerRequest *pRequestInfo);

	AtlServerRequest* CreateRequest();
	void FreeRequest(AtlServerRequest* pRequest);
	HTTP_CODE TransferRequest(
				AtlServerRequest *pRequest, 
				IServiceProvider *pServiceProvider,
				IWriteStream *pWriteStream,
				IHttpRequestLookup *pLookup,
				LPCSTR szNewUrl,
				WORD nCodePage,
				bool bContinueAfterProcess = false,
				void *pState = NULL
			);
};

// This interface controls the cacheability of the current page
__interface ATL_NO_VTABLE __declspec(uuid("9868BFC0-D44D-4154-931C-D186EC0C45D5")) 
	IPageCacheControl : public IUnknown
{
    HRESULT GetExpiration(FILETIME *pftExpiration);
    HRESULT SetExpiration(FILETIME ftExpiration);
    BOOL IsCached();
    BOOL Cache(BOOL bCache);
};

__interface ATL_NO_VTABLE __declspec(uuid("2B75C68D-0DDF-48d6-B58A-CC7C2387A6F2"))
	IRequestStats : public IUnknown
{
	long GetTotalRequests();
	long GetFailedRequests();
	long GetAvgResponseTime();
	long GetCurrWaiting();
	long GetMaxWaiting();
	long GetActiveThreads();
};

__interface __declspec(uuid("3339FCE2-99BC-4985-A702-4ABC8304A995"))
	IBrowserCaps : public IUnknown
{
public:
    HRESULT GetPropertyString(BSTR bstrProperty, BSTR * pbstrOut);
    BOOL GetBooleanPropertyValue(BSTR bstrProperty);
    HRESULT GetBrowserName(BSTR * pbstrName);
    HRESULT GetPlatform(BSTR * pbstrPlatform);
	HRESULT GetVersion(BSTR * pbstrVersion);
	HRESULT GetMajorVer(BSTR * pbstrMajorVer);
	HRESULT GetMinorVer(BSTR * pbstrMinorVer);
	BOOL SupportsFrames();
	BOOL SupportsTables();
	BOOL SupportsCookies();
	BOOL SupportsBackgroundSounds();
	BOOL SupportsVBScript();
	BOOL SupportsJavascript();
	BOOL SupportsJavaapplets();
	BOOL SupportsActiveXControls();
	BOOL SupportsCDF();
	BOOL SupportsAuthenticodeUpdate();
	BOOL IsBeta();
	BOOL IsCrawler();
	BOOL IsAOL();
	BOOL IsWin16();
	BOOL IsAK();
	BOOL IsSK();
	BOOL IsUpdate();        
};

__interface __declspec(uuid("391E7418-863B-430e-81BB-1312ED2FF3E9"))
	IBrowserCapsSvc : public IUnknown
{
public:
    HRESULT Initialize(HINSTANCE hInstance);
    HRESULT GetCaps(IHttpServerContext * pContext, IBrowserCaps ** ppOut);
    HRESULT GetCapsUserAgent(BSTR bstrAgent, IBrowserCaps ** ppOut);
    HRESULT Shutdown();
};
}; // namespace ATL