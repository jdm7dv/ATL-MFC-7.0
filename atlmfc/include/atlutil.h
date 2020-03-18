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
#include <stdio.h>
#include <string.h>
#include <crtdbg.h>
#include <stdlib.h>
#include <mbstring.h>
#include <atldef.h>
#include <imagehlp.h>
#include <atlbase.h>
#include <atlstr.h>
#include <atlcoll.h>

#pragma warning( push )
#pragma warning( disable: 4127 )

namespace ATL {


inline BOOL IsFullPath(LPCTSTR szPath)
{
	size_t nLen = _tcslen(szPath);
	if (nLen <= 1)
		return FALSE;
	if (szPath[1]==':')		// drive: case
		return TRUE;
	if (nLen > 2 && szPath[0]=='\\' &&
		szPath[1]=='\\')	// unc path name
		return TRUE;
	return FALSE;
}

inline BOOL IsFullPathA(LPCSTR szPath)
{
	DWORD nLen = (DWORD) strlen(szPath);
	if (nLen <= 1)
		return FALSE;
	if (szPath[1]==':')		// drive: case
		return TRUE;
	if (nLen > 2 && szPath[0]=='\\' &&
		szPath[1]=='\\')	// unc path name
		return TRUE;
	return FALSE;
}

#ifndef ATL_ISAPI_BUFFER_SIZE
#define ATL_ISAPI_BUFFER_SIZE 4096
#endif

//typedefs and defines for CUrl (essentially the same as the ones from wininet, but with an ATL_ prepended)
typedef WORD ATL_URL_PORT;

typedef enum {
	ATL_URL_SCHEME_UNKNOWN = -1, 
    ATL_URL_SCHEME_FTP     = 0,
    ATL_URL_SCHEME_GOPHER  = 1,
    ATL_URL_SCHEME_HTTP    = 2,
    ATL_URL_SCHEME_HTTPS   = 3,
    ATL_URL_SCHEME_FILE    = 4,
    ATL_URL_SCHEME_NEWS    = 5,
    ATL_URL_SCHEME_MAILTO  = 6,
    ATL_URL_SCHEME_SOCKS   = 7,
} ATL_URL_SCHEME;


#define ATL_URL_MAX_HOST_NAME_LENGTH   256
#define ATL_URL_MAX_USER_NAME_LENGTH   128
#define ATL_URL_MAX_PASSWORD_LENGTH    128
#define ATL_URL_MAX_PORT_NUMBER_LENGTH 5           // ATL_URL_PORT is unsigned short
#define ATL_URL_MAX_PORT_NUMBER_VALUE  65535       // maximum unsigned short value
#define ATL_URL_MAX_PATH_LENGTH        2048
#define ATL_URL_MAX_SCHEME_LENGTH      32          // longest protocol name length
#define ATL_URL_MAX_URL_LENGTH         (ATL_URL_MAX_SCHEME_LENGTH \
                                       + sizeof("://") \
                                       + ATL_URL_MAX_PATH_LENGTH)

#define ATL_URL_INVALID_PORT_NUMBER    0           // use the protocol-specific default

#define ATL_URL_DEFAULT_FTP_PORT       21          // default for FTP servers
#define ATL_URL_DEFAULT_GOPHER_PORT    70          //    "     "  gopher "
#define ATL_URL_DEFAULT_HTTP_PORT      80          //    "     "  HTTP   "
#define ATL_URL_DEFAULT_HTTPS_PORT     443         //    "     "  HTTPS  "
#define ATL_URL_DEFAULT_SOCKS_PORT     1080        // default for SOCKS firewall servers.


template <DWORD dwSize=ATL_ISAPI_BUFFER_SIZE>
class CAtlIsapiBuffer
{
protected:
	char m_szBuffer[dwSize];
	LPSTR m_pBuffer;
	DWORD m_dwLen;
	DWORD m_dwAlloc;
    HANDLE m_hProcHeap;

public:
	CAtlIsapiBuffer() throw()
	{
		if (dwSize > 0)
			m_szBuffer[0] = 0;

		m_pBuffer = m_szBuffer;
		m_dwLen = 0;
		m_dwAlloc = dwSize;
        m_hProcHeap = GetProcessHeap();
	}

	CAtlIsapiBuffer(LPCSTR sz) throw()
	{
		m_pBuffer = m_szBuffer;
		m_dwLen = 0;
		m_dwAlloc = dwSize;
        m_hProcHeap = GetProcessHeap();

		Append(sz);
	}

	~CAtlIsapiBuffer() throw()
	{
		Free();
	}

	BOOL Alloc(DWORD dwSize) throw()
	{
		if (m_dwAlloc >= dwSize)
		{
			return TRUE;
		}
		if (m_pBuffer != m_szBuffer)
		{
			HeapFree(m_hProcHeap, 0, m_pBuffer);
			m_dwLen = 0;
			m_dwAlloc = 0;
		}
		m_pBuffer = (LPSTR)HeapAlloc(m_hProcHeap, 0, dwSize);
		if (m_pBuffer)
		{
			m_dwAlloc = dwSize;
			return TRUE;
		}
		return FALSE;
	}

	BOOL ReAlloc(DWORD dwNewSize) throw()
	{
		if (dwNewSize <= m_dwAlloc)
			return TRUE;

		if (m_pBuffer == m_szBuffer)
		{
			BOOL bRet = Alloc(dwNewSize);
			if (bRet)
				memcpy(m_pBuffer, m_szBuffer, m_dwLen);
			return bRet;
		}

		LPSTR pvNew = (LPSTR )HeapReAlloc(m_hProcHeap, 0, m_pBuffer, dwNewSize);
		if (pvNew)
		{
			m_pBuffer = pvNew;
			m_dwAlloc = dwNewSize;
			return TRUE;
		}
		return FALSE;
	}

	void Free() throw()
	{
		if (m_pBuffer != m_szBuffer)
		{
			HeapFree(m_hProcHeap,0 , m_pBuffer);
			m_dwAlloc = dwSize;
			m_pBuffer = m_szBuffer;
		}
		Empty();
	}

	void Empty() throw()
	{
		if (m_pBuffer)
		{
			m_pBuffer[0]=0;
			m_dwLen  = 0;
		}
	}

	DWORD GetLength() throw()
	{
		return m_dwLen;
	}

	BOOL Append(LPCSTR sz, int nLen = -1) throw()
	{
		if (nLen == -1)
			nLen = (int) strlen(sz);

		if (m_dwLen + nLen + 1 > m_dwAlloc)
		{
			if (!ReAlloc(m_dwAlloc + (nLen+1 > 4096 ? nLen+1 : 4096)))
				return FALSE;
		}
		memcpy(m_pBuffer + m_dwLen, sz, nLen);
		m_dwLen += nLen;
		m_pBuffer[m_dwLen]=0;
//		ATLASSERT(strlen(m_pBuffer)==m_dwLen);
		return TRUE;
	}

	operator LPCSTR() throw()
	{
		return (LPCSTR)m_pBuffer;
	}

	CAtlIsapiBuffer& operator+=(LPCSTR sz) throw()
	{
		Append(sz);
		return *this;
	}
}; // class CAtlIsapiBuffer

#ifdef _M_IX86
#define STACK_TRACE_PART_DELIMITER ';'
#define STACK_TRACE_LINE_DELIMITER '~'

static DWORD_PTR __stdcall GetModuleBase(HANDLE hProcess, DWORD_PTR dwReturnAddress, CString& str) throw()
{	
	MEMORY_BASIC_INFORMATION memoryBasicInfo;
	TCHAR szFile[512] = {0};
	if (VirtualQueryEx(hProcess, (LPVOID)dwReturnAddress, &memoryBasicInfo, sizeof(memoryBasicInfo)))
	{
		DWORD cch = 0;
		cch = GetModuleFileName((HINSTANCE)memoryBasicInfo.AllocationBase, szFile, 512);
// REVIEW (take this pragma out when merging StackTrace code)
#pragma warning(push)
#pragma warning(disable:4244)
		if (!SymLoadModule(hProcess, NULL, ((cch) ? CT2AEX<MAX_PATH+1>(szFile) : NULL), NULL, (DWORD_PTR) memoryBasicInfo.AllocationBase, 0))
		{
#pragma warning(pop)
			str += "Load Failed: ";
			str += szFile;
			return 0;
		}
		if (cch)
		{
			TCHAR* pszFile = szFile;
			TCHAR* pszModule = pszFile;

			while (*pszFile)
			{
				if (*pszFile == '\\')
					pszModule = pszFile+1;
				else if (*pszFile == '.')
					*pszFile = 0;
				pszFile++;
			}
			*pszFile = 0;

			str += pszModule;
			return (DWORD_PTR)memoryBasicInfo.AllocationBase;
		}
	}
	str += "<No Info>";
	
	return 0;
}

static BOOL StackTrace(HANDLE hProcess, HANDLE hThread, CString& strOut) throw()
{	
	if (!SymInitialize(hProcess, 0, TRUE))
	{
		return FALSE;
	}
	SymSetOptions(SymGetOptions() | SYMOPT_UNDNAME | SYMOPT_CASE_INSENSITIVE);

	CONTEXT threadContext;
	threadContext.ContextFlags = CONTEXT_FULL;
	if (!GetThreadContext(hThread, &threadContext))
	{
		return FALSE;
	}
	STACKFRAME stackFrame;
	memset(&stackFrame, 0, sizeof(stackFrame));
	stackFrame.AddrPC.Mode = AddrModeFlat;

	DWORD dwMachType;

	dwMachType = IMAGE_FILE_MACHINE_I386;

	stackFrame.AddrPC.Offset = threadContext.Eip;
	stackFrame.AddrPC.Mode = AddrModeFlat;
	stackFrame.AddrStack.Offset = threadContext.Esp;
	stackFrame.AddrStack.Mode = AddrModeFlat;
	stackFrame.AddrFrame.Offset = threadContext.Ebp;
	stackFrame.AddrFrame.Mode = AddrModeFlat;

	BYTE symbolBuffer[sizeof(IMAGEHLP_SYMBOL)+1025];
	PIMAGEHLP_SYMBOL pSymbol = (PIMAGEHLP_SYMBOL)symbolBuffer;
	pSymbol->SizeOfStruct = sizeof(symbolBuffer);
	pSymbol->MaxNameLength = 1024;

	IMAGEHLP_MODULE moduleInfo;
	moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE);
				
	DWORD dwDisplacement = 0;
	TCHAR szBuf[100];


	for (int i=0; i<1024; i++)
	{
		if (!StackWalk(dwMachType, hProcess, hThread, 
			&stackFrame, &threadContext, NULL, SymFunctionTableAccess, 
			SymGetModuleBase, NULL))
		{
			break;
		}

		//A very basic check to see if StackWalk is doing the right thing
		if (stackFrame.AddrPC.Offset == 0)
		{
			break;
		}

		//Get the address (in hex)
		_stprintf(szBuf, _T("0x%.8x;"), stackFrame.AddrPC.Offset);
		strOut += szBuf;

		//Get the Module name
		if (SymGetModuleInfo(hProcess, stackFrame.AddrPC.Offset, &moduleInfo))
		{
			strOut += moduleInfo.ModuleName;
		}
		else
		{
			//If the basic function fails, try to load the module manually
			GetModuleBase(hProcess, stackFrame.AddrPC.Offset, strOut);
		}
		strOut += STACK_TRACE_PART_DELIMITER;
		//Get the Symbol
		if (SymGetSymFromAddr(hProcess, stackFrame.AddrPC.Offset, &dwDisplacement,  pSymbol))
		{
			strOut += CA2CT(pSymbol->Name);
		}
		else
		{
			strOut += _T("<No Info>");
		}
		strOut += STACK_TRACE_PART_DELIMITER;
		strOut += STACK_TRACE_LINE_DELIMITER;
		stackFrame.AddrPC.Offset = 0;
		dwDisplacement = 0;
	}

	strOut += '\0';
	return TRUE;
}


