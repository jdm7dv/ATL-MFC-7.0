// This is a part of the Active Template Library.
// Copyright (C) 1996-2000 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef _WINSOCKAPI_
	#include <winsock2.h>
#endif


#include <atlutil.h>
#include <atlcoll.h>
#include <atlfile.h>

#define SECURITY_WIN32
#include <security.h>
#include <atlenc.h>

#ifndef _ATL_NO_DEFAULT_LIBS
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "SECUR32.LIB")
#endif  // !_ATL_NO_DEFAULT_LIBS

#pragma once

namespace ATL {

class CAtlHttpClient;

enum status_headerparse{
				ATL_HEADER_PARSE_COMPLETE=0,
				ATL_HEADER_PARSE_HEADERNOTCOMPLETE,
				ATL_HEADER_PARSE_HEADERERROR
};

enum readstate{rs_init=0, rs_readheader, rs_scanheader, rs_readbody, rs_complete};

#define ATL_DEFAULT_DATA_TYPE _T("application/x-www-form-urlencoded")
#define ATL_HEADER_END "\r\n\r\n"
#define ATL_HEADER_END_LEN 4
#define ATL_DW_HEADER_END 0x0a0d0a0d
#define ATL_FIELDNAME_DELIMITER _T(':')
#define ATL_MAX_FIELDNAME_LEN 1024
#define ATL_MAX_VALUE_LEN 1024
#define ATL_AUTH_HDR_SIZE 1024
#define ATL_READ_BUFF_SIZE 2048
#define ATL_INVALID_STATUS -1
#define ATL_HTTP_HEADER _T(" HTTP/1.1\r\n")
#define ATL_HTTP_HEADER_PROXY _T(" HTTP/1.1\r\n")
#define ATL_HTTP_FOOTER \
					_T("User-Agent: Microsoft-ATL-Native/7.00\r\n")

#define ATL_IS_INVALIDCREDHANDLE(x) ((x.dwLower==0xFFFFFFFF) && (x.dwUpper==0xFFFFFFFF))					
#define ATL_HTTP_AUTHTYPE_NTLM _T("NTLM")
#define ATL_HTTP_AUTHTYPE_BASIC _T("BASIC")
#define ATL_HTTP_METHOD_GET _T("GET")
#define ATL_HTTP_METHOD_POST _T("POST")
#define ATL_HTTP_FLAG_AUTO_REDIRECT 0x1
#define ATL_BASIC_AUTH_STRING "Authorization: Basic "

#ifndef ATL_SOCK_TIMEOUT
	#define ATL_SOCK_TIMEOUT 10000
#endif

#ifndef ATL_MAX_USERNAME
	#define ATL_MAX_USERNAME 64
#endif

#ifndef ATL_MAX_PWD
	#define ATL_MAX_PWD 64
#endif

#define ATL_MAX_BASIC_ID_LEN (ATL_MAX_USERNAME + ATL_MAX_PWD+2)


// One of these objects can be created globally to turn
// on the socket stuff at CRT startup and shut it down
// on CRT term.
class _AtlWSAInit
{
public:
	_AtlWSAInit() throw()
	{
		m_dwErr = WSAEFAULT;
	}

	bool Init()
	{
		m_dwErr = WSAStartup(WINSOCK_VERSION, &m_stData);
		return m_dwErr == 0;
	}

	bool IsStarted(){ return m_dwErr == 0; }

	~_AtlWSAInit() throw()
	{
		if (!m_dwErr)
			WSACleanup();
	}

	WSADATA  m_stData;
	DWORD m_dwErr;
};

__declspec(selectany)_AtlWSAInit g_HttpInit;

// ZEvtSyncSocket
// ************ This is an implementation only class ************
// Class ZEvtSyncSocket is a non-supported, implementation only 
// class used by the ATL HTTP client class CAtlHttpClient. Do not
// use this class in your code. Use of this class is not supported by Microsoft.
class ZEvtSyncSocket
{
public:
	ZEvtSyncSocket() throw()
	{
		m_dwCreateFlags = WSA_FLAG_OVERLAPPED;
		m_hEventRead = m_hEventWrite = m_hEventConnect = NULL;
		m_socket = INVALID_SOCKET;
		m_bConnected = false;
		m_dwLastError = 0;
		g_HttpInit.Init();
	}

	~ZEvtSyncSocket() throw()
	{
		Term();
	}

	operator SOCKET() throw()
	{
		return m_socket;
	}

	void Close()
	{
		if (m_socket != INVALID_SOCKET)
		{
			m_bConnected = false;
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
			Term();
		}
	}

	void Term() throw()
	{
		if (m_hEventRead)
		{
			WSACloseEvent(m_hEventRead);
			m_hEventRead = NULL;
		}
		if (m_hEventWrite)
		{
			WSACloseEvent(m_hEventWrite);
			m_hEventWrite = NULL;
		}
		if (m_hEventConnect)
		{
			WSACloseEvent(m_hEventConnect);
			m_hEventConnect = NULL;
		}
		m_socket = INVALID_SOCKET;
	}

	bool Create(WORD wFlags=0)
	{
		return Create(PF_INET, SOCK_STREAM, IPPROTO_TCP, wFlags);
	}

	bool Create(short af, short st, short proto,
		WORD wFlags=0) throw()
	{
		bool bRet = true;
		m_socket = WSASocket(af, st, proto, NULL, 0,
			wFlags | m_dwCreateFlags);
		if (m_socket == INVALID_SOCKET)
		{
			bRet = false;
		}
		else
			bRet = Init(m_socket, NULL);
		return bRet;
	}

	bool Connect(LPCTSTR szAddr, unsigned short nPort) throw()
	{
		if (m_bConnected)
			return true;

		bool bRet = true;
		sockaddr_in sa;
		sa.sin_family = PF_INET;
		sa.sin_port = htons(nPort);
		sa.sin_zero[0] = 0;
		sa.sin_addr.s_addr = INADDR_NONE;

		if (INADDR_NONE == (sa.sin_addr.s_addr = inet_addr(CT2A(szAddr))))
		{
			HOSTENT *ph = gethostbyname(CT2A(szAddr));
			if (ph)
			{
				sa.sin_addr.s_addr = *((ULONG FAR*)ph->h_addr);
			}
		}

		if (sa.sin_addr.s_addr == INADDR_NONE)
		{
			m_dwLastError = WSAGetLastError();
			bRet = false;
		}
		else
		{
			bRet = Connect((sockaddr*)&sa);
		}
		if (bRet)
			m_bConnected = true;
		return bRet;
	}

	bool Write(WSABUF *pBuffers, int nCount, DWORD *pdwSize) throw()
	{
		// if we aren't already connected we'll wait to see if the connect
		// event happens
		if (WAIT_OBJECT_0 != WaitForSingleObject((HANDLE)m_hEventConnect , ATL_SOCK_TIMEOUT))
			return false; // not connected

		// make sure we aren't already writing
		if (WAIT_TIMEOUT == WaitForSingleObject((HANDLE)m_hEventWrite, 0))
			return false; // another write on is blocking this socket

		bool bRet = true;
		*pdwSize = 0;
		WSAOVERLAPPED o;
		m_csWrite.Lock();
		o.hEvent = m_hEventWrite;
		WSAResetEvent(o.hEvent);
		if (WSASend(m_socket, pBuffers, nCount, pdwSize, 0, &o, 0))
		{	
			DWORD dwLastError = WSAGetLastError();
			if (dwLastError != WSA_IO_PENDING)
			{
				m_dwLastError = dwLastError;
				bRet = false;
			}
		}
		
		// wait for write to complete
		if (bRet && WAIT_OBJECT_0 == WaitForSingleObject((HANDLE)m_hEventWrite, ATL_SOCK_TIMEOUT))
		{
			DWORD dwFlags = 0;
			if (WSAGetOverlappedResult(m_socket, &o, pdwSize, FALSE, &dwFlags))
				bRet = true;
			else
				bRet = false;
		}
		
		m_csWrite.Unlock();
		return bRet;

	}