#define PIPE_INPUT_BUFFER_SIZE  4096
#define PIPE_OUTPUT_BUFFER_SIZE 2048

enum { DEBUG_SERVER_MESSAGE_TRACE, DEBUG_SERVER_MESSAGE_ASSERT, DEBUG_SERVER_MESSAGE_QUIT };

struct DEBUG_SERVER_MESSAGE
{
	DWORD dwType;		// one of DEBUG_SERVER_MESSAGE_*
	DWORD dwProcessId;	// process id of client
	size_t dwTextLen;	// length of text message including null terminator
	BOOL bIsDebuggerAttached;	// TRUE if the debugger is already attached
};

#ifdef _DEBUG
#ifndef _ATL_NO_DEFAULT_LIBS

#pragma comment(lib, "imagehlp.lib")

#endif  // !_ATL_NO_DEFAULT_LIBS

extern "C" WINBASEAPI
BOOL
WINAPI
IsDebuggerPresent(
    VOID
    );


class CDebugReportHook
{
protected:
	_CRT_REPORT_HOOK m_pfnOldHook;

public:
	CDebugReportHook() throw()
	{
		SetHook();
	}

	~CDebugReportHook() throw()
	{
		RemoveHook();
	}

	void SetHook() throw()
	{
		m_pfnOldHook = _CrtSetReportHook(CDebugReportHookProc);
	}

	void RemoveHook() throw()
	{
		_CrtSetReportHook(m_pfnOldHook);
	}

	static int __cdecl CDebugReportHookProc(int reportType, char *message, int *returnValue) throw()
	{
		DWORD dwWritten;
		
		*returnValue = 0;

		CHandle hdlPipe;
		while (1)
		{
			char* lpszBuf = "\\\\.\\pipe\\BltPipe";
			HANDLE hPipe = CreateFileA(lpszBuf, GENERIC_WRITE | GENERIC_READ,
				FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
			
			if (hPipe != INVALID_HANDLE_VALUE )
			{
				hdlPipe.Attach(hPipe);
				break;
			}

			if (GetLastError() != ERROR_PIPE_BUSY)
			{
				if (reportType == _CRT_ASSERT)
					return TRUE;
				return FALSE;
			}

			//If the pipe is busy, we wait for up to 20 seconds
			if (!WaitNamedPipeA(lpszBuf, 20000)) 
			{
				if (reportType == _CRT_ASSERT)
					return TRUE;
				return FALSE;
			}
		}

		DEBUG_SERVER_MESSAGE Message;

		Message.bIsDebuggerAttached = IsDebuggerPresent();

		if (reportType == _CRT_ASSERT)
		{
			Message.dwType = DEBUG_SERVER_MESSAGE_ASSERT;
		}
		else
		{
			Message.dwType = DEBUG_SERVER_MESSAGE_TRACE;
		}

		Message.dwProcessId = GetCurrentProcessId();
		Message.dwTextLen = strlen(message)+1;

		HANDLE hThread = GetCurrentThread();
		HANDLE hProcess = GetCurrentProcess();

		if (hThread == NULL || hProcess == NULL)
		{
			return (reportType == _CRT_ASSERT ? TRUE : FALSE);
		}

		int nRet = 1;

		WriteFile(hdlPipe, &Message, sizeof(DEBUG_SERVER_MESSAGE), &dwWritten, NULL);

		WriteFile(hdlPipe, message, (DWORD)Message.dwTextLen, &dwWritten, NULL);

		//Check to see whether or not to send stack trace
		ReadFile(hdlPipe, &nRet, sizeof(nRet), &dwWritten, NULL);

		//if nRet == 1, the user wants stack trace info
		if (nRet)
		{
			//NOTE: Sending the call stack over the pipe may need some fine tuning

			CString str;
			if (StackTrace(hProcess, hThread, str))
			{
				if (!WriteFile(hdlPipe, (LPCTSTR)str, str.GetLength(), &dwWritten, NULL))
				{
					return (reportType == _CRT_ASSERT ? TRUE : FALSE);
				}
			}
			else
			{
				return (reportType == _CRT_ASSERT ? TRUE : FALSE);
			}
		}

		ReadFile(hdlPipe, &nRet, sizeof(nRet), &dwWritten, NULL);

		// possible return values
		// 0 -> Ignore or cancel
		// 1 -> Retry
		// 2 -> Abort
		if (nRet == 0)
		{
			return (reportType == _CRT_ASSERT ? TRUE : FALSE);
		}
		if (nRet == 1)
		{
			if (IsDebuggerPresent())
			{
				DebugBreak();
			}
		}
		if (nRet == 2)
			abort();
		
		return (reportType == _CRT_ASSERT ? TRUE : FALSE);
	}
}; // class CDebugReportHook
#endif
#endif // _M_IX86

#ifndef ATL_POOL_NUM_THREADS
	#define ATL_POOL_NUM_THREADS 0
#endif

#ifndef ATL_POOL_STACK_SIZE
	#define ATL_POOL_STACK_SIZE 0
#endif

// {B1F64757-6E88-4fa2-8886-7848B0D7E660}
extern "C" __declspec(selectany) IID IID_IThreadPoolConfig = { 0xb1f64757, 0x6e88, 0x4fa2, { 0x88, 0x86, 0x78, 0x48, 0xb0, 0xd7, 0xe6, 0x60 } };

interface ATL_NO_VTABLE __declspec(uuid("B1F64757-6E88-4fa2-8886-7848B0D7E660")) 
    IThreadPoolConfig : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE SetSize(int nNumThreads) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSize(int *pnNumThreads) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetTimeout(DWORD dwMaxWait) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTimeout(DWORD *pdwMaxWait) = 0;
};

#ifndef ATLS_DEFAULT_THREADSPERPROC
    #define ATLS_DEFAULT_THREADSPERPROC 2
#endif

#ifndef ATLS_DEFAULT_THREADPOOLSHUTDOWNTIMEOUT
    #define ATLS_DEFAULT_THREADPOOLSHUTDOWNTIMEOUT 36000
#endif

//
// CThreadPool
// This class is a simple IO completion port based thread pool
//	Worker:
//		is a class that is responsible for handling requests
//		queued on the thread pool.
//		It must have a typedef for RequestType, where request type
//		is the datatype to be queued on the pool
//		RequestType must be castable to (DWORD)
//		The value -1 is reserved for shutdown
//		of the pool
//		Worker must also have a void Execute(RequestType request, void *pvParam, OVERLAPPED *pOverlapped) function
//	ThreadTraits:
//		is a class that implements a static CreateThread function
//		This allows for overriding how the threads are created
#define ATLS_POOL_SHUTDOWN ((OVERLAPPED*) -1)
template <class Worker, class ThreadTraits=DefaultThreadTraits>
class CThreadPool : public IThreadPoolConfig
{
protected:

//    CAtlMap<DWORD, HANDLE> m_ThreadMap;
    CSimpleMap<DWORD, HANDLE> m_ThreadMap;

    DWORD m_dwThreadEventId;
    HANDLE m_hThreadEvent;

    CComCriticalSection m_critSec;
    DWORD m_dwStackSize;
    DWORD m_dwMaxWait;
	HANDLE m_hRequestQueue;
	void *m_pvWorkerParam;

public:
	
	CThreadPool() throw() :
		m_hRequestQueue(NULL),
		m_pvWorkerParam(NULL),
        m_dwMaxWait(ATLS_DEFAULT_THREADPOOLSHUTDOWNTIMEOUT)
	{
	}

	~CThreadPool() throw()
	{
		Shutdown();
	}
	
	// Initialize the thread pool
    // if nNumThreads > 0, then it specifies the number of threads
    // if nNumThreads < 0, then it specifies the number of threads per proc (-)
    // if nNumThreads == 0, then it defaults to two threads per proc
	// hCompletion is a handle of a file to associate with the completion port
	// pvWorkerParam is a parameter that will be passed to Worker::Execute
	//	dwStackSize:
	//		The stack size to use when creating the threads
	HRESULT Initialize(void *pvWorkerParam=NULL, int nNumThreads=0, DWORD dwStackSize=0, HANDLE hCompletion=INVALID_HANDLE_VALUE) throw()
	{
        if (m_hRequestQueue)   // Already initialized
            return AtlHresultFromWin32(ERROR_ALREADY_INITIALIZED);

        if (S_OK != m_critSec.Init())
            return E_FAIL;

        m_hThreadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (!m_hThreadEvent)
        {
            m_critSec.Term();
            return AtlHresultFromLastError();
        }

		// Create IO completion port to queue the requests
		m_hRequestQueue = CreateIoCompletionPort(hCompletion, NULL, 0, nNumThreads);
		if (m_hRequestQueue == NULL)
		{
			// failed creating the Io completion port
            m_critSec.Term();
            CloseHandle(m_hThreadEvent);
			return AtlHresultFromLastError();		
		}
        m_pvWorkerParam = pvWorkerParam;
        m_dwStackSize = dwStackSize;

        HRESULT hr = ResizePool(nNumThreads);
        if (hr != S_OK)
        {
		    // Close the request queue handle
		    CloseHandle(m_hRequestQueue);
		    
		    // Clear the queue handle
		    m_hRequestQueue = NULL;

            // Uninitialize the critical sections
            m_critSec.Term();
            CloseHandle(m_hThreadEvent);


            return hr;
        }

		return S_OK;
	}

    // Resize the thread pool
    HRESULT ResizePool(int nNumThreads) throw()
    {
        if (nNumThreads == 0)
            nNumThreads = -ATLS_DEFAULT_THREADSPERPROC;

		if (nNumThreads < 0)
		{
			SYSTEM_INFO si;
			GetSystemInfo(&si);
			nNumThreads = (int) (-nNumThreads) * si.dwNumberOfProcessors;
		}

        return InternalResizePool(nNumThreads, INFINITE);
    }

	// Shutdown the thread pool
	// This function posts the shutdown request to all the threads in the pool
	// It will wait for the threads to shutdown a maximum of dwMaxWait MS.
	// If the timeout expires it just returns without terminating the threads.
	void Shutdown(DWORD dwMaxWait=0) throw()
	{
        if (!m_hRequestQueue)   // Not initialized
            return;

        m_critSec.Lock();

		if (dwMaxWait == 0)
			dwMaxWait = m_dwMaxWait;

        HRESULT hr = InternalResizePool(0, dwMaxWait);

        if (hr != S_OK)
            ATLTRACE(atlTraceUtil, 0, _T("Thread pool not shutting down cleanly : %08x"), hr);
            // If the threads have not returned, then something is wrong

        for (int i = m_ThreadMap.GetSize() - 1; i >= 0; i--)
        {
            HANDLE hThread = m_ThreadMap.GetValueAt(i);
            DWORD dwExitCode;
            GetExitCodeThread(hThread, &dwExitCode);
            if (dwExitCode == STILL_ACTIVE)
            {
                ATLTRACE(atlTraceUtil, 0, _T("Terminating thread"));
                TerminateThread(hThread, 0);
            }
            CloseHandle(hThread);
        }

		// Close the request queue handle
		CloseHandle(m_hRequestQueue);
		
		// Clear the queue handle
		m_hRequestQueue = NULL;

        ATLASSERT(m_ThreadMap.GetSize() == 0);

        m_critSec.Unlock();

        // Uninitialize the critical sections
        m_critSec.Term();
        CloseHandle(m_hThreadEvent);

	}

    // IThreadPoolConfig methods
    HRESULT STDMETHODCALLTYPE SetSize(int nNumThreads) throw()
    {
        return ResizePool(nNumThreads);
    }