	bool Write(const unsigned char *pBuffIn, DWORD *pdwSize) throw()
	{
		// if we aren't already connected we'll wait to see if the connect
		// event happens
		if (WAIT_OBJECT_0 != WaitForSingleObject((HANDLE)m_hEventConnect , ATL_SOCK_TIMEOUT))
			return false; // not connected

		// make sure we aren't already writing
		if (WAIT_TIMEOUT == WaitForSingleObject((HANDLE)m_hEventWrite, 0))
			return false; // another write on is blocking this socket

		bool bRet = true;
		WSABUF buff;
		buff.buf = (char*)pBuffIn;
		buff.len = *pdwSize;
		*pdwSize = 0;
		WSAOVERLAPPED o;
		m_csWrite.Lock();
		o.hEvent = m_hEventWrite;
		WSAResetEvent(o.hEvent);
		if (WSASend(m_socket, &buff, 1, pdwSize, 0, &o, 0))
		{	
			DWORD dwLastError = WSAGetLastError();
			if (dwLastError != WSA_IO_PENDING)
			{
				m_dwLastError = dwLastError;
				bRet = false;
			}
		}
		
		// wait for write to complete
		if (bRet && WAIT_OBJECT_0 == WaitForSingleObject((HANDLE)m_hEventWrite, ATL_SOCK_TIMEOUT))
		{
			DWORD dwFlags = 0;
			if (WSAGetOverlappedResult(m_socket, &o, pdwSize, FALSE, &dwFlags))
				bRet = true;
			else
				bRet = false;
		}
		
		m_csWrite.Unlock();
		return bRet;
	}

	bool Read(const unsigned char *pBuff, DWORD *pdwSize) throw()
	{
		// if we aren't already connected we'll wait to see if the connect
		// event happens
		if (WAIT_OBJECT_0 != WaitForSingleObject((HANDLE)m_hEventConnect , ATL_SOCK_TIMEOUT))
			return false; // not connected

		if (WAIT_ABANDONED == WaitForSingleObject((HANDLE)m_hEventRead, 0))
			return false; // already reading

		bool bRet = true;
		WSABUF buff;
		buff.buf = (char*)pBuff;
		buff.len = *pdwSize;
		*pdwSize = 0;
		DWORD dwFlags = 0;
		WSAOVERLAPPED o;
		ZeroMemory(&o, sizeof(o));

		// protect against re-entrency
		m_csRead.Lock();
		o.hEvent = m_hEventRead;
		WSAResetEvent(o.hEvent);
		if (WSARecv(m_socket, &buff, 1, pdwSize, &dwFlags, &o, 0))
		{
			DWORD dwLastError = WSAGetLastError();
			if (dwLastError != WSA_IO_PENDING)
			{
				m_dwLastError = dwLastError;
				bRet = false;
			}
		}

		// wait for the read to complete
		if (bRet && WAIT_OBJECT_0 == WaitForSingleObject((HANDLE)o.hEvent, ATL_SOCK_TIMEOUT))
		{
			DWORD dwFlags = 0;
			if (WSAGetOverlappedResult(m_socket, &o, pdwSize, FALSE, &dwFlags))
				bRet = true;
			else
				bRet = false;

		}

		m_csRead.Unlock();
		return bRet;
	}
	
	bool Connect(SOCKADDR* psa) throw()
	{
		DWORD dwLastError;
		bool bRet = true;
		if (WSAConnect(m_socket, 
			psa, sizeof(SOCKADDR),
			NULL, NULL, NULL, NULL))
		{
			dwLastError = WSAGetLastError();
			if (dwLastError != WSAEWOULDBLOCK)
			{
				m_dwLastError = dwLastError;
				bRet = false;
			}
			else
			{
				dwLastError = WaitForSingleObject((HANDLE)m_hEventConnect, 10000);
				if (dwLastError == WAIT_OBJECT_0)
				{
					// make sure there were no connection errors.
					WSANETWORKEVENTS wse;
					ZeroMemory(&wse, sizeof(wse));
					WSAEnumNetworkEvents(m_socket, NULL, &wse);
					if (wse.iErrorCode[FD_CONNECT_BIT]!=0)
					{
						m_dwLastError = (DWORD)(wse.iErrorCode[FD_CONNECT_BIT]);
						return false;
					}
				}
			}

		}
		return bRet;
	}

	bool Init(SOCKET hSocket, void * /*pData=NULL*/) throw()
	{
		ATLASSERT(hSocket != INVALID_SOCKET);

		if (hSocket == INVALID_SOCKET)
			return false;
		m_socket = hSocket;
		
		// Allocate Events. On error, any open event handles will be closed
		// in the destructor
		if (NULL != (m_hEventRead = WSACreateEvent()))
			if (NULL != (m_hEventWrite = WSACreateEvent()))
				if (NULL != (m_hEventConnect = WSACreateEvent()))
		{
			if (!WSASetEvent(m_hEventWrite) || !WSASetEvent(m_hEventRead))
				return false;

			if (SOCKET_ERROR != WSAEventSelect(m_socket, m_hEventRead, FD_READ))
				if (SOCKET_ERROR != WSAEventSelect(m_socket, m_hEventWrite, FD_WRITE))
					if (SOCKET_ERROR != WSAEventSelect(m_socket, m_hEventConnect, FD_CONNECT))
						return true;
		}
		return false;
	}

	DWORD m_dwCreateFlags;
	WSAEVENT m_hEventRead;
	WSAEVENT m_hEventWrite;
	WSAEVENT m_hEventConnect;