    HRESULT STDMETHODCALLTYPE GetSize(int *pnNumThreads) throw()
    {
		if (!pnNumThreads)
			return E_POINTER;

        *pnNumThreads = GetNumThreads();
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE SetTimeout(DWORD dwMaxWait) throw()
    {
        m_dwMaxWait = dwMaxWait;

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetTimeout(DWORD *pdwMaxWait) throw()
    {
        if (!pdwMaxWait)
            return E_POINTER;

        *pdwMaxWait = m_dwMaxWait;

        return S_OK;
    }

    // IUnknown methods
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppv) throw()
	{
		if (!ppv)
			return E_POINTER;

		if (InlineIsEqualGUID(riid, __uuidof(IUnknown)) ||
			InlineIsEqualGUID(riid, __uuidof(IThreadPoolConfig)))
		{
			*ppv = static_cast<IUnknown*>(this);
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}

	ULONG STDMETHODCALLTYPE AddRef() throw()
	{
		return 1;
	}
	
	ULONG STDMETHODCALLTYPE Release() throw()
	{
		return 1;
	}


	HANDLE GetQueueHandle() throw()
	{
		return m_hRequestQueue;
	}

	int GetNumThreads() throw()
	{
		return m_ThreadMap.GetSize();
	}

	// QueueRequest adds a request to the thread pool
	// it will be picked up by one of the threads and dispatched to the worker
	// in WorkerThreadProc
	BOOL QueueRequest(Worker::RequestType request) throw()
	{
		if (!PostQueuedCompletionStatus(m_hRequestQueue, 0, (ULONG_PTR) request, NULL))
			return FALSE;
		return TRUE;
	}
	
    DWORD ThreadProc() throw()
    {
		DWORD dwBytesTransfered;
		ULONG_PTR dwCompletionKey;

		OVERLAPPED* pOverlapped;

		// We instatiate only one instance of the worker class it on the stack
		// for the life time of the
		// thread.
		Worker theWorker;

		theWorker.Initialize(m_pvWorkerParam);
        SetEvent(m_hThreadEvent);
		// Get the request from the IO completion port
		while (GetQueuedCompletionStatus(m_hRequestQueue, &dwBytesTransfered, &dwCompletionKey, &pOverlapped, INFINITE))
		{
			if (pOverlapped == ATLS_POOL_SHUTDOWN) // Shut down
				break;
			else										// Do work
			{
				Worker::RequestType request = (Worker::RequestType) dwCompletionKey;

				// Process the request.  Notice the following:
				// (1) It is the worker's responsibility to free any memory assoicated
				// with the request if the request is complete
				// (2) If the request still requires some more processing
				// the worker should queue the request again for dispatching
				theWorker.Execute(request, m_pvWorkerParam, pOverlapped);
			}
		}
        m_dwThreadEventId = GetCurrentThreadId();
    	theWorker.Terminate(m_pvWorkerParam);
        SetEvent(m_hThreadEvent);
		return 0; 
    }

	static DWORD WINAPI WorkerThreadProc(LPVOID pv) throw()
	{
		CThreadPool<Worker, ThreadTraits>* pThis = 
			reinterpret_cast< CThreadPool<Worker, ThreadTraits>* >(pv); 

        return pThis->ThreadProc();
	} 

protected:
    HRESULT InternalResizePool(int nNumThreads, int dwMaxWait) throw()
    {
        if (!m_hRequestQueue)   // Not initialized
            return E_FAIL;

        m_critSec.Lock();

        int nCurThreads = m_ThreadMap.GetSize();
        if (nNumThreads == nCurThreads)
        {
            m_critSec.Unlock();
            return S_OK;
        }
        else if (nNumThreads < nCurThreads)
        {
            int nNumShutdownThreads = nCurThreads - nNumThreads;
            for (int nThreadIndex = 0; nThreadIndex < nNumShutdownThreads; nThreadIndex++)
            {
                ResetEvent(m_hThreadEvent);

                PostQueuedCompletionStatus(m_hRequestQueue, 0, 0, ATLS_POOL_SHUTDOWN);
                DWORD dwRet = WaitForSingleObject(m_hThreadEvent, dwMaxWait);

                if (dwRet == WAIT_TIMEOUT)
                {
                    m_critSec.Unlock();
                    return HRESULT_FROM_WIN32(WAIT_TIMEOUT);
                }
                else if (dwRet != WAIT_OBJECT_0)
                {
                    m_critSec.Unlock();
                    return AtlHresultFromLastError();
                }

                int nIndex = m_ThreadMap.FindKey(m_dwThreadEventId);
                if (nIndex != -1)
                {
                    HANDLE hThread = m_ThreadMap.GetValueAt(nIndex);
                    CloseHandle(hThread);
                    m_ThreadMap.RemoveAt(nIndex);
                }
            }
        }
        else
        {
            int nNumNewThreads = nNumThreads - nCurThreads;
		    // Create and initialize worker threads

		    for (int nThreadIndex = 0; nThreadIndex < nNumNewThreads; nThreadIndex++)
		    {
    		    DWORD dwThreadID;
                ResetEvent(m_hThreadEvent);
                HANDLE hThread = ThreadTraits::CreateThread(NULL, m_dwStackSize, WorkerThreadProc, (LPVOID)this, 0, &dwThreadID);
            
			    if (!hThread)
                {
					HRESULT hr = AtlHresultFromLastError();
					ATLASSERT(hr != S_OK);
                    m_critSec.Unlock();
				    return hr;
                }

                DWORD dwRet = WaitForSingleObject(m_hThreadEvent, dwMaxWait);
                if (dwRet != WAIT_OBJECT_0)
                {
                    if (dwRet == WAIT_TIMEOUT)
                    {
                        m_critSec.Unlock();
                        return HRESULT_FROM_WIN32(WAIT_TIMEOUT);
                    }
                    else
                    {
                        m_critSec.Unlock();
                        return AtlHresultFromLastError();
                    }
                }
                
                m_ThreadMap.Add(dwThreadID, hThread);
            }
        }
        m_critSec.Unlock();
        return S_OK;
    }

}; // class CThreadPool

//
// CNonStatelessWorker
// This class is a simple wrapper for use with CThreadPool.
//	It instantiates one instance of Worker per request
//	this allows Worker to hold state for each request
//	and depend on the destructor being called
//	Worker:
//		is a class that is responsible for handling requests
//		queued on the thread pool (See CThreadPool)
template <class Worker>
class CNonStatelessWorker
{
public:
	typedef Worker::RequestType RequestType;

	BOOL Initialize(void * /*pvParam*/)
	{
		return TRUE;
	}
	
	void Execute(Worker::RequestType request, void *pvWorkerParam, OVERLAPPED *pOverlapped)
	{
		Worker worker;
		worker.Execute(request, pvWorkerParam, pOverlapped);
	}
	void Terminate(void* /*pvParam*/){}
}; // class CNonStatelessWorker


//Flags
#define ATL_URL_ESCAPE             1   // (un)escape URL characters
#define ATL_URL_NO_ENCODE          2   // Don't convert unsafe characters to escape sequence
#define ATL_URL_DECODE             4   // Convert %XX escape sequences to characters
#define ATL_URL_NO_META            8   // Don't convert .. etc. meta path sequences
#define ATL_URL_ENCODE_SPACES_ONLY 16  // Encode spaces only
#define ATL_URL_BROWSER_MODE       32  // Special encode/decode rules for browser
#define ATL_URL_ENCODE_PERCENT     64  // Encode percent (by default, not encoded)
#define ATL_URL_CANONICALIZE       128 // Internal: used by Canonicalize for AtlEscapeUrl: Cannot be set via SetFlags
#define ATL_URL_COMBINE            256 // Internal: Cannot be set via SetFlags


//Get the decimal value of a hexadecimal character
inline short AtlHexValue(TCHAR chIn)
{
	unsigned char ch = (unsigned char)chIn;
	if (ch >= '0' && ch <= '9')
		return (short)(ch - '0');
	if (ch >= 'A' && ch <= 'F')
		return (short)(ch - 'A' + 10);
	if (ch >= 'a' && ch <= 'f')
		return (short)(ch - 'a' + 10);
	return -1;
}


//Determine if the character is unsafe under the URI RFC document
inline BOOL AtlIsUnsafeUrlChar(TCHAR chIn) throw()
{
	unsigned char ch = (unsigned char)chIn;
	switch(ch)
	{
		case ';': case '\\': case '?': case '@': case '&':
		case '=': case '+': case '$': case ',': case ' ':
		case '<': case '>': case '#': case '%': case '\"':
		case '{': case '}': case '|':
		case '^': case '[': case ']': case '`':
			return TRUE;
		default:
		{
			if (ch < 32 || ch > 126)
				return TRUE;
			return FALSE;
		}
	}
}


//Get the default internet port for a particular scheme
inline ATL_URL_PORT AtlGetDefaultUrlPort(ATL_URL_SCHEME m_nScheme) throw()
{
	switch (m_nScheme)
	{
		case ATL_URL_SCHEME_FTP:
			return ATL_URL_DEFAULT_FTP_PORT;
		case ATL_URL_SCHEME_GOPHER:
			return ATL_URL_DEFAULT_GOPHER_PORT;
		case ATL_URL_SCHEME_HTTP:
			return ATL_URL_DEFAULT_HTTP_PORT;
		case ATL_URL_SCHEME_HTTPS:
			return ATL_URL_DEFAULT_HTTPS_PORT;
		case ATL_URL_SCHEME_SOCKS:
			return ATL_URL_DEFAULT_SOCKS_PORT;
		default:
			return ATL_URL_INVALID_PORT_NUMBER;
	}
}

//Escape a meta sequence with lpszOutUrl as the base url and lpszInUrl as the relative url
//i.e. lpszInUrl = ./* or ../*
ATL_NOINLINE inline BOOL AtlEscapeUrlMetaHelper(
	LPTSTR* lpszOutUrl,
	DWORD dwOutLen,
	LPTSTR* lpszInUrl,
	DWORD* dwLen, 
	DWORD dwFlags = 0,
	DWORD dwColonPos = ATL_URL_MAX_URL_LENGTH) throw()
{
	ATLASSERT(lpszOutUrl != NULL && lpszInUrl != NULL && dwLen != NULL);

	LPTSTR lpszOut = *lpszOutUrl;
	LPTSTR lpszIn = *lpszInUrl;
	DWORD dwUrlLen = dwOutLen;
	TCHAR chPrev = *(lpszOut-1);
	BOOL bRet = FALSE;

	//if the previous character is a directory delimiter
	if (chPrev == '/' || chPrev == '\\')
	{
		TCHAR chNext = *lpszIn;

		//if the next character is a directory delimiter
		if (chNext == '/' || chNext == '\\')
		{
			//the meta sequence is of the form /./*
			lpszIn++;
			bRet = TRUE;
		}
		else if (chNext == '.' && ((chNext = *(lpszIn+1)) == '/' || 
			chNext == '\\' || chNext == '\0'))
		{
			//otherwise if the meta sequence is of the form "/../"
			//skip the preceding "/"
			lpszOut--;

			//skip the ".." of the meta sequence
			lpszIn+= 2;
			DWORD dwOutPos = dwUrlLen-1;
			LPTSTR lpszTmp = lpszOut;

			//while we are not at the beginning of the base url
			while (dwOutPos)
			{
				lpszTmp--;
				dwOutPos--;

				//if it is a directory delimiter
				if (*lpszTmp == '/' || *lpszTmp == '\\')
				{
					//if we are canonicalizing the url and NOT combining it
					//and if we have encountered the ':' or we are at a position before the ':'
					if ((dwFlags & ATL_URL_CANONICALIZE) && ((dwFlags & ATL_URL_COMBINE) == 0) &&
						(dwColonPos && (dwOutPos <= dwColonPos+1)))
					{
						//NOTE: this is to match the way that InternetCanonicalizeUrl and 
						//      InternetCombineUrl handle this case
						break;
					}

					//otherwise, set the current output string position to right after the '/'
					lpszOut = lpszTmp+1;

					//update the length to match
					dwUrlLen = dwOutPos+1;
					bRet = TRUE;
					break;
				}
			}

			//if we could not properly escape the meta sequence
			if (dwUrlLen != dwOutPos+1)
			{
				//restore everything to its original value
				lpszIn-= 2;
				lpszOut++;
			}
			else
			{
				bRet = TRUE;
			}
		}
	}
	//update the strings
	*lpszOutUrl = lpszOut;
	*lpszInUrl = lpszIn;
	*dwLen = dwUrlLen;
	return bRet;
}

//Convert all unsafe characters in szStringIn to escape sequences
//lpszStringIn and lpszStringOut should be different strings
inline BOOL AtlEscapeUrl(
	LPCTSTR lpszStringIn,
	LPTSTR lpszStringOut,
	DWORD* pdwStrLen,
	DWORD dwMaxLength,
	DWORD dwFlags = 0) throw()
{
	ATLASSERT(lpszStringIn != NULL);
	ATLASSERT(lpszStringOut != NULL);

	TCHAR ch;
	DWORD dwLen = 0;
	BOOL bRet = TRUE;
	BOOL bSchemeFile = FALSE;
	DWORD dwColonPos = 0;
	DWORD dwFlagsInternal = dwFlags;
	while((ch = *lpszStringIn++) != '\0')
	{
		//if we are at the maximum length, set bRet to FALSE
		//this ensures no more data is written to lpszStringOut, but
		//the length of the string is still updated, so the user
		//knows how much space to allocate
		if (dwLen == dwMaxLength)
		{
			bRet = FALSE;
		}
		
		//Keep track of the first ':' position to match the weird way
		//InternetCanonicalizeUrl handles it
		if (ch == ':' && (dwFlagsInternal & ATL_URL_CANONICALIZE) && !dwColonPos)
		{
			_tcslwr(lpszStringOut-dwLen);
			dwColonPos = dwLen+1;

			if (dwLen == 4 && !_tcsncmp(_T("file"), (lpszStringOut-4), 4))
			{
				bSchemeFile = TRUE;
			}
		}
		else if (ch == '%' && (dwFlagsInternal & ATL_URL_DECODE))
		{
			//decode the escaped sequence
			ch = (TCHAR)(16*AtlHexValue(*lpszStringIn++));
			ch = (TCHAR)(ch+AtlHexValue(*lpszStringIn++));
		}
		else if ((ch == '?' || ch == '#') && (dwFlagsInternal & ATL_URL_BROWSER_MODE))
		{
			//ATL_URL_BROWSER mode does not encode after a '?' or a '#'
			dwFlagsInternal |= ATL_URL_NO_ENCODE;
		}

		if ((dwFlagsInternal & ATL_URL_CANONICALIZE) && (dwFlagsInternal & ATL_URL_NO_ENCODE)==0)
		{
			//canonicalize the '\' to '/'
			if (ch == '\\' && (dwColonPos || (dwFlagsInternal & ATL_URL_COMBINE)) && bRet)
			{
				//if the scheme is not file or it is file and the '\' is in "file:\\"
				//NOTE: This is to match the way InternetCanonicalizeUrl handles this case
				if (!bSchemeFile || (dwLen < 7))
				{
					ch = '/';
				}
			}
			else if (ch == '.' && dwLen > 0 && (dwFlagsInternal & ATL_URL_NO_META)==0)
			{
				//if we are escaping meta sequences, attempt to do so
				if (AtlEscapeUrlMetaHelper(&lpszStringOut, dwLen, (TCHAR**)(&lpszStringIn), &dwLen, dwFlagsInternal, dwColonPos))
					continue;
			}
		}

		//if we are encoding and it is an unsafe character
		if (AtlIsUnsafeUrlChar(ch) && (dwFlagsInternal & ATL_URL_NO_ENCODE)==0)
		{
			//if we are only encoding spaces, and ch is not a space or
			//if we are not encoding meta sequences and it is a dot or
			//if we not encoding percents and it is a percent
			if (((dwFlagsInternal & ATL_URL_ENCODE_SPACES_ONLY) && ch != ' ') ||
				((dwFlagsInternal & ATL_URL_NO_META) && ch == '.') ||
				(((dwFlagsInternal & ATL_URL_ENCODE_PERCENT) == 0) && ch == '%'))
			{
				//just output it without encoding
				if (bRet)
					*lpszStringOut++ = ch;
			}
			else 
			{
				//if there is not enough space for the escape sequence
				if (dwLen >= (dwMaxLength-3))
				{
					bRet = FALSE;
				}
				if (bRet)
				{
					//output the percent, followed by the hex value of the character
					*lpszStringOut++ = '%';
					_stprintf(lpszStringOut, _T("%.2X"), (unsigned char)(ch));
					lpszStringOut+= 2;
				}
				dwLen += 2;
			}
		}
		else //safe character
		{
			if (bRet)
				*lpszStringOut++ = ch;
		}
		dwLen++;
	}

	if ((dwFlags & ATL_URL_BROWSER_MODE)==0)
	{
		//trim trailing whitespace
		lpszStringOut--;
		while (1)
		{
			if (*lpszStringOut == ' ')
			{
				--lpszStringOut;
				continue;
			}
			if (!_tcsncmp(lpszStringOut-2, _T("%20"), 3))
			{
				lpszStringOut -= 3;
				continue;
			}
			break;
		}
		lpszStringOut++;
	}

	if (bRet)
		*lpszStringOut = '\0';

	if (pdwStrLen)
		*pdwStrLen = dwLen;
	return bRet;
}

inline BOOL AtlEscapeUrlA(
	LPCSTR lpszStringIn,
	LPSTR lpszStringOut,
	DWORD* pdwStrLen,
	DWORD dwMaxLength,
	DWORD dwFlags = 0) throw()
{
#ifndef UNICODE
	return AtlEscapeUrl(lpszStringIn, lpszStringOut, pdwStrLen, dwMaxLength, dwFlags);
#else
	LPWSTR szTemp = NULL;
	WCHAR szStatic[ATL_URL_MAX_URL_LENGTH];
	LPWSTR szDyn = NULL;
	if ((dwMaxLength*sizeof(WCHAR)) < ATL_URL_MAX_URL_LENGTH)
		szTemp = szStatic;
	else
	{
		szDyn = new WCHAR[dwMaxLength*sizeof(WCHAR)];
		if (!szDyn)
			return FALSE;
		szTemp = szDyn;
	}

	BOOL bRet = AtlEscapeUrl(CA2W(lpszStringIn), szTemp, pdwStrLen, dwMaxLength*sizeof(WCHAR), dwFlags);
	if (bRet)
		lstrcpyA(lpszStringOut, CW2A(szTemp));
	if (szDyn)
		delete [] szDyn;

	return bRet;
#endif
}


//Convert all escaped characters in szString to their real values
//lpszStringIn and lpszStringOut can be the same string	
inline BOOL AtlUnescapeUrl(
	LPCTSTR lpszStringIn,
	LPTSTR lpszStringOut,
	LPDWORD pdwStrLen,
	DWORD dwMaxLength) throw()
{
	ATLASSERT(lpszStringIn != NULL);
	ATLASSERT(lpszStringOut != NULL);

	int nValue = 0;
	TCHAR ch;
	DWORD dwLen = 0;
	BOOL bRet = TRUE;
	while ((ch = *lpszStringIn) != 0)
	{
		if (dwLen == dwMaxLength)
			bRet = FALSE;

		if (bRet)
		{
			if (ch == '%')
			{
				ch = *(++lpszStringIn);
				//currently assuming 2 hex values after '%'
				//as per the RFC 2396 document
				nValue = 16*AtlHexValue(ch);
				nValue+= AtlHexValue(*(++lpszStringIn));
				*lpszStringOut++ = (TCHAR) nValue;
			}
			else //non-escape character
			{
				if (bRet)
					*lpszStringOut++ = ch;
			}
		}
		dwLen++;
		lpszStringIn++;
	}

	if (bRet)
		*lpszStringOut = '\0';

	if (pdwStrLen)
		*pdwStrLen = dwLen;
	return TRUE;
}


inline BOOL AtlUnescapeUrlA(
	LPCSTR lpszStringIn, 
	LPSTR lpszStringOut,
	LPDWORD pdwStrLen,
	DWORD dwMaxLength) throw()
{
#ifndef UNICODE
	return AtlUnescapeUrl(lpszStringIn, lpszStringOut, pdwStrLen, dwMaxLength);
#else
	LPWSTR szTemp = NULL;
	WCHAR szStatic[ATL_URL_MAX_URL_LENGTH];
	LPWSTR szDyn = NULL;
	if ((dwMaxLength*sizeof(WCHAR)) < ATL_URL_MAX_URL_LENGTH)
		szTemp = szStatic;
	else
	{
		szDyn = new WCHAR[dwMaxLength*sizeof(WCHAR)];
		if (!szDyn)
			return FALSE;
		szTemp = szDyn;
	}

	BOOL bRet = AtlUnescapeUrl(CA2W(lpszStringIn), szTemp, pdwStrLen, dwMaxLength*sizeof(WCHAR));
	if (bRet)
		lstrcpyA(lpszStringOut, CW2A(szTemp));
	if (szDyn)
		delete [] szDyn;

	return bRet;
#endif
}

//Canonicalize a URL (same as InternetCanonicalizeUrl)
inline BOOL AtlCanonicalizeUrl(
	LPCTSTR lpszUrl,
	LPTSTR lpszCanonicalized,
	DWORD* dwMaxLength,
	DWORD dwFlags = 0) throw()
{
	ATLASSERT(lpszUrl && lpszCanonicalized && dwMaxLength);

	return AtlEscapeUrl(lpszUrl, lpszCanonicalized, dwMaxLength, *dwMaxLength, dwFlags | ATL_URL_CANONICALIZE);
}

//Combine a base and relative URL (same as InternetCombineUrl)
inline BOOL AtlCombineUrl(
	LPCTSTR lpszBaseUrl,
	LPCTSTR lpszRelativeUrl,
	LPTSTR lpszBuffer,
	DWORD* dwMaxLength,
	DWORD dwFlags = 0) throw()
{
	ATLASSERT(lpszBaseUrl && lpszRelativeUrl && lpszBuffer && dwMaxLength);

	TCHAR szCombined[2*ATL_URL_MAX_URL_LENGTH];
	_tcscpy(szCombined, lpszBaseUrl);
	_tcscat(szCombined, lpszRelativeUrl);
	DWORD dwLen = (DWORD) _tcslen(szCombined);
	if (dwLen >= *dwMaxLength)
	{
		*dwMaxLength = dwLen;
		return FALSE;
	}
	return AtlEscapeUrl(szCombined, lpszBuffer, dwMaxLength, *dwMaxLength, dwFlags | ATL_URL_COMBINE | ATL_URL_CANONICALIZE);
}

class CUrl;

typedef CUrl* LPURL;
typedef const LPURL LPCURL;

class CUrl
{
private:
	//scheme names cannot contain escape/unsafe characters
	TCHAR m_szScheme[ATL_URL_MAX_SCHEME_LENGTH+1];

	//host names cannot contain escape/unsafe characters
	TCHAR m_szHostName[ATL_URL_MAX_HOST_NAME_LENGTH+1];

	TCHAR m_szUserName[ATL_URL_MAX_USER_NAME_LENGTH+1];
	TCHAR m_szPassword[ATL_URL_MAX_PASSWORD_LENGTH+1];
	TCHAR m_szUrlPath[ATL_URL_MAX_PATH_LENGTH+1];
	TCHAR m_szExtraInfo[ATL_URL_MAX_PATH_LENGTH+1];

	ATL_URL_PORT m_nPortNumber;
	ATL_URL_SCHEME m_nScheme;

	DWORD m_dwSchemeNameLength;
	DWORD m_dwHostNameLength;
	DWORD m_dwUserNameLength;
	DWORD m_dwPasswordLength;
	DWORD m_dwUrlPathLength;
	DWORD m_dwExtraInfoLength;

public:
	//Empty constructor
	CUrl() throw()
	{
		InitFields();
		SetScheme(ATL_URL_SCHEME_HTTP);
	}

	//Copy constructor--maybe make private
	CUrl(const CUrl& urlThat) throw()
	{
		CopyFields(urlThat);
	}

	//Destructor (empty)
	~CUrl() throw()
	{
	}

	//Assignment operator--maybe make private
	const CUrl& operator=(const CUrl& urlThat)  throw()
	{
		CopyFields(urlThat);
		return (*this);
	}

	//Set the url
	BOOL CrackUrl(LPCTSTR lpszUrl, DWORD dwFlags = 0) throw()
	{
		ATLASSERT(lpszUrl != NULL);

		InitFields();
		BOOL bRet = FALSE;
		if (dwFlags & ATL_URL_DECODE)
		{
			//decode the url before parsing it
			TCHAR szDecodedUrl[ATL_URL_MAX_URL_LENGTH];
			DWORD dwLen;
			if (!AtlUnescapeUrl(lpszUrl, szDecodedUrl, &dwLen, ATL_URL_MAX_URL_LENGTH))
				return FALSE;
			bRet = Parse(szDecodedUrl);
		}
		else
		{
			bRet = Parse(lpszUrl);
		}
		if (bRet && (dwFlags & ATL_URL_ESCAPE))
		{
			AtlUnescapeUrl(m_szUserName, m_szUserName, &m_dwUserNameLength, ATL_URL_MAX_USER_NAME_LENGTH);
			AtlUnescapeUrl(m_szPassword, m_szPassword, &m_dwPasswordLength, ATL_URL_MAX_PASSWORD_LENGTH);
			AtlUnescapeUrl(m_szUrlPath, m_szUrlPath, &m_dwUrlPathLength, ATL_URL_MAX_PATH_LENGTH);
			AtlUnescapeUrl(m_szExtraInfo, m_szExtraInfo, &m_dwExtraInfoLength, ATL_URL_MAX_PATH_LENGTH);
		}
		return bRet;
	}

	inline BOOL CreateUrl(LPTSTR lpszUrl, DWORD* pdwMaxLength, DWORD dwFlags = 0) const throw()
	{
		ATLASSERT(lpszUrl != NULL);
		ATLASSERT(pdwMaxLength != NULL);

		//build URL: <scheme>://<user>:<pass>@<domain>:<port><path><extra>
		TCHAR szPortNumber[6];
		DWORD dwLength = *pdwMaxLength;
		*pdwMaxLength = GetUrlLength()+1;
		if (*pdwMaxLength > dwLength)
			return FALSE;
		_stprintf(szPortNumber, _T(":%d"), m_nPortNumber);
		LPTSTR lpszOutUrl = lpszUrl;
		*lpszUrl = '\0';

		if (*m_szScheme)
		{
			_tcsncpy(lpszUrl, m_szScheme, m_dwSchemeNameLength);
			lpszUrl += m_dwSchemeNameLength;
			*lpszUrl++ = ':';
			if (m_nScheme != ATL_URL_SCHEME_MAILTO)
			{
				*lpszUrl++ = '/';
				*lpszUrl++ = '/';
			}
		}
		
		if (*m_szUserName)
		{
			_tcsncpy(lpszUrl, m_szUserName, m_dwUserNameLength);
			lpszUrl += m_dwUserNameLength;
			if (*m_szPassword)
			{
				*lpszUrl++ = ':';
				_tcsncpy(lpszUrl, m_szPassword, m_dwPasswordLength);
				lpszUrl += m_dwPasswordLength;
			}
			*lpszUrl++ = '@';
		}

		if (*m_szHostName)
		{
			_tcsncpy(lpszUrl, m_szHostName, m_dwHostNameLength);
			lpszUrl += m_dwHostNameLength;
			if (m_nPortNumber != AtlGetDefaultUrlPort(m_nScheme))
			{
				DWORD dwPortLen = (DWORD) _tcslen(szPortNumber);
				_tcsncpy(lpszUrl, szPortNumber, dwPortLen);
				lpszUrl += dwPortLen;
			}
			if (*m_szUrlPath && *m_szUrlPath != '/' && *m_szUrlPath != '\\')
				*lpszUrl++ = '/';
		}

		if (*m_szUrlPath)
		{
			_tcsncpy(lpszUrl, m_szUrlPath, m_dwUrlPathLength);
			lpszUrl+= m_dwUrlPathLength;
		}

		if (*m_szExtraInfo)
		{
			_tcsncpy(lpszUrl, m_szExtraInfo, m_dwExtraInfoLength);
			lpszUrl += m_dwExtraInfoLength;
		}
		*lpszUrl = '\0';

		*pdwMaxLength--;
		
		if (dwFlags & ATL_URL_ESCAPE)
		{
			TCHAR szUrl[ATL_URL_MAX_URL_LENGTH];
			_tcsncpy(szUrl, lpszOutUrl, *pdwMaxLength+1);
			return AtlUnescapeUrl(szUrl, lpszOutUrl, pdwMaxLength, dwLength);
		}

		return TRUE;
	}

	inline void Clear() throw()
	{
		InitFields();
	}

	inline DWORD GetUrlLength() const throw()
	{
		//The conditionals in this method are related to the conditionals in the CreateUrl method
		//scheme + ':'
		DWORD dwUrlLength = m_dwSchemeNameLength+1;

		//i.e. "//"
		if (m_nScheme != ATL_URL_SCHEME_MAILTO)
			dwUrlLength += 2;  

		dwUrlLength += m_dwUserNameLength;

		//i.e. "username@"
		if (m_dwUserNameLength > 0)
			dwUrlLength += m_dwUserNameLength+1;

		//i.e. ":password"
		if (m_dwPasswordLength > 0)
			dwUrlLength += m_dwPasswordLength+1;
	
		dwUrlLength += m_dwHostNameLength;

		// will need to add an extra '/' in this case
		if (m_dwHostNameLength && m_dwUrlPathLength && *m_szUrlPath != '/' && *m_szUrlPath != '\\')
			dwUrlLength++;

		//i.e. ":xx" where "xx" is the port number
		if (m_nPortNumber != AtlGetDefaultUrlPort(m_nScheme))
		{
			TCHAR szPortTmp[6];
			dwUrlLength += _stprintf(szPortTmp, _T(":%d"), m_nPortNumber);
		}

		dwUrlLength += m_dwUrlPathLength + m_dwExtraInfoLength;

		return dwUrlLength;
	}

	//Get the Scheme Name (i.e. http, ftp, etc.)
	inline LPCTSTR GetSchemeName() const throw()
	{
		return m_szScheme;
	}

	//Get the Scheme Name length
	inline DWORD GetSchemeNameLength() const throw()
	{
		return m_dwSchemeNameLength;
	}

	//This method will incur the cost of 
	//validating the scheme and updating the scheme name
	inline BOOL SetSchemeName(LPCTSTR lpszSchm) throw()
	{
		ATLASSERT(lpszSchm != NULL);

		//keep nScheme and lpszScheme in synch and verify validity of scheme
		if (_tcsicmp(lpszSchm, _T("http")) == 0)
		{
			m_nScheme = ATL_URL_SCHEME_HTTP;
			m_dwSchemeNameLength = 4;
		}
		else if (!_tcsicmp(lpszSchm, _T("https")))
		{
			m_nScheme = ATL_URL_SCHEME_HTTPS;
			m_dwSchemeNameLength = 5;
		}
		else if (!_tcsicmp(lpszSchm, _T("ftp")))
		{
			m_nScheme = ATL_URL_SCHEME_FTP;
			m_dwSchemeNameLength = 3;
		}
		else if (!_tcsicmp(lpszSchm, _T("file")))
		{
			m_nScheme = ATL_URL_SCHEME_FILE;
			m_dwSchemeNameLength = 4;
		}
		else if (!_tcsicmp(lpszSchm, _T("news")))
		{
			m_nScheme = ATL_URL_SCHEME_NEWS;
			m_dwSchemeNameLength = 4;
		}
		else if (!_tcsicmp(lpszSchm, _T("mailto")))
		{
			m_nScheme = ATL_URL_SCHEME_MAILTO;
			m_dwSchemeNameLength = 6;
		}
		else if (!_tcsicmp(lpszSchm, _T("socks")))
		{
			m_nScheme = ATL_URL_SCHEME_SOCKS;
			m_dwSchemeNameLength = 5;
		}
		else if (!_tcsicmp(lpszSchm, _T("gopher")))
		{
			m_nScheme = ATL_URL_SCHEME_GOPHER;
			m_dwSchemeNameLength = 6;
		}
		else 
		{
			DWORD dwSchemeNameLength = (DWORD) _tcslen(lpszSchm);
			if (dwSchemeNameLength > ATL_URL_MAX_SCHEME_LENGTH)
				return FALSE;

			m_dwSchemeNameLength = dwSchemeNameLength; 
			m_nScheme = ATL_URL_SCHEME_UNKNOWN;
		}
		_tcsncpy(m_szScheme, lpszSchm, m_dwSchemeNameLength);
		m_szScheme[m_dwSchemeNameLength] = '\0';
		m_nPortNumber = (unsigned short)AtlGetDefaultUrlPort(m_nScheme);

		return TRUE;
	}

	inline BOOL SetScheme(ATL_URL_SCHEME nScheme) throw()
	{
		//keep nScheme and m_szScheme in sync and verify the validity of nScheme
		switch(nScheme)
		{
			case ATL_URL_SCHEME_HTTP:
				_tcsncpy(m_szScheme, _T("http"), 4);
				m_dwSchemeNameLength = 4;
				break;
			case ATL_URL_SCHEME_HTTPS:
				_tcsncpy(m_szScheme, _T("https"), 5);
				m_dwSchemeNameLength = 5;
				break;
			case ATL_URL_SCHEME_FTP:
				_tcsncpy(m_szScheme, _T("ftp"), 3);
				m_dwSchemeNameLength = 3;
				break;
			case ATL_URL_SCHEME_FILE:
				_tcsncpy(m_szScheme, _T("file"), 4);
				m_dwSchemeNameLength = 4;
				break;
			case ATL_URL_SCHEME_NEWS:
				_tcsncpy(m_szScheme, _T("news"), 4);
				m_dwSchemeNameLength = 4;
				break;
			case ATL_URL_SCHEME_MAILTO:
				_tcsncpy(m_szScheme, _T("mailto"), 6);
				m_dwSchemeNameLength = 6;
				break;
			case ATL_URL_SCHEME_SOCKS:
				_tcsncpy(m_szScheme, _T("socks"), 5);
				m_dwSchemeNameLength = 5;
				break;
			case ATL_URL_SCHEME_GOPHER:
				_tcsncpy(m_szScheme, _T("gopher"), 6);
				m_dwSchemeNameLength = 6;
				break;
			default:
				return FALSE;
		}
		m_szScheme[m_dwSchemeNameLength] = '\0';
		m_nScheme = nScheme;
		m_nPortNumber = (unsigned short)AtlGetDefaultUrlPort(m_nScheme);

		return TRUE;
	}

	inline ATL_URL_SCHEME GetScheme() const throw()
	{
		return m_nScheme;
	}

	//Get the host name
	inline LPCTSTR GetHostName() const throw()
	{
		return m_szHostName;
	}

	//Get the host name's length
	inline DWORD GetHostNameLength() const throw()
	{
		return m_dwHostNameLength;
	}

	//Set the Host name
	inline BOOL SetHostName(LPCTSTR lpszHost) throw()
	{
		ATLASSERT(lpszHost != NULL);

		DWORD dwLen = (DWORD) _tcslen(lpszHost);
		if (dwLen > ATL_URL_MAX_HOST_NAME_LENGTH)
			return FALSE;

		_tcsncpy(m_szHostName, lpszHost, dwLen+1);
		m_dwHostNameLength = dwLen;
		
		return TRUE;
	}

	//Get the port number in terms of ATL_URL_PORT
	inline ATL_URL_PORT GetPortNumber() const throw()
	{
		return m_nPortNumber;
	}

	//Set the port number in terms of ATL_URL_PORT
	inline BOOL SetPortNumber(ATL_URL_PORT nPrt) throw()
	{
		m_nPortNumber = nPrt;
		return TRUE;
	}

	//Get the user name
	inline LPCTSTR GetUserName() const throw()
	{
		return m_szUserName;
	}

	//Get the user name's length
	inline DWORD GetUserNameLength() const throw()
	{
		return m_dwUserNameLength;
	}

	//Set the user name
	inline BOOL SetUserName(LPCTSTR lpszUser) throw()
	{
		ATLASSERT(lpszUser != NULL);
		
		DWORD dwLen = (DWORD) _tcslen(lpszUser);
		if (dwLen > ATL_URL_MAX_USER_NAME_LENGTH)
			return FALSE;

		_tcsncpy(m_szUserName, lpszUser, dwLen+1);
		m_dwUserNameLength = dwLen;

		return TRUE;
	}

	//Get the password
	inline LPCTSTR GetPassword() const throw()
	{
		return m_szPassword;
	}

	//Get the password's length
	inline DWORD GetPasswordLength() const throw()
	{
		return m_dwPasswordLength;
	}

	//Set the password
	inline BOOL SetPassword(LPCTSTR lpszPass) throw()
	{
		ATLASSERT(lpszPass != NULL);

		if (*lpszPass && !*m_szUserName)
			return FALSE;

		DWORD dwLen = (DWORD) _tcslen(lpszPass);
		if (dwLen > ATL_URL_MAX_PASSWORD_LENGTH)
			return FALSE;

		_tcsncpy(m_szPassword, lpszPass, dwLen+1);
		m_dwPasswordLength = dwLen;

		return TRUE;
	}

	//Get the url path (everything after scheme and
	//before extra info)
	inline LPCTSTR GetUrlPath() const throw()
	{
		return m_szUrlPath;
	}

	//Get the url path's length
	inline DWORD GetUrlPathLength() const throw()
	{
		return m_dwUrlPathLength;
	}

	//Set the url path
	inline BOOL SetUrlPath(LPCTSTR lpszPath) throw()
	{
		ATLASSERT(lpszPath != NULL);

		DWORD dwLen = (DWORD) _tcslen(lpszPath);
		if (dwLen > ATL_URL_MAX_PATH_LENGTH)
			return FALSE;

		_tcsncpy(m_szUrlPath, lpszPath, dwLen+1);
		m_dwUrlPathLength = dwLen;
		
		return TRUE;
	}

	//Get extra info (i.e. ?something or #something)
	inline LPCTSTR GetExtraInfo() const throw()
	{
		return m_szExtraInfo;
	}

	//Get extra info's length
	inline DWORD GetExtraInfoLength() const throw()
	{
		return m_dwExtraInfoLength;
	}

	//Set extra info
	inline BOOL SetExtraInfo(LPCTSTR lpszInfo) throw()
	{
		ATLASSERT(lpszInfo != NULL);

		DWORD dwLen = (DWORD) _tcslen(lpszInfo);
		if (dwLen > ATL_URL_MAX_PATH_LENGTH)
			return FALSE;

		_tcsncpy(m_szExtraInfo, lpszInfo, dwLen+1);
		m_dwExtraInfoLength = dwLen;
		
		return TRUE;
	}
	
	//Insert Escape characters into URL
	inline BOOL Canonicalize(DWORD dwFlags = 0) throw()
	{
		_tcslwr(m_szScheme);
		TCHAR szTmp[ATL_URL_MAX_URL_LENGTH];
		_tcscpy(szTmp, m_szUserName);
		BOOL bRet = AtlEscapeUrl(szTmp, m_szUserName, &m_dwUserNameLength, ATL_URL_MAX_USER_NAME_LENGTH, dwFlags);
		if (bRet)
		{
			_tcscpy(szTmp, m_szPassword);
			bRet = AtlEscapeUrl(szTmp, m_szPassword, &m_dwPasswordLength, ATL_URL_MAX_PASSWORD_LENGTH, dwFlags);
		}
		if (bRet)
		{
			_tcscpy(szTmp, m_szHostName);
			bRet = AtlEscapeUrl(szTmp, m_szHostName, &m_dwHostNameLength, ATL_URL_MAX_HOST_NAME_LENGTH, dwFlags);
		}
		if (bRet)
		{
			_tcscpy(szTmp, m_szUrlPath);
			bRet = AtlEscapeUrl(szTmp, m_szUrlPath, &m_dwUrlPathLength, ATL_URL_MAX_PATH_LENGTH, dwFlags);
		}

		//in ATL_URL_BROWSER mode, the portion of the URL following the '?' or '#' is not encoded
		if (bRet && (dwFlags & ATL_URL_BROWSER_MODE) == 0)
		{
			_tcscpy(szTmp, m_szExtraInfo);
			bRet = AtlEscapeUrl(szTmp+1, m_szExtraInfo+1, &m_dwExtraInfoLength, ATL_URL_MAX_PATH_LENGTH-1, dwFlags);
			if (bRet)
				m_dwExtraInfoLength++;
		}

		return bRet;
	}

private:
	inline BOOL Parse(LPCTSTR lpszUrl) throw()
	{
		ATLASSERT(lpszUrl != NULL);

		TCHAR ch;
		BOOL bGotScheme = FALSE;
		BOOL bGotUserName = FALSE;
		BOOL bGotHostName = FALSE;
		BOOL bGotPortNumber = FALSE;
		TCHAR szCurrentUrl[ATL_URL_MAX_URL_LENGTH+6];
		TCHAR* pszCurrentUrl = szCurrentUrl;

		//parse lpszUrl using szCurrentUrl to store temporary data
		
		//this loop will get the following if it exists:
		//<protocol>://user:pass@server:port
		while ((ch = *lpszUrl) != '\0')
		{
			if (ch == ':')
			{
				//3 cases:
				//(1) Just encountered a scheme
				//(2) Port number follows
				//(3) Form of username:password@

				// Check to see if we've just encountered a scheme
				*pszCurrentUrl = '\0';
				if (!bGotScheme)
				{
					if (!SetSchemeName(szCurrentUrl))
						goto error;

					//Set a flag to avoid checking for
					//schemes everytime we encounter a :
					bGotScheme = TRUE;

					if (*(lpszUrl+1) == '/')
					{
						if (*(lpszUrl+2) == '/')
						{
							//the mailto scheme cannot have a '/' following the "mailto:" portion
							if (bGotScheme && m_nScheme == ATL_URL_SCHEME_MAILTO)
								goto error;

							//Skip these characters and continue
							lpszUrl+= 2;
						}
						else 
						{
							//it is an absolute path
							//no domain name, port, username, or password is allowed in this case
							//break to loop that gets path
							lpszUrl++;
							pszCurrentUrl = szCurrentUrl;
							break;
						}
					}

					//reset pszCurrentUrl
					pszCurrentUrl = szCurrentUrl;
					lpszUrl++;

					//if the scheme is file, skip to getting the path information
					if (m_nScheme == ATL_URL_SCHEME_FILE)
						break;
					continue;
				}
				else if (!bGotUserName || !bGotPortNumber)
				{
					//It must be a username:password or a port number
					*pszCurrentUrl = '\0';

					pszCurrentUrl = szCurrentUrl;
					TCHAR tmpBuf[ATL_URL_MAX_PASSWORD_LENGTH];
					TCHAR* pTmpBuf = tmpBuf;
					int j = 0;

					//get the user or portnumber (break on either '/', '@', or '\0'
					while (((ch = *(++lpszUrl)) != '/') && (ch != '@') && (ch != '\0'))
					{
						if (j >= ATL_URL_MAX_PASSWORD_LENGTH)
							goto error;
 
						*pTmpBuf++ = ch;
						j++;
					}
					*pTmpBuf = '\0';

					//if we broke on a '/' or a '\0', it must be a port number
					if (!bGotPortNumber && (ch == '/' || ch == '\0'))
					{
						//the host name must immediately preced the port number
						if (!SetHostName(szCurrentUrl))
							goto error;

						//get the port number
						m_nPortNumber = (ATL_URL_PORT) _ttoi(tmpBuf);
						if (m_nPortNumber < 0)
							goto error;

						bGotPortNumber = bGotHostName = TRUE;
					}
					else if (!bGotUserName && ch=='@')
					{
						//otherwise it must be a username:password
						if (!SetUserName(szCurrentUrl) || !SetPassword(tmpBuf))
							goto error;

						bGotUserName = TRUE;
						lpszUrl++;
					}
					else
					{
						goto error;
					}
				}
			}
			else if (ch == '@')
			{
				if (bGotUserName)
					goto error;
				
				//type is userinfo@
				*pszCurrentUrl = '\0';
				if (!SetUserName(szCurrentUrl))
					goto error;

				bGotUserName = TRUE;
				lpszUrl++;
				pszCurrentUrl = szCurrentUrl;
			}
			else if (ch == '/' || ch == '?' || (!*(lpszUrl+1)))
			{
				//we're at the end of this loop
				//set the domain name and break
				if (!*(lpszUrl+1) && ch != '/' && ch != '?')
				{
					*pszCurrentUrl++ = ch;
					lpszUrl++;
				}
				*pszCurrentUrl = '\0';
				if (!bGotHostName)
				{
					if (!SetHostName(szCurrentUrl))
						goto error;
				}
				pszCurrentUrl = szCurrentUrl;
				break;
			}
			else
			{
				*pszCurrentUrl++ = ch;
				lpszUrl++;
			}
		}

		if (!bGotScheme)
			goto error;

		//Now build the path
		while ((ch = *lpszUrl) != '\0')
		{
			//break on a '#' or a '?', which delimit "extra information"
			if (m_nScheme != ATL_URL_SCHEME_FILE && (ch == '#' || ch == '?'))
			{
				break;
			}
			*pszCurrentUrl++ = ch;
			lpszUrl++;
		}
		*pszCurrentUrl = '\0';
		
		if (*szCurrentUrl != '\0' && !SetUrlPath(szCurrentUrl))
			goto error;

		pszCurrentUrl = szCurrentUrl;

		while ((ch = *lpszUrl++) != '\0')
		{
			*pszCurrentUrl++ = ch;
		}

		*pszCurrentUrl = '\0';
		if (*szCurrentUrl != '\0' && !SetExtraInfo(szCurrentUrl))
			goto error;

		switch(m_nScheme)
		{
			case ATL_URL_SCHEME_FILE:
				m_nPortNumber = ATL_URL_INVALID_PORT_NUMBER;
				break;
			case ATL_URL_SCHEME_NEWS:
				m_nPortNumber = ATL_URL_INVALID_PORT_NUMBER;
				break;
			case ATL_URL_SCHEME_MAILTO:
				m_nPortNumber = ATL_URL_INVALID_PORT_NUMBER;
				break;
			default:
				if (!bGotPortNumber)
					m_nPortNumber = (unsigned short)AtlGetDefaultUrlPort(m_nScheme);
		}

		return TRUE;

error:
		InitFields();
		return FALSE;

	}

	ATL_NOINLINE void InitFields() throw()
	{
		m_nPortNumber = ATL_URL_INVALID_PORT_NUMBER;
		m_nScheme = ATL_URL_SCHEME_UNKNOWN;

		m_dwSchemeNameLength = 0;
		m_dwHostNameLength   = 0;
		m_dwUserNameLength   = 0;
		m_dwUrlPathLength    = 0;
		m_dwPasswordLength   = 0;
		m_dwExtraInfoLength  = 0;

		m_szScheme[0]        = '\0';
		m_szHostName[0]      = '\0';
		m_szUserName[0]      = '\0';
		m_szPassword[0]      = '\0';
		m_szUrlPath[0]       = '\0';
		m_szExtraInfo[0]     = '\0';
	}

	//copy all fields from urlThat
	inline void CopyFields(const CUrl& urlThat) throw()
	{
		_tcsncpy(m_szScheme, urlThat.m_szScheme, urlThat.m_dwSchemeNameLength+1);
		_tcsncpy(m_szHostName, urlThat.m_szHostName, urlThat.m_dwHostNameLength+1);
		_tcsncpy(m_szUserName, urlThat.m_szUserName, urlThat.m_dwUserNameLength+1);
		_tcsncpy(m_szPassword, urlThat.m_szPassword, urlThat.m_dwPasswordLength+1);
		_tcsncpy(m_szUrlPath, urlThat.m_szUrlPath, urlThat.m_dwUrlPathLength+1);
		_tcsncpy(m_szExtraInfo, urlThat.m_szExtraInfo, urlThat.m_dwExtraInfoLength+1);

		m_nPortNumber        = urlThat.m_nPortNumber;
		m_nScheme            = urlThat.m_nScheme;
		m_dwSchemeNameLength = urlThat.m_dwSchemeNameLength;
		m_dwHostNameLength   = urlThat.m_dwHostNameLength;
		m_dwUserNameLength   = urlThat.m_dwUserNameLength;
		m_dwPasswordLength   = urlThat.m_dwPasswordLength;
		m_dwUrlPathLength    = urlThat.m_dwUrlPathLength;
		m_dwExtraInfoLength  = urlThat.m_dwExtraInfoLength;
	}

}; // class CUrl

//
// IWorkerThreadClient
// Interface to be used with CWorkerThread

class ATL_NO_VTABLE IWorkerThreadClient
{
public:
	virtual HRESULT Execute(DWORD_PTR dwParam, HANDLE hObject)=0;
	virtual HRESULT CloseHandle(HANDLE hHandle)=0;
};


#ifndef ATL_WORKER_THREAD_WAIT
#define ATL_WORKER_THREAD_WAIT 10000	// time to wait when shutting down
#endif

//
// CWorkerThread
// This class creates a worker thread that waits on kernel
// object handles and executes a specified client
// function when the handle is signaled
// To use it, construct an instance, call Initialize
// then call add AddHandle with the handle of a kernel
// object and pass a pointer to your implementation
// of IWorkerThreadClient.  Execute on your IWorkerThreadClient
// implementation will be called when the handle is signaled
// You can also use AddTimer() to add a waitable timer
// to the worker thread.
// If the thread is still active when your object is destroyed
// you must call RemoveHandle() on each handle that your object
// owns.
// To terminate the thread, call Shutdown
//
template <class ThreadTraits=DefaultThreadTraits>
class CWorkerThread
{
protected:
	HANDLE m_hThread;
	DWORD m_dwThreadId;

	struct WorkerClientEntry
	{
		IWorkerThreadClient *pClient;
		DWORD_PTR dwParam;
	};

	CSimpleValArray<HANDLE> m_hWaitHandles;
	CSimpleArray<WorkerClientEntry, CSimpleArrayEqualHelperFalse<WorkerClientEntry> > m_ClientEntries;
	CComCriticalSection m_critSec;
	HANDLE m_hRefreshComplete;

	void Refresh() throw()
	{
		ATLASSERT(m_hRefreshComplete);
		BOOL bRet = SetEvent(m_hWaitHandles[1]);
		ATLASSERT(bRet);
		bRet; // unused
		WaitForSingleObject(m_hRefreshComplete, INFINITE);
	}

public:
	CWorkerThread() throw() : m_hThread(NULL), m_dwThreadId(0), m_hRefreshComplete(NULL)
	{

	}

	~CWorkerThread() throw()
	{
		Shutdown();
	}

	DWORD GetThreadId() throw()
	{
		return m_dwThreadId;
	}

	HANDLE GetThreadHandle() throw()
	{
		return m_hThread;
	}

	HRESULT Initialize() throw()
	{
		// the object should be initialized first
		ATLASSERT(m_hWaitHandles.GetSize() == 0);

		m_critSec.Init();

		// create the refresh complete event
		m_hRefreshComplete = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (!m_hRefreshComplete)
		{
			m_critSec.Term();
			return AtlHresultFromLastError();
		}

		// add the shutdown event
		HRESULT hr;

		HANDLE hEventShutdown = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (!hEventShutdown)
		{
			hr = AtlHresultFromLastError();
			Shutdown();
			return hr;
		}

		hr = AddHandle(hEventShutdown, NULL, 0);
		if (FAILED(hr))
		{
			CloseHandle(hEventShutdown);
			Shutdown();
			return hr;
		}

		// create the refresh event
		HANDLE hEventRefresh = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (!hEventRefresh)
		{
			hr = AtlHresultFromLastError();
			Shutdown();
			return hr;
		}

		hr = AddHandle(hEventRefresh, NULL, 0);
		if (FAILED(hr))
		{
			CloseHandle(hEventRefresh);
			Shutdown();
			return hr;
		}

		m_hThread = ThreadTraits::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) _WorkerThreadProc, 
			this, 0, &m_dwThreadId);
		if (!m_hThread)
		{
			hr = AtlHresultFromLastError();
			Shutdown();
			return hr;
		}

		WaitForSingleObject(m_hRefreshComplete, INFINITE);

		return S_OK;
	}

	HRESULT AddHandle(HANDLE hObject, IWorkerThreadClient *pClient, DWORD_PTR dwParam) throw()
	{
		// Make sure the object has been initialized
		ATLASSERT(m_hRefreshComplete != NULL);

		m_critSec.Lock();
		BOOL bRet = m_hWaitHandles.Add(hObject);
		if (!bRet)
		{
			m_critSec.Unlock();
			return E_OUTOFMEMORY;
		}

		WorkerClientEntry entry;
		entry.pClient = pClient;
		entry.dwParam = dwParam;
		bRet = m_ClientEntries.Add(entry);
		if (!bRet)
		{
			m_hWaitHandles.RemoveAt(m_hWaitHandles.GetSize()-1);
			m_critSec.Unlock();
			return E_OUTOFMEMORY;
		}
		if (m_hWaitHandles.GetSize() > 2)
		{
			// tell the worker thread to refresh
			Refresh();
		}
		m_critSec.Unlock();
		return S_OK;
	}

#if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
	HRESULT AddTimer(DWORD dwInterval, IWorkerThreadClient *pClient, DWORD_PTR dwParam, HANDLE *phTimer) throw()
	{
		// Make sure the object has been initialized
		ATLASSERT(m_hRefreshComplete != NULL);

		ATLASSERT(phTimer);
		*phTimer = NULL;

		HANDLE hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
		if (!hTimer)
		{
			return AtlHresultFromLastError();
		}

		HRESULT hr;
		LARGE_INTEGER liDueTime;

		liDueTime.QuadPart = -10000 * (__int64) dwInterval;

		BOOL bRet = SetWaitableTimer(hTimer, &liDueTime, dwInterval,  NULL, NULL, FALSE);
		if (!bRet)
		{
			hr = AtlHresultFromLastError();
			CloseHandle(hTimer);
			return hr;
		}

		hr = AddHandle(hTimer, pClient, dwParam);
		if (FAILED(hr))
		{
			CloseHandle(hTimer);
			return hr;
		}
		if (phTimer)
			*phTimer = hTimer;
		return S_OK;
	}
#endif