	CComAutoCriticalSection m_csRead;
	CComAutoCriticalSection m_csWrite;
	SOCKET m_socket;
	bool m_bConnected;
	DWORD m_dwLastError;
};
// pure virtual class that describes all authorization objects
class CAtlBaseAuthObject
{
public:
	virtual bool Authenticate(LPCTSTR szAuthTypes) = 0;
	virtual void Init(CAtlHttpClient *pSocket) = 0;
};

// CDefaultAuthObject only does NTLM authintication using the
// credentials of the logged on user.
class CDefaultAuthObject :
	public CAtlBaseAuthObject
{
public:
	CAtlHttpClient *m_pSocket;
	CredHandle m_hCredentials;
	int m_nMaxTokenSize;
	TimeStamp m_ts;
	CDefaultAuthObject() throw(): 
		m_pSocket(NULL),
		m_nMaxTokenSize(0)
	{
		SecInvalidateHandle(&m_hCredentials)
	}

	~CDefaultAuthObject()
	{
		if (!ATL_IS_INVALIDCREDHANDLE(m_hCredentials))
			FreeCredentialsHandle(&m_hCredentials);
	}

	void Init(CAtlHttpClient *pSocket) throw()
	{
		m_pSocket = pSocket;
	}

	bool Authenticate(LPCTSTR szAuthTypes) throw()
	{
		if (_tcsstr(szAuthTypes, ATL_HTTP_AUTHTYPE_NTLM))
		{
			if (AcquireCredHandle())
				return DoNTLMAuthenticate();
		}
		return false;
	}

	bool AcquireCredHandle()
	{
		PSecPkgInfo pPackageInfo = NULL;
		SECURITY_STATUS SecurityStatus = SEC_E_OK;

		// Acquire a credentials handle on the NTLM security package
		SecurityStatus = QuerySecurityPackageInfo(ATL_HTTP_AUTHTYPE_NTLM,
								&pPackageInfo);

		if (SecurityStatus != SEC_E_OK)
			return false;

		SecurityStatus = AcquireCredentialsHandle(
						0,
						pPackageInfo->Name,
						SECPKG_CRED_OUTBOUND,
						0,
						0,
						0,
						0,
						&m_hCredentials,
						&m_ts
						);

		m_nMaxTokenSize = pPackageInfo->cbMaxToken;
		FreeContextBuffer(pPackageInfo);
		return SecurityStatus == SEC_E_OK ? true : false;
	}

	// This function creates an NTML Authorization header
	// and sends it to the HTTP server.
	bool SendSecurityInfo(SecBuffer *pSecBuffer, LPSTR *pszBuffer);
	bool DoNTLMAuthenticate() throw();
};

// Interface used by the basic authentication object
// CBasicAuthObject to retrieve a user's credentials.
// CBasicAuthObject does not store a user's credentials.
__interface IBasicAuthInfo
{
	bool GetPassword(LPSTR szPwd, DWORD dwBuffSize);
	bool GetUsername(LPSTR szUid, DWORD dwBuffSize);
};

// Performs BASIC authentication for an CAtlHttpClient
// object. Caller must implement an IBasicAuthInfo interface
// and pass it to this object before this object attempts
// to authenticate or authentication will fail. We use an
// interface to acquire logon credentials so we don't have
// to cache the credentials in this class.
class CBasicAuthObject : 
	public CAtlBaseAuthObject
{
public:
	CBasicAuthObject()
	{
		m_pClient = NULL;
		m_pAuthInfo = NULL;
	}

	CBasicAuthObject(IBasicAuthInfo *pAuthInfo)
	{
		m_pAuthInfo = pAuthInfo;
		m_pClient = NULL;
	}

	void SetAuthInfo(IBasicAuthInfo *pAuthInfo)
	{
		m_pAuthInfo = pAuthInfo;
	}

	// Called by the CAtlHttpClient class to 
	// authenticate a user.
	virtual bool Authenticate(LPCTSTR szAuthTypes)
	{
		TCHAR buff[ATL_AUTH_HDR_SIZE];
		_tcscpy(buff, szAuthTypes);
		_tcslwr(buff);

		if (_tcsstr(buff, _T("basic")))
		{
			return DoBasicAuthenticate();
		}
		return false;

	}

	// Called by the CAtlHttpClient class to initialize
	// this authentication object.
	virtual void Init(CAtlHttpClient *pSocket)
	{
		ATLASSERT(pSocket);
		m_pClient = pSocket;
	}
	bool DoBasicAuthenticate();

protected:
	IBasicAuthInfo *m_pAuthInfo;
	CAtlHttpClient *m_pClient;
};
// CAtlLCStringAMap lower cases all the keys before they are stored and/or
// looked up in the map. Using this map in the HTTP socket class, 
// we won't have to worry about what format the server passes it's
// headers in. The RFC's leave the case format of header name fields
// as an implementation detail.

typedef CAtlMap< 
				CString,
				CString,
				CStringElementTraitsI<CString>,
				CStringElementTraitsI<CString>
			   > CAtlLCStringMap;

class CAtlHttpClient : 
	public ZEvtSyncSocket
{
protected:
// class data
	CAtlLCStringMap m_HeaderMap; // Map of response headers
	CAtlIsapiBuffer<> m_current; // The entire response
	CAtlBaseAuthObject *m_pAuthObject; // Object used to do authorization
	CUrl m_urlCurrent; // URL of current request

	CString m_strMethod; // Current request method.
	CString m_strProxy; // Path to current proxy server.
	CString m_strDataType; // If this is a post, this contains the data type of the data

	long m_nStatus; // Current response status (from status line)
	short m_nProxyPort; // Port used on current proxy server
	DWORD m_dwBodyLen; // Length of body
	DWORD m_dwHeaderLen; // Length of current raw headers
	DWORD m_dwHeaderStart;
	BYTE *m_pData; // Pointer to data sent with request (warning: we don't make our own copy of the data!)
	DWORD m_dwDataLen; // Length of current data in bytes
	DWORD m_dwFlags; // Flags that describe the request.
	BYTE *m_pCurrent;


public:
	CAtlHttpClient()
	{
		InitializeObject();
	}

	// Sets this object to a known state.
	void InitializeObject() throw()
	{
		Close(); // will close the socket if it's already open

		// reset all data that has to do with the current request
		m_HeaderMap.RemoveAll();
		m_current.Empty();
		m_pAuthObject = NULL;
		m_urlCurrent.Clear();

		m_strMethod = __T("");
		m_strProxy = _T("");
		m_strDataType = _T("");

		m_nStatus = ATL_INVALID_STATUS;
		m_nProxyPort = ATL_URL_INVALID_PORT_NUMBER;
		m_dwBodyLen = 0;
		m_dwHeaderLen = 0;
		m_dwHeaderStart = 0;
		m_pData = NULL;
		m_dwDataLen = 0;
		m_dwFlags = 0;
		m_pCurrent = NULL;
	}


	// Use this function to retrieve an entity from a server via an HTTP
	// request. This function will either request a connection from the
	// server specified in the szURL parameter or request a connection from
	// the proxy server. If a proxy server is to be used, you must call
	// SetProxy prior to calling this function to specify the proxy server
	// being used. Once the connection is established, an HTTP request 
	// is built and sent to the HTTP server. An attempt to read the HTTP
	// response is then made. If the response is successfully read, the
	// response will be parsed and stored in this class instance. The 
	// headers can be parsed via the LookupHeader function and the body
	// of the respone can be retrieved using the GetBody function. You
	// can also retrieve the contents of the entire response by calling
	// GetResponse.
	//
	bool Navigate(
				const CUrl* pUrl,
				LPCTSTR szExtraHeaders=NULL,
				DWORD dwFlags=ATL_HTTP_FLAG_AUTO_REDIRECT,
				LPCTSTR szMethod=ATL_HTTP_METHOD_GET,
				BYTE *pData=NULL,
				DWORD dwDataLen=0,
				LPCTSTR szDataType=ATL_DEFAULT_DATA_TYPE,
				bool bProcessResult=true
				)
	{
		if (!pUrl)
			return false;

		TCHAR szUrl[ATL_URL_MAX_URL_LENGTH];
		DWORD dwMaxLen = ATL_URL_MAX_URL_LENGTH;
		if (!pUrl->CreateUrl(szUrl, &dwMaxLen))
			return false;

		// Navigate
		return Navigate(szUrl, dwFlags, szExtraHeaders, szMethod,
						(short)pUrl->GetPortNumber(), pData,
						dwDataLen, szDataType,  bProcessResult);

	}

	bool Navigate(
				LPCTSTR szServer,
				LPCTSTR szPath, 
				LPCTSTR szExtraHeaders=NULL,
				DWORD dwFlags=ATL_HTTP_FLAG_AUTO_REDIRECT,
				LPCTSTR szMethod=ATL_HTTP_METHOD_GET,
				short nPort = ATL_URL_DEFAULT_HTTP_PORT,
				BYTE *pData=NULL,
				DWORD dwDataLen=0,
				LPCTSTR szDataType=ATL_DEFAULT_DATA_TYPE,
				bool bProcessResult=true
				)
	{
		// Create a URL
		CUrl url;
		url.SetScheme(ATL_URL_SCHEME_HTTP);
		url.SetHostName(szServer);
		url.SetUrlPath(szPath);
		url.SetPortNumber(nPort);
		TCHAR szUrl[ATL_URL_MAX_URL_LENGTH];
		DWORD dwMaxLen = ATL_URL_MAX_URL_LENGTH;
		if (!url.CreateUrl(szUrl, &dwMaxLen))
			return false;

		// Navigate
		return Navigate(szUrl, dwFlags, szExtraHeaders, szMethod, nPort, pData,
			dwDataLen, szDataType,  bProcessResult);
	}

	bool Navigate(
				LPCTSTR szURL,
				DWORD dwFlags=ATL_HTTP_FLAG_AUTO_REDIRECT,
				LPCTSTR szExtraHeaders=NULL,
				LPCTSTR szMethod=ATL_HTTP_METHOD_GET,
				short nPort=ATL_URL_DEFAULT_HTTP_PORT,
				BYTE *pData=NULL,
				DWORD dwDataLen=0,
				LPCTSTR szDataType=ATL_DEFAULT_DATA_TYPE,
				bool bProcessResult=true
				) throw()
	{
		m_strMethod = szMethod;
		m_dwFlags = dwFlags;

		// set m_urlCurrent
		if (!SetDefaultUrl(szURL, nPort))
			return false;

		// connect to the correct server
		if (GetProxy())
		{
			//if we're using a proxy connect to the proxy
			if (!Connect(m_strProxy, m_nProxyPort))
				return false;
		}
		else
			if (!Connect(m_urlCurrent.GetHostName(),
				m_urlCurrent.GetPortNumber())) // connect to the server
				return false;
	
		DWORD dwSent = 0;
		CString strRequest;
		CString strExtraInfo;
		
		BuildRequest(&strRequest, szMethod,
						m_urlCurrent.GetHostName(), 
						m_urlCurrent.GetUrlPath(),
						m_urlCurrent.GetExtraInfo(),
						pData, dwDataLen, szDataType,
						szExtraHeaders);

		// m_dwDataLen is calculated in the previous call to BuildReqeust
		// dwAvailable represents the length in bytes of the data that
		// needs to be sent to the server. After the call to the socket's
		// Write function below, dwSent will contain the actual number of bytes
		// sent to the server
		DWORD dwAvailable = strRequest.GetLength() + m_dwDataLen;
		dwSent = dwAvailable;

		WSABUF Buffers[2];
		int nCount = 0;;
		CT2A pRequest(strRequest);

		if (m_pData)
		{
			Buffers[0].buf = pRequest;
			Buffers[0].len = strRequest.GetLength();
			Buffers[1].buf = (char*)m_pData;
			Buffers[1].len = m_dwDataLen;
			nCount = 2;
		}
		else
		{
			Buffers[0].buf = pRequest;
			Buffers[0].len = strRequest.GetLength();
			nCount = 1;
		}
		if (!Write(Buffers, nCount, &dwSent))
			return false;

		strRequest.ReleaseBuffer();

		// make sure everything was sent
		if (dwSent != dwAvailable)
			return false;

		// Read the response
		if (ReadHttpResponse())
		{
			// if navigation isn't complete, try to complete
			// it based on the status code and flags
			if (bProcessResult && !ProcessStatus(dwFlags))
			{
				return false;
			}
			return true;
		}
		return false;
	}

	bool BuildRequest(/*out*/CString *pstrRequest, LPCTSTR szMethod,
		LPCTSTR szServer, LPCTSTR szPath,
		LPCTSTR szExtraInfo=NULL, const BYTE* pData=NULL, DWORD dwDataLen=0,
		LPCTSTR szDataType=NULL, LPCTSTR szExtraHeaders=NULL) throw()
	{
		// build up the request
		CString strRequest = szMethod;
		strRequest += _T(" ");
		if (GetProxy())
		{
			CUrl urlRequest;
			urlRequest.SetScheme(ATL_URL_SCHEME_HTTP);
			urlRequest.SetHostName(szServer);
			urlRequest.SetUrlPath(szPath);
			urlRequest.SetPortNumber(ATL_URL_DEFAULT_HTTP_PORT);

			TCHAR buffURL[ATL_URL_MAX_URL_LENGTH];
			DWORD dwSize = ATL_URL_MAX_URL_LENGTH;
			urlRequest.CreateUrl(buffURL, &dwSize);
			strRequest += buffURL;
			if (szExtraInfo)
				strRequest += szExtraInfo;

			strRequest += ATL_HTTP_HEADER_PROXY;
			CString strHost;
			strHost.Format(_T("Host: %s\r\n"), szServer);
			strRequest += strHost;

			if (szDataType && dwDataLen>0)
			{
				CString strCL;
				strCL.Format(
					_T("Content-Type: %s\r\n")
					_T("Content-Length: %d\r\n"),szDataType, dwDataLen);
				strRequest += strCL;
			}
			if (szExtraHeaders)
				strRequest += szExtraHeaders;
			strRequest += ATL_HTTP_FOOTER;
		}
		else
		{
			strRequest += szPath;
			if (szExtraInfo)
				strRequest += szExtraInfo;
			strRequest += ATL_HTTP_HEADER;

			if (szDataType && dwDataLen>0)
			{
				CString strCL;
				strCL.Format(
					_T("Content-Type: %s\r\n")
					_T("Content-Length: %d\r\n"),szDataType, dwDataLen);
				strRequest += strCL;
			}
			if (szExtraHeaders)
				strRequest += szExtraHeaders;

			CString strHost;
			strHost.Format(_T("Host: %s\r\n"), szServer);
			strRequest += strHost;
			strRequest += ATL_HTTP_FOOTER;
		}
		strRequest += _T("\r\n");

		if (dwDataLen && pData)
		{
			m_pData = (BYTE*)pData;
			m_dwDataLen = dwDataLen;
			m_strDataType = szDataType;
		}

		*pstrRequest = strRequest;
		return true;
	}


	bool ReadSocket()
	{
		bool bRet = false;
		unsigned char read_buff[ATL_READ_BUFF_SIZE];
		int dwSize = ATL_READ_BUFF_SIZE;

		// read some data
		bRet = Read(read_buff, (DWORD*)&dwSize);
		if (bRet)
		{
			if (dwSize > 0)
			{
				// append the data to our internal buffer
				// m_current holds bytes (not UNICODE!)
				m_current.Append((LPCSTR)read_buff, dwSize);
				if (!m_pCurrent)
					m_pCurrent = (BYTE*)(LPCSTR)m_current;
			}
			else
				bRet = false; // nothing was read
		}
		return bRet;
	}

	// Starts searching for a complete header set at
	// m_pCurrent. This function will only move m_pCurrent
	// if a complete set is found. Returns the header beginning
	// optionally.
	unsigned char* FindHeaderEnd(unsigned char** ppBegin)
	{
		if (!m_pCurrent)
			return NULL;

		BYTE *pCurr = m_pCurrent;
		BYTE *pBegin = m_pCurrent;
		int nLen = m_current.GetLength();

		if (pCurr >= (BYTE*)(LPCSTR)m_current + m_current.GetLength())
			return NULL; // no more chars in buffer
		// look for the end of the header (the \r\n\r\n)
		while (pCurr < (pCurr + nLen - ATL_HEADER_END_LEN - 1))
		{
			if (*((DWORD*)pCurr) == ATL_DW_HEADER_END)
			{
				// set m_pCurrent pointer to the end of the header
				m_pCurrent = pCurr + ATL_HEADER_END_LEN;
				if (ppBegin)
					*ppBegin = pBegin;
				return m_pCurrent;
			}
			pCurr++;
		}
		return NULL;
	}

	// Call this function after sending an HTTP request over the socket. The complete
	// HTTP response will be read. This function will also parse
	// response headers into the response header map.
	bool ReadHttpResponse() throw()
	{
		// Read until we at least have the response headers
		bool bRet = true;
		readstate state = rs_init;
		unsigned char *pBodyBegin = NULL;
		unsigned char *pHeaderBegin = NULL;
		m_current.Empty();
		m_pCurrent = NULL;

		while (state != rs_complete)
		{

			switch(state)
			{
			case rs_init:
				m_HeaderMap.RemoveAll();
				m_nStatus = ATL_INVALID_STATUS;
				m_dwHeaderLen = 0;
				m_dwBodyLen = 0;
				state = rs_readheader;
				// fall through

			case rs_readheader:

				// read from the socket until we have a complete set of headers.
				pBodyBegin = FindHeaderEnd(&pHeaderBegin);
				if (!pBodyBegin)
				{
					if (!ReadSocket())
					{
						// Either reading from the socket failed, or there
						// was not data to read. Set the nav status to error
						// and change the state to complete.
						state = rs_complete;
						bRet = false;
						break;
					}
					else
						break; // loop back and FindHeaderEnd again.
				}
				// we have a complete set of headers
				m_dwHeaderLen = (DWORD)(pBodyBegin-pHeaderBegin);
				m_dwHeaderStart = (DWORD)(pHeaderBegin - (BYTE*)(LPCSTR)m_current);
				// fall through
				state = rs_scanheader;

			case rs_scanheader:
				// set m_nStatus and check for valid status
				ParseStatusLine(pHeaderBegin);
				// failed to set m_nStatus;
				if (m_nStatus == ATL_INVALID_STATUS)
				{
					state = rs_complete;
					bRet = false;
					break;
				}

				else if (m_nStatus == 100)
				{
					state = rs_init;
					break;
				}

				// crack all the headers and put them into a header map.
				if (ATL_HEADER_PARSE_COMPLETE != CrackResponseHeader((LPCSTR)pHeaderBegin, 
					(LPCSTR*)&pBodyBegin))
				{
					// something bad happened while parsing the headers!
					state = rs_complete;
					bRet = false;
					break;
				}
				state = rs_readbody;
				// fall through

			case rs_readbody:
				// headers are parsed and cracked, we're ready to read the rest
				// of the response. 
				if (!ReadBody(GetContentLength(), m_current.GetLength()-(m_dwHeaderStart+m_dwHeaderLen)))
					bRet =false;
				state = rs_complete;
				//fall through

			case rs_complete:
				// clean up the connection if the server requested a close;
				DisconnectIfRequired();
				break;
			}
		}
		return bRet;
	}

	// Checks to see if the server has closed the connection.
	// If it has, we create a new socket and reconnect it to
	// the current server. This also clears the contents of the
	// current response buffer.
	void ResetConnection() throw()
	{
		ReconnectIfRequired();
		m_HeaderMap.RemoveAll();
		m_current.Empty();
		m_nStatus = ATL_INVALID_STATUS;
	}

	// Takes action based on the flags passed and the current
	// status for this object.
	virtual bool ProcessStatus(DWORD dwFlags, bool bNoAuth = false) throw()
	{
		switch(m_nStatus)
		{
		case 200: // In all these cases there is no further action
		case 201: // to take. Any additional informaion is returned
		case 202: // in the entity body.
		case 203:
		case 204:
		case 205:
		case 206:
		case 304:
		case 305:
			return true;
			break;
		case 301:
		case 302:
		case 303:
			if (dwFlags & ATL_HTTP_FLAG_AUTO_REDIRECT)
				return ProcessObjectMoved();
			break;
		case 401:
			if (GetAuthObj() && !bNoAuth)
				return ProcessAuthRequired();
			break;

		}
		return false;
	}

	// Looks up the value of a response header in the header map. Call with
	// NULL szBuffer to have length of the required buffer placed in 
	// pdwLen on output.

	// szName is the name of the header to look up.
	// szBuffer is the buffer that will contain the looked up string.
	// pdwLen contains the length of szBuffer in characters on input and the length
	// of the string including NULL terminator in characters on output.
	bool GetHeaderValue(LPCTSTR szName, LPTSTR szBuffer, int *pdwLen) const throw()
	{
		CString strValue;
		bool bRet = m_HeaderMap.Lookup(szName, strValue);
		int nLen = strValue.GetLength();
		if (!bRet || !pdwLen)
			return false;

		if (*pdwLen < nLen+1 || !szBuffer)
		{
			*pdwLen = nLen+1;
			return szBuffer ? false : true;
		}

		_tcsncpy(szBuffer, (LPCTSTR)strValue, nLen+1);
		*pdwLen = nLen+1;
		return true;
	}

	// Sets the current authorization object for this socket.
	void SetAuthObj(const CAtlBaseAuthObject *pObject) throw()
	{
		if (!pObject)
			m_pAuthObject = NULL;
		else
		{
			m_pAuthObject = (CAtlBaseAuthObject*)pObject;
			m_pAuthObject->Init(this);
		}
	}

	// Retrieves a pointer to the current authorization object
	// for this socket.
	const CAtlBaseAuthObject* GetAuthObj() const throw()
	{
		return m_pAuthObject;
	}

	// Sets the current proxy server and port
	void SetProxy(LPCTSTR szProxy, short nProxyPort) throw()
	{
		m_strProxy = szProxy;
		m_nProxyPort = nProxyPort;
	}

	// retrieves the current proxy
	LPCTSTR GetProxy() const throw()
	{
		if (m_strProxy.GetLength())
			return (LPCTSTR)m_strProxy;
		return NULL;
	}

	// Gets the contents of the entire response buffer.
	const BYTE* GetResponse() throw()
	{
		return (const BYTE*)(LPCSTR)m_current;
	}
	
	// Gets the length in bytes of the body of the
	// current response
	DWORD GetBodyLength() const throw()
	{
		return m_dwBodyLen;
	}

	// Gets the contents of the body of the current response. This
	// is the response without the headers. 
	const BYTE* GetBody() throw()
	{
		return (BYTE*)((LPCSTR)m_current+m_dwHeaderStart+m_dwHeaderLen);
	}

	// Get the length of the header part of the response in bytes.
	DWORD GetHeaderLength()
	{
		return m_dwHeaderLen-2; // m_dwHeaderLen includes the final \r\n
	}

	// buffer must include space for null terminator.
	// on input, pdwLen specifies the size of szBuffer,
	// on output, pdwLen holds the number of bytes copied
	// to szBuffer, or the required size of szBuffer if 
	// szBuffer wasn't big enough
	bool GetRawRequestHeaders(LPBYTE szBuffer, int *pdwLen)
	{
		if (!pdwLen)
			return false;

		int header_len = GetHeaderLength();
		if (!szBuffer || *pdwLen < header_len+1)
		{
			*pdwLen = header_len+1;
			return false;
		}

		memcpy(szBuffer, (BYTE*)(LPCSTR)m_current, header_len);
		szBuffer[header_len]='\0';

		*pdwLen = header_len+1;
		return true;
	}

	// Gets the current URL object.
	LPCURL GetCurrentUrl() const throw()
	{
		return (LPCURL)&m_urlCurrent;
	}

	bool SetDefaultUrl(LPCTSTR szUrl, 
		short nPortNumber=ATL_URL_DEFAULT_HTTP_PORT) throw()
	{
		return _SetDefaultUrl(szUrl,nPortNumber);
	}

	bool SetDefaultUrl(LPCURL pUrl, 
		short nPortNumber=ATL_URL_DEFAULT_HTTP_PORT) throw()
	{
		m_urlCurrent = *pUrl;
		return _SetDefaultUrl(NULL, nPortNumber);
	}

	void SetDefaultMethod(LPCTSTR szMethod) throw()
	{
		m_strMethod = szMethod;
	}

	LPCTSTR GetDefaultMethod() const throw()
	{
		return m_strMethod;
	}

	void SetFlags(DWORD dwFlags) throw()
	{
		m_dwFlags = dwFlags;
	}

	DWORD GetFlags() const throw()
	{
		return m_dwFlags;
	}

// Implementation
private:
	bool DisconnectIfRequired()
	{
		CString strValue;
		if (m_HeaderMap.Lookup(_T("Connection"), strValue))
		{
			if (!strValue.CompareNoCase(_T("close")))
			{
				Close();
			}
		}
		return true;
	}
public:
	long GetContentLength() throw()
	{
		CString strValue;
		if (m_HeaderMap.Lookup(_T("Content-Length"), strValue))
		{
			TCHAR *pStop = NULL;
			return _tcstol(strValue, &pStop, 10);
		}
		else
			return -1;
	}

	LPCSTR NextLine(BYTE* pCurr) throw()
	{
		if (!pCurr)
			return NULL;

		while ( *pCurr && !(*pCurr == '\r' && *(pCurr+1) == '\n'))
			pCurr++;

		if (!memcmp(pCurr, ATL_HEADER_END, 4))
			return NULL;

		return (LPCSTR)(pCurr+2);
	}

	bool IsMsgBodyChunked() throw()
	{
		CString strValue;
		return (
			m_HeaderMap.Lookup(
				_T("Transfer-Encoding"), strValue) &&
				strValue == _T("chunked") // m_HeaderMap lower cases all values before storing
				);

	}

	// finds the end of an individual header field pointed to by
	// pszStart. Header fields can be multi-line with multi-line 
	// header fields being a line that starts with some kind of 
	// white space.
	LPCSTR FindEndOfHeader(LPCSTR pszStart) throw()
	{
		// move through all the lines until we come to one
		// that doesn't start with white space
		LPCSTR pLineStart = pszStart;
		LPCSTR pHeaderEnd = NULL;

		do 
		{
			pLineStart = NextLine((BYTE*)pLineStart);
		}while (pLineStart && isspace(*pLineStart));

		//if we reach the end of all headers then pLineStart 
		// will be NULL
		if (pLineStart)
			pHeaderEnd = pLineStart-2;
		else // on last header
		{
			pHeaderEnd = strstr(pszStart, ATL_HEADER_END);
		}

		return pHeaderEnd;
	}

	void DecodeHeader(LPCSTR pHeaderStart, LPCSTR pHeaderEnd) throw()
	{
		if (!pHeaderStart || !pHeaderEnd)
			return;
		LPCSTR pTemp = pHeaderStart;
		while (*pTemp != ATL_FIELDNAME_DELIMITER && pTemp < pHeaderEnd)
			pTemp++;
		if (*pTemp == ATL_FIELDNAME_DELIMITER)
		{
			char szName[ATL_MAX_FIELDNAME_LEN];
			char szValue[ATL_MAX_VALUE_LEN];
			int nLen = (int)(pTemp-pHeaderStart) ;
			ATLASSERT(nLen < ATL_MAX_FIELDNAME_LEN);
			memcpy(szName, pHeaderStart, nLen);
			szName[nLen]=0;

			pTemp++; // move past delimiter;
			while (isspace(*pTemp) && pTemp < pHeaderEnd)
				pTemp++;

			nLen = (int)(pHeaderEnd-pTemp);
			ATLASSERT(nLen < ATL_MAX_VALUE_LEN);
			memcpy(szValue, pTemp, nLen);
			szValue[nLen]=0;

			CString strExist;
			CA2T pszName(szName);
			CA2T pszValue(szValue);

			if (!m_HeaderMap.Lookup(pszName, strExist))
				m_HeaderMap.SetAt(pszName, pszValue);
			else
			{	
				// field-values for headers with the same name can be appended
				// per rfc2068 4.2, we do the appending so we don't have to
				// store/lookup duplicate keys.
				strExist += ',';
				strExist += pszValue;
				m_HeaderMap.SetAt(pszName, (LPCTSTR)strExist);
			}

			// if it's a set-cookie header notify users so they can do 
			// somthing with it.
			if (!_tcsicmp(pszName, _T("set-cookie")))
				OnSetCookie(pszValue);
		}
	}

	virtual void OnSetCookie(LPCTSTR /*szCookie*/)
	{
		return;
	}

	LPCSTR ParseStatusLine(BYTE* pBuffer) throw()
	{
		if (!pBuffer)
			return NULL;
		// find the first space'
		while (!isspace(*pBuffer))
			pBuffer++;

		// move past the space
		while (isspace(*pBuffer))
			pBuffer++;

		// should be pointing at the status code
		LPCSTR pEnd = NULL;
		m_nStatus = strtol((LPSTR)pBuffer, (LPSTR*)&pEnd, 10);

		// move to end of line
		while (*pBuffer !=  '\n')
			pBuffer++;

		// set the return pointing to the first 
		// character after our status line.
		return (LPCSTR)++pBuffer;
	}


	// pBuffer should start at the first character
	// after the status line.
	int CrackResponseHeader(LPCSTR pBuffer, /*out*/ LPCSTR *pEnd) throw()
	{
		// read up to the double /r/n
		LPCSTR pszStartSearch = pBuffer;
		if (!pEnd)
			return ATL_HEADER_PARSE_HEADERERROR;

		*pEnd = NULL;
		if (pszStartSearch == NULL)
			return ATL_HEADER_PARSE_HEADERERROR;

		// check to see if we have a complete set of headers
		if (!strstr(pszStartSearch, ATL_HEADER_END))
			return ATL_HEADER_PARSE_HEADERNOTCOMPLETE;

		// start parsing headers
		LPCSTR pHeaderStart = ParseStatusLine((BYTE*)pBuffer);
		LPCSTR pHeaderEnd = NULL;

		while (pHeaderStart)
		{
			pHeaderEnd = FindEndOfHeader(pHeaderStart);
			if (!pHeaderEnd)
				break;
			DecodeHeader(pHeaderStart, pHeaderEnd);

			if (!strncmp(pHeaderEnd, ATL_HEADER_END, strlen(ATL_HEADER_END)))
			{
				*pEnd = pHeaderEnd + ATL_HEADER_END_LEN;
				break; 		// we're done
			}
			else
				pHeaderStart = pHeaderEnd+2;
		}

		return ATL_HEADER_PARSE_COMPLETE;		
	}

public:

	// Reads the body if the encoding is not chunked.
	bool ReadBody(int nContentLen, int nCurrentBodyLen) throw()
	{
		// nCurrentBodyLen is the length of the body that has already been read
		// nContentLen is the value of Content-Length
		// current is the buffer that will contain the entire response
		bool bRet = true;

		if (nContentLen != -1) // We know the content length.
		{
			// read the rest of the body.
			BYTE readbuff[ATL_READ_BUFF_SIZE];
			while (nCurrentBodyLen < nContentLen)
			{
				DWORD dwRead = ATL_READ_BUFF_SIZE-1;
				if (!Read(readbuff, &dwRead))
					return false;
				else
				{
					nCurrentBodyLen += dwRead;
					m_current.Append((LPCSTR)readbuff, dwRead);
				}
			}
			m_dwBodyLen = nCurrentBodyLen;
		}
		else // We don't know content length. All we can do is
		{	 // read until there is nothing else to read.
		
			// read the rest of the body.
			DWORD dwRead = ATL_READ_BUFF_SIZE-1;
			BYTE readbuff[ATL_READ_BUFF_SIZE];
			do
			{
				if (Read((BYTE*)readbuff, (DWORD*)&dwRead))
				{
					nCurrentBodyLen += dwRead;
					m_current.Append((LPCSTR)readbuff, dwRead);
				}
				else 
				{
					bRet = true;
					break;
				}
			}while (dwRead);
			m_dwBodyLen = nCurrentBodyLen;
		}
		return bRet;
	}

	bool ReconnectIfRequired() throw()
	{
		CString strValue;
		// if we have a keep-alive header then return true
		// else we have to close and re-open the connection
		if (!m_HeaderMap.Lookup(_T("Connection"), strValue)
			|| !strValue.CompareNoCase(_T("keep-alive")))
			return true; // No connection header or server said stay open.

		if (!strValue.CompareNoCase(_T("close")))
		{
			Close();
			if (Create())
			{
				if (GetProxy())
					return Connect(m_strProxy, m_nProxyPort);
				else
					return Connect(m_urlCurrent.GetHostName(), m_urlCurrent.GetPortNumber());
			}
		}	
		return false;
	}

	bool ProcessAuthRequired() throw()
	{
		CString strValue;
		// must have an authenticate header
		if (!m_HeaderMap.Lookup(_T("WWW-Authenticate"), strValue))
			return false;

		// if we got multiple WWW-Authenticate headers, their values
		// will have been appended together in a comma separated list
		// by our DecodeHeader function
		if (!strValue.GetLength())
			return false;

		if (m_pAuthObject)
			return m_pAuthObject->Authenticate(strValue);
		return false;
	}

	// Complete relative URLs and URLs
	// that have a missing path. These are common with redirect headers.
	// http://www.microsoft.com becomes http://www.microsoft.com/
	// localstart.asp becomes whatever our current (m_urlCurrent) 
	// path is plus localstart.asp
	bool CompleteURL(CString& strURL) throw()
	{
		CString strUrlTemp = strURL;
		CUrl url;
		bool bErr = false;
		if (url.CrackUrl(strUrlTemp))
		{
			return true; // URL is already valid
		}


		// if we have a scheme and a host name but no
		// path, then add the path of '/'
		if (url.GetScheme() == ATL_URL_SCHEME_HTTP &&
			url.GetHostNameLength() > 0 &&
			!url.GetUrlPathLength() )
		{
			url.SetUrlPath(_T("/"));
			bErr = true;
		}
		// if we don't have a scheme or host name or path we derive
		// the url from our current URL (m_urlCurrent) and add
		// our relative paths
		else if (url.GetScheme() == ATL_URL_SCHEME_UNKNOWN &&
			url.GetHostNameLength() == 0 &&
			url.GetUrlPathLength() == 0)
		{
			TCHAR szPath[ATL_URL_MAX_PATH_LENGTH];
			szPath[0]=0;
			url = m_urlCurrent;


			if (!url.GetUrlPathLength())
				_tcscpy(szPath, _T("/")); // current URL has no path!
			else
				_tcscpy(szPath, url.GetUrlPath());

			// back up to the first / and insert our current url
			TCHAR* pBuff = _tcsrchr(szPath,  _T('/'));
			pBuff++;
			if (!(*pBuff))
				_tcscat(szPath, (LPCTSTR)strURL);
			else
				_tcscat(pBuff, (LPCTSTR)strURL);

			url.SetUrlPath(szPath);
			bErr = true;
		}
		if (!bErr)
			return bErr;
		DWORD dwLen = ATL_URL_MAX_PATH_LENGTH;

		return url.CreateUrl(strURL.GetBuffer(ATL_URL_MAX_PATH_LENGTH),
			&dwLen) ? true : false;
	}

	bool ProcessObjectMoved() throw()
	{
		// look for a location header
		CString strValue;
		CString strURLNew;
		if (m_HeaderMap.Lookup(_T("Location"), strValue))
		{
			ReconnectIfRequired();
			m_HeaderMap.RemoveAll();
			m_current.Empty();


			// create a new URL based on what is in the
			// Location header and set it as this object's 
			// default Url
			strURLNew = strValue;
			CompleteURL(strURLNew);
			SetDefaultUrl((LPCTSTR)strURLNew, m_urlCurrent.GetPortNumber());
			
			// build up a request			
			CString strRequest;
			BuildRequest(&strRequest, m_strMethod, 
				m_urlCurrent.GetHostName(),
				m_urlCurrent.GetUrlPath(), 
				m_urlCurrent.GetExtraInfo(), m_pData, m_dwDataLen, m_strDataType);

			// send the request
			DWORD dwSent = strRequest.GetLength();
			DWORD dwAvailable = dwSent;
			if (!Write((BYTE*)((LPCSTR)CT2A(strRequest.GetBuffer(dwAvailable))), &dwSent))
				return false;
			strRequest.ReleaseBuffer();

			if (dwSent != dwAvailable)
				return false;

			// read the response
			if (ReadHttpResponse())
			{
				ProcessStatus(m_dwFlags);
			}
		}
		return true;
	}

	bool _SetDefaultUrl(LPCTSTR szURL, short nPort) throw()
	{

		if (szURL)
			if (!m_urlCurrent.CrackUrl(szURL)) // re-inits the field of the CUrl first
				return false;

		if (ATL_URL_SCHEME_HTTP != m_urlCurrent.GetScheme())
			return false; // only support HTTP

		if (!m_urlCurrent.GetUrlPathLength())
		{
			// no path, default to /
			m_urlCurrent.SetUrlPath(_T("/"));
		}

		if (!m_urlCurrent.GetHostNameLength())
		{
			// no server name
			return false;
		}

		if (m_urlCurrent.GetPortNumber() == ATL_URL_INVALID_PORT_NUMBER)
			m_urlCurrent.SetPortNumber(nPort);
		return true;
	}
public:
	int GetStatus()
	{
		return m_nStatus;
	}
	LPCTSTR GetMethod()
	{
		return m_strMethod;
	}

	BYTE *GetData()
	{
		return m_pData;
	}

	DWORD GetDataLen()
	{
		return m_dwDataLen;
	}

	LPCTSTR GetDataType()
	{
		return m_strDataType;
	}
};

bool inline CDefaultAuthObject::DoNTLMAuthenticate()
{
	bool bRet = false;

	// make sure we have a good credentials handle
	ATLASSERT(!ATL_IS_INVALIDCREDHANDLE(m_hCredentials));
	if (ATL_IS_INVALIDCREDHANDLE(m_hCredentials))
		return false;

	SECURITY_STATUS SecurityStatus = SEC_E_OK;

	unsigned long ContextAttributes = 0;
	SecBufferDesc OutBufferDesc = {0,0,0};
	CtxtHandle SecurityContext = {0, 0};
	SecBuffer OutSecBuffer = {0, 0, 0};
	unsigned char securitybuffer[ATL_AUTH_HDR_SIZE];


	OutBufferDesc.ulVersion = 0;
	OutBufferDesc.cBuffers = 1;
	OutBufferDesc.pBuffers = &OutSecBuffer;

	OutSecBuffer.cbBuffer = m_nMaxTokenSize;
	OutSecBuffer.BufferType = SECBUFFER_TOKEN;
	OutSecBuffer.pvBuffer = securitybuffer;

	SecurityStatus = InitializeSecurityContext(
				&m_hCredentials,
				0,
				ATL_HTTP_AUTHTYPE_NTLM,
				ISC_REQ_USE_DCE_STYLE | ISC_REQ_DELEGATE |
				ISC_REQ_MUTUAL_AUTH |ISC_REQ_REPLAY_DETECT |
				ISC_REQ_SEQUENCE_DETECT |ISC_REQ_CONFIDENTIALITY |
				ISC_REQ_CONNECTION,
				0,
				0,
				0,
				0,
				&SecurityContext,
				&OutBufferDesc,
				&ContextAttributes,
				&m_ts
				);

	if ( (SecurityStatus == SEC_I_COMPLETE_NEEDED) ||
		 (SecurityStatus == SEC_I_COMPLETE_AND_CONTINUE) )
	{
		SecurityStatus = CompleteAuthToken( &SecurityContext, &OutBufferDesc );
	}

	if (IS_ERROR(SecurityStatus))
		return false;

	// create an Authentication header with the contents of the
	// security buffer and send it to the HTTP server. The output
	// buffer will be pointing to a buffer that contains the 
	// response from the HTTP server on return.
	LPSTR pszbuff = NULL;
	if (!SendSecurityInfo(&OutSecBuffer, &pszbuff) || !pszbuff)
		return false;


	char *szResponsecode = strstr(pszbuff, "WWW-Authenticate: NTLM");
	char pszcode[ATL_AUTH_HDR_SIZE];
	if (szResponsecode)
	{
		szResponsecode += strlen("WWW-Authenticate: NTLM ");
		char *pszend = strstr(szResponsecode, "\r\n");
		bRet = false;
		int nsize = 0;
		if (pszend)
		{
			// copy authentication data to our buffer
			// and base64decode it.
			int nlen = (int)(pszend-szResponsecode);
			memcpy(pszcode, szResponsecode, nlen );
			pszcode[pszend-szResponsecode]=0;
			nsize = m_nMaxTokenSize;
			ZeroMemory(securitybuffer, ATL_AUTH_HDR_SIZE);
			bRet = Base64Decode(pszcode, (int) strlen(pszcode), securitybuffer, &nsize) != FALSE;
		}

		if (!bRet)
			return false;
	
		// Create buffers for the challenge data
		SecBufferDesc OutBufferDesc2, InBufferDesc;
		SecBuffer OutSecBuffer2, InSecBuffer;
		unsigned char Buffer[ATL_AUTH_HDR_SIZE];
		ZeroMemory(Buffer, ATL_AUTH_HDR_SIZE);
		OutBufferDesc2.ulVersion = 0;
		OutBufferDesc2.cBuffers = 1;
		OutBufferDesc2.pBuffers = &OutSecBuffer2;

		OutSecBuffer2.cbBuffer = ATL_AUTH_HDR_SIZE;
		OutSecBuffer2.BufferType = SECBUFFER_TOKEN;
		OutSecBuffer2.pvBuffer = Buffer;

		InBufferDesc.ulVersion = 0;
		InBufferDesc.cBuffers = 1;
		InBufferDesc.pBuffers = &InSecBuffer;

		InSecBuffer.cbBuffer = nsize;
		InSecBuffer.BufferType = SECBUFFER_TOKEN;
		InSecBuffer.pvBuffer = securitybuffer;

		SecurityStatus = InitializeSecurityContext(
					0,
					&SecurityContext,
					ATL_HTTP_AUTHTYPE_NTLM,
					0,
					0,
					0 ,
					&InBufferDesc,
					0,
					&SecurityContext,
					&OutBufferDesc2,
					&ContextAttributes,
					&m_ts
					);

		if (IS_ERROR(SecurityStatus))
			return false;
		pszbuff = NULL;
		if (SendSecurityInfo(&OutSecBuffer2, &pszbuff))
		{
			// at this point we should be authenticated and either have the page
			// we requested or be getting re-directed to another page under our
			// authorization. Either way, we don't want to go through authorization
			// code again if we are not authorized to prevent recursive authorization
			// so we send a flag saying not to process authorization status' again.
			bRet = m_pSocket->ProcessStatus(m_pSocket->GetFlags(), true);
		}

	}

	return bRet;

}
bool inline CDefaultAuthObject::SendSecurityInfo(SecBuffer *pSecBuffer, LPSTR *pszBuffer)
{
	ATLASSERT(pSecBuffer);
	ATLASSERT(m_pSocket);
	ATLASSERT(pszBuffer);

	int nDest = ATL_AUTH_HDR_SIZE;
	char auth_b64encoded[ATL_AUTH_HDR_SIZE];
	char auth_header[ATL_AUTH_HDR_SIZE];
	char *pszFmtStr = "Authorization: NTLM %s\r\n";

	if (!pSecBuffer || !pSecBuffer->pvBuffer || !pszBuffer)
		return false;
	*pszBuffer = 0;

	// Base64Encode will fail gracefully if buffer not big enough
	if (Base64Encode((BYTE*)pSecBuffer->pvBuffer, pSecBuffer->cbBuffer,
		auth_b64encoded, &nDest, ATL_BASE64_FLAG_NOCRLF))
	{
		if (nDest < ATL_AUTH_HDR_SIZE)
		{
			auth_b64encoded[nDest]=0;
			// make sure we have enough room in our header buffer
			if ( (strlen(pszFmtStr)-2 + nDest) < ATL_AUTH_HDR_SIZE)
				sprintf(auth_header, pszFmtStr, auth_b64encoded);
			else
				return false;
		}
		else
			return false;
	}
	else
		return false;

	// reset the connection if required
	m_pSocket->ResetConnection();

	// Resend the request with the authorization information
	CStringA strRequest;
	LPCURL pUrl = m_pSocket->GetCurrentUrl();	
	bool bRet = false;

	TCHAR szUrl[ATL_URL_MAX_URL_LENGTH];
	DWORD dwMaxLen = ATL_URL_MAX_URL_LENGTH;
	pUrl->CreateUrl(szUrl, &dwMaxLen);

	bRet = m_pSocket->Navigate(
						szUrl,
						ATL_HTTP_FLAG_AUTO_REDIRECT,
						CA2T(auth_header),
						m_pSocket->GetMethod(),
						pUrl->GetPortNumber(),
						m_pSocket->GetData(),
						m_pSocket->GetDataLen(),
						m_pSocket->GetDataType(),
						false
						);
	if (bRet)
		*pszBuffer = (LPSTR)m_pSocket->GetResponse();
	return bRet;
}

bool inline CBasicAuthObject::DoBasicAuthenticate()
{
	bool bRet = false;
	ATLASSERT(m_pClient);
	ATLASSERT(m_pAuthInfo);
	// Create an authentication string
	char auth_string[ATL_MAX_BASIC_ID_LEN];
	char szUid[ATL_MAX_USERNAME];
	char szPwd[ATL_MAX_PWD];

	if (!m_pAuthInfo->GetPassword(szPwd, ATL_MAX_PWD) ||
		!m_pAuthInfo->GetUsername(szUid, ATL_MAX_USERNAME))
		return false;
	strcpy(auth_string, szUid);
	strcat(auth_string, ":");
	strcat(auth_string, szPwd);

	// Base64 encode the auth string
	char *auth_string_enc = NULL;
	int nLen = Base64EncodeGetRequiredLength((int)strlen(auth_string));
	auth_string_enc = (char*)alloca(nLen);
	if (!Base64Encode((const BYTE*)auth_string, (int)strlen(auth_string),
					  auth_string_enc, &nLen))
		return false;

	// Format the Authentication header
	nLen += (int)strlen(ATL_BASIC_AUTH_STRING) + 2;
	char *auth_header = (char*)alloca(nLen);
	strcpy(auth_header, ATL_BASIC_AUTH_STRING);
	strcat(auth_header, auth_string_enc);
	strcat(auth_header, "\r\n");

	// Resend the request with the authorization information
	LPCURL pUrl = m_pClient->GetCurrentUrl();	
	TCHAR szUrl[ATL_URL_MAX_URL_LENGTH];
	DWORD dwMaxLen = ATL_URL_MAX_URL_LENGTH;
	pUrl->CreateUrl(szUrl, &dwMaxLen);

	// reset the connection if required
	m_pClient->ResetConnection();

	bRet = m_pClient->Navigate(
						szUrl,
						ATL_HTTP_FLAG_AUTO_REDIRECT,
						CA2T(auth_header),
						m_pClient->GetMethod(),
						pUrl->GetPortNumber(),
						m_pClient->GetData(),
						m_pClient->GetDataLen(),
						m_pClient->GetDataType(),
						false
						);
	if (bRet)
	{
		// Request was successfully sent. Process the result.
		bRet = m_pClient->ProcessStatus(m_pClient->GetFlags(), true);
	}
	return bRet;
}


} // ATL