	HRESULT RemoveHandle(HANDLE hObject) throw()
	{
		// Make sure the object has been initialized
		ATLASSERT(m_hRefreshComplete != NULL);

		m_critSec.Lock();

		int nIndex = m_hWaitHandles.Find(hObject);
		if (nIndex >= 0)
		{
			ATLASSERT(nIndex < m_ClientEntries.GetSize());

			WorkerClientEntry& entry = m_ClientEntries[nIndex];
			if (!entry.pClient || FAILED(entry.pClient->CloseHandle(m_hWaitHandles[nIndex])))
				CloseHandle(m_hWaitHandles[nIndex]);

			m_hWaitHandles.RemoveAt(nIndex);
			m_ClientEntries.RemoveAt(nIndex);

			Refresh();
		}
		m_critSec.Unlock();
		return S_OK;
	}

	void RemoveAllClients() throw()
	{
		ATLASSERT(m_hWaitHandles.GetSize() == m_ClientEntries.GetSize());

		int nLen = m_hWaitHandles.GetSize();
		for (int i=0; i<nLen; i++)
		{
			WorkerClientEntry& entry = m_ClientEntries[i];
			if (!entry.pClient || FAILED(entry.pClient->CloseHandle(m_hWaitHandles[i])))
				CloseHandle(m_hWaitHandles[i]);
		}
		m_hWaitHandles.RemoveAll();

		m_ClientEntries.RemoveAll();
	}

	HRESULT Shutdown(DWORD dwWait=ATL_WORKER_THREAD_WAIT) throw()
	{
		if (!m_hThread)
		{
			RemoveAllClients();
			m_critSec.Term();
			if (m_hRefreshComplete)
			{
				CloseHandle(m_hRefreshComplete);
				m_hRefreshComplete = NULL;
			}
			return S_OK;
		}

		ATLASSERT(m_hWaitHandles.GetSize() > 0);

		SetEvent(m_hWaitHandles[0]);

		DWORD dwRet = WaitForSingleObject(m_hThread, dwWait);

		RemoveAllClients();

		CloseHandle(m_hThread);
		m_hThread = NULL;
		if (m_hRefreshComplete)
		{
			CloseHandle(m_hRefreshComplete);
			m_hRefreshComplete = NULL;
		}
		m_critSec.Term();
		return (dwRet == WAIT_OBJECT_0) ? S_OK : AtlHresultFromWin32(dwRet);
	}

protected:
	DWORD WorkerThreadProc() throw()
	{
		// Make sure the object has been initialized
		ATLASSERT(m_hRefreshComplete != NULL);

		CSimpleValArray<HANDLE> handles(m_hWaitHandles);
		CSimpleArray<WorkerClientEntry, CSimpleArrayEqualHelperFalse<WorkerClientEntry> > clientEntries(m_ClientEntries);

		// tell the main thread we're done copying
		SetEvent(m_hRefreshComplete);

		while (TRUE)
		{
			DWORD dwRet = WaitForMultipleObjects(handles.GetSize(), handles.GetData(),
					FALSE, INFINITE);
			// check for shutdown
			if (dwRet == WAIT_OBJECT_0)
				return 0;
			else if (dwRet == WAIT_OBJECT_0+1)	// check for refresh
			{
				handles = m_hWaitHandles;
				clientEntries = m_ClientEntries;

				// tell the main thread we're done copying
				SetEvent(m_hRefreshComplete);
				continue;
			}
			else if (dwRet > WAIT_OBJECT_0 && dwRet < WAIT_OBJECT_0 + handles.GetSize())
			{
				// execute the approriate client
				WorkerClientEntry& entry = clientEntries[dwRet - WAIT_OBJECT_0];

				// We ignore the error code because nothing useful can be done with it in this
				// implementation
				entry.pClient->Execute(entry.dwParam, handles[dwRet - WAIT_OBJECT_0]);
			}
		}
		return 0;		
	}

	static DWORD WINAPI _WorkerThreadProc(CWorkerThread *pThis) throw()
	{
		return pThis->WorkerThreadProc();
	}
}; // class CWorkerThread


template <class ThreadTraits=DefaultThreadTraits>
class CWorkerThreadWrapper
{
protected:
	CWorkerThread<ThreadTraits> *m_pThread;

public:
	CWorkerThreadWrapper() throw() : m_pThread(NULL)
	{

	}

	DWORD GetThreadId() throw()
	{
		if (!m_pThread)
			return 0;
		return m_pThread->GetThreadId();
	}

	HANDLE GetThreadHandle() throw()
	{
		if (!m_pThread)
			return NULL;
		return m_pThread->GetThreadHandle();
	}

	HRESULT InitializeWorker(CWorkerThread<ThreadTraits> *pThread) throw()
	{
		m_pThread = pThread;
		return S_OK;
	}

	HRESULT AddHandle(HANDLE hObject, IWorkerThreadClient *pClient, DWORD_PTR dwParam) throw()
	{
		if (!m_pThread)
			return S_OK;
		return m_pThread->AddHandle(hObject, pClient, dwParam);
	}

#if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
	HRESULT AddTimer(DWORD dwInterval, IWorkerThreadClient *pClient, DWORD_PTR dwParam, HANDLE *phTimer) throw()
	{
		if (!m_pThread)
			return S_OK;
		return m_pThread->AddTimer(dwInterval, pClient, dwParam, phTimer);
	}
#endif

	void RemoveAllClients() throw()
	{
		if (!m_pThread)
			return;
		m_pThread->RemoveAllClients();
	}

	HRESULT RemoveHandle(HANDLE hObject) throw()
	{
		if (!m_pThread)
			return S_OK;
		return m_pThread->RemoveHandle(hObject);
	}

	HRESULT Shutdown(DWORD dwWait=ATL_WORKER_THREAD_WAIT) throw()
	{
		dwWait;
		return S_OK;
	}
};

class CDummyWorkerThread
{
protected:

public:
	DWORD GetThreadId() throw()
	{
		return 0;
	}

	HANDLE GetThreadHandle() throw()
	{
		return NULL;
	}

	HRESULT Initialize() throw()
	{
		return S_OK;
	}

	HRESULT AddHandle(HANDLE /*hObject*/, IWorkerThreadClient * /*pClient*/, DWORD_PTR /*dwParam*/) throw()
	{
		return S_OK;
	}


	HRESULT AddTimer(DWORD /*dwInterval*/, IWorkerThreadClient * /*pClient*/, DWORD_PTR /*dwParam*/, HANDLE * /*phTimer*/) throw()
	{
		return S_OK;
	}

	HRESULT RemoveHandle(HANDLE /*hObject*/) throw()
	{
		return S_OK;
	}

	HRESULT Shutdown(DWORD dwWait=ATL_WORKER_THREAD_WAIT) throw()
	{
		dwWait;
		return S_OK;
	}
};

inline BOOL CopyCString(const CString& str, TCHAR *pszDest, DWORD *pnDestLen) throw()
{
	if (!pszDest || !pnDestLen)
		return FALSE;

	*pszDest = 0;
	DWORD nLen = (DWORD)(str.GetLength() * sizeof(TCHAR));
	if (nLen > 0 && *pnDestLen < (nLen + sizeof(TCHAR)))
	{
		*pnDestLen = nLen + sizeof(TCHAR);
		return FALSE;
	}
	else if (nLen <= 0)
	{
		*pnDestLen = 0;
		return TRUE;
	}

	BOOL bRet = FALSE;
	LPCTSTR pszBuffer = (LPCTSTR)str;
	if (pszBuffer)
	{
		memcpy(pszDest, pszBuffer, nLen);
		pszDest[nLen/sizeof(TCHAR)] = 0;
		*pnDestLen = nLen + sizeof(TCHAR);
		bRet = TRUE;
	}
	return bRet;
}

inline BOOL CopyCStringA(const CStringA& str, LPSTR szDest, DWORD *pnDestLen) throw()
{
	if (!szDest || !pnDestLen)
		return FALSE;

	*szDest = 0;
	DWORD nLen = (DWORD)(str.GetLength());
	if (nLen > 0 && *pnDestLen < (nLen + 1))
	{
		*pnDestLen = nLen + 1;
		return FALSE;
	}
	else if (nLen <= 0)
	{
		*pnDestLen = 0;
		return TRUE;
	}

	BOOL bRet = FALSE;
	LPCSTR szBuffer = (LPCSTR)str;
	if (szBuffer)
	{
		memcpy(szDest, szBuffer, nLen);
		szDest[nLen] = 0;
		*pnDestLen = nLen;
		bRet = TRUE;
	}
	return bRet;
}

// Call this function to convert from a SYSTEMTIME
// structure to an Http-date as defined in rfc2616
inline void SystemTimeToHttpDate(const SYSTEMTIME& st, CStringA &strTime) throw()
{
	static LPCSTR szDays[] = { "Sun", "Mon", "Tue",
		"Wed", "Thu", "Fri", "Sat" };
	static LPCSTR szMonth[] = { "Jan", "Feb", "Mar", "Apr",
		"May", "Jun", "Jul", "Aug", "Sep", 
		"Oct", "Nov", "Dec" };

	strTime.Format("%s, %02d %s %d %02d:%02d:%02d GMT",
		szDays[st.wDayOfWeek], st.wDay, szMonth[st.wMonth-1], st.wYear,
		st.wHour, st.wMinute, st.wSecond);
}

// RGBToHtml - Converts a COLORREF to a color that can be used in HTML.
//             Eg. RGB(11,22,33) would be converted to #112233
// color:    The color to convert.
// pbOut:    The output buffer that will hold the the resulting color.
// nBuffer:	 Specifies the number of bytes in pbOut.
bool inline RGBToHtml(COLORREF color, LPTSTR pbOut, long nBuffer)
{
	// make sure the buffer is big enough
	if (nBuffer < (7 * sizeof(TCHAR)))
		return false;

	BYTE *pColor = (BYTE*)&color;
	wsprintf(pbOut, _T("#%0.2x%0.2x%0.2x"),
		(BYTE)*(pColor), (BYTE)*(pColor+1), (BYTE)*(pColor+2));
	return true;
}

} // namespace ATL

#pragma warning( pop )
