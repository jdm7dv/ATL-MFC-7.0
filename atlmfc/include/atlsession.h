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

#include <atldbcli.h>
#include <atlcom.h>
#include <atlstr.h>
#include <stdio.h>
#include <atlcoll.h>
#include <atltime.h>
#include <atlcrypt.h>
#include <atlcrypt.inl>
#include <atlenc.h>
#include <atlutil.h>
#include <atlcache.h>

#ifndef SESSION_KEY_LENGTH 
	#define SESSION_KEY_LENGTH 37
#endif

#ifndef MAX_SESSION_KEY_LEN 
	#define MAX_SESSION_KEY_LEN 128
#endif

#ifndef MAX_VARIABLE_NAME_LENGTH 
	#define MAX_VARIABLE_NAME_LENGTH 50
#endif

#ifndef MAX_VARIABLE_VALUE_LENGTH 
	#define MAX_VARIABLE_VALUE_LENGTH 100
#endif

#ifndef DEFAULT_SQL_LEN
	#define DEFAULT_SQL_LEN 1024
#endif

#ifndef MAX_CONNECTION_STRING_LEN
	#define MAX_CONNECTION_STRING_LEN 2048
#endif

#ifndef SESSION_COOKIE_NAME
	#define SESSION_COOKIE_NAME "SESSIONID"
#endif

#ifndef ATL_SESSION_TIMEOUT
	#define ATL_SESSION_TIMEOUT 600000 //10 min
#endif

#ifndef ATL_SESSION_SWEEPER_TIMEOUT
	#define ATL_SESSION_SWEEPER_TIMEOUT 1000 // 1sec
#endif

#ifndef ATL_SESSIONREF_DELETE
	#define ATL_SESSIONREF_DELETE \
				_T( "DELETE FROM SessionReferences " )\
				_T( "WHERE SessionID=? AND RefCount <= 0 " )\
				_T( "AND DATEDIFF(millisecond,  LastAccess, getdate()) > TimeoutMs" )
#endif

#ifndef ATL_SESSIONREF_ISEXPIRED
	#define ATL_SESSIONREF_ISEXPIRED \
				_T( "SELECT SessionID FROM SessionReferences ")\
				_T( "WHERE (SessionID=?) AND (DATEDIFF(millisecond,  LastAccess, getdate()) > TimeoutMs)" )
#endif


#ifndef ATL_SESSIONREF_DELETEFINAL
	#define ATL_SESSIONREF_DELETEFINAL \
				_T( "DELETE FROM SessionReferences " )\
				_T( "WHERE SessionID=?" )
#endif


#ifndef ATL_SESSIONREF_CREATE
	#define ATL_SESSIONREF_CREATE \
				_T( "INSERT INTO SessionReferences " )\
				_T( "(SessionID, LastAccess, RefCount, TimeoutMs) " )\
				_T( "VALUES (?, getdate(), 1, ?)" )
#endif

#ifndef ATL_SESSIONREF_UPDATETIMEOUT
	#define ATL_SESSIONREF_UPDATETIMEOUT \
				_T( "UPDATE SessionReferences " )\
				_T( "SET TimeoutMs=? WHERE SessionID=?")
#endif


#ifndef ATL_SESSIONREF_ADDREF
	#define ATL_SESSIONREF_ADDREF \
				_T( "UPDATE SessionReferences " )\
				_T( "SET RefCount=RefCount+1, " )\
				_T( "LastAccess=getdate() " )\
				_T( "WHERE SessionID=?" )
#endif

#ifndef ATL_SESSIONREF_REMOVEREF
	#define ATL_SESSIONREF_REMOVEREF \
				_T( "UPDATE SessionReferences " )\
				_T( "SET RefCount=RefCount-1, " )\
				_T( "LastAccess=getdate() " )\
				_T( "WHERE SessionID=?" )
#endif

#ifndef ATL_SESSIONREF_ACCESS
	#define ATL_SESSIONREF_ACCESS \
				_T( "UPDATE SessionReferences " )\
				_T( "SET LastAccess=getdate() " )\
				_T( "WHERE SessionID=?" )
#endif

#ifndef ATL_SESSIONREF_SELECT
	#define ATL_SESSIONREF_SELECT \
				_T( "SELECT * FROM SessionReferences " )\
				_T( "WHERE SessionID=?" )
#endif

#ifndef ATL_SESSIONREF_GETCOUNT
	#define ATL_SESSIONREF_GETCOUNT \
				_T("SELECT COUNT(*) FROM SessionReferences")
#endif


#ifndef ATL_SESSVAR_GETCOUNT
	#define ATL_SESSVAR_GETCOUNT \
				_T("SELECT COUNT(*) FROM SessionVariables WHERE SessionID=?")
#endif

#ifndef ATL_SESSVAR_INSERT
	#define ATL_SESSVAR_INSERT \
				_T("INSERT INTO SessionVariables ")\
				_T("(VariableValue, VarType, SessionID, VariableName) ")\
				_T("VALUES (?, ?, ?, ?)")
#endif

#ifndef ATL_SESSVAR_UPDATE
	#define ATL_SESSVAR_UPDATE \
				_T("UPDATE SessionVariables ")\
				_T("SET VariableValue=?, VarType=? ")\
				_T("WHERE SessionID=? AND VariableName=?")
#endif

#ifndef ATL_SESSVAR_DELETEVAR
	#define ATL_SESSVAR_DELETEVAR \
				_T("DELETE FROM SessionVariables ")\
				_T("WHERE SessionID=? AND VariableName=?")
#endif

#ifndef ATL_SESSVAR_DELETEALLVARS
	#define ATL_SESSVAR_DELETEALLVARS \
				_T("DELETE FROM SessionVariables WHERE (SessionID=?)")
#endif

#ifndef ATL_SESSVAR_SELECTVAR
	#define ATL_SESSVAR_SELECTVAR \
				_T("SELECT SessionID, VariableName, VariableValue, VarType ")\
				_T("FROM SessionVariables ")\
				_T("WHERE SessionID=? AND VariableName=?")
#endif

#ifndef ATL_SESSVAR_SELECTALLVARS
	#define ATL_SESSVAR_SELECTALLVARS \
				_T("SELECT SessionID, VariableName, VariableValue, VarType ")\
				_T("FROM SessionVariables ")\
				_T("WHERE SessionID=?")
#endif

#define INVALID_DB_SESSION_POS 0x0
#define MIN_SESSION_NAME_SIZE 5
#define ATL_DBSESSION_ID _T("__ATL_SESSION_DB_CONNECTION")

namespace ATL {

__interface ATL_NO_VTABLE __declspec(uuid("DEB69BE3-7AC9-4a13-9519-266C1EA3AB39")) 
ISession : public IUnknown
{
	STDMETHOD(SetVariable)(LPCSTR szName, VARIANT NewVal);
	STDMETHOD(GetVariable)(LPCSTR szName, VARIANT *pVal);
	STDMETHOD(GetCount)(long *pnCount);
	STDMETHOD(RemoveVariable)(LPCSTR szName);
	STDMETHOD(RemoveAllVariables)();
	STDMETHOD(BeginVariableEnum)(HANDLE *phEnumHandle, POSITION *pPOS);
	STDMETHOD(GetNextVariable)(HANDLE hEnum, POSITION *pPOS, LPSTR szName, DWORD dwLen, VARIANT *pVal);
	STDMETHOD(CloseEnum)(HANDLE hEnumHandle);
	STDMETHOD(IsExpired)();
	STDMETHOD(SetTimeout)(unsigned __int64 dwNewTimeout);
}; //ISession

__interface ATL_NO_VTABLE __declspec(uuid("C5740C4F-0C6D-4b43-92C4-2AF778F35DDE"))
ISessionStateService : public IUnknown
{
	STDMETHOD(CreateNewSession)(LPSTR szNewID, DWORD *pdwSize, ISession** ppSession)= 0;
	STDMETHOD(GetSession)(LPCSTR szID, ISession **ppSession) = 0;
	STDMETHOD(CloseSession)(LPCSTR szID) = 0;
};

__interface ATL_NO_VTABLE __declspec(uuid("6C7F5F56-6CBD-49ee-9797-4C837D4C527A"))
ISessionStateControl : public IUnknown
{
	STDMETHOD(SetSessionTimeout)(unsigned __int64 nTimeoutMS) = 0;
	STDMETHOD(GetSessionTimeout)(unsigned __int64 *pnTimeouMS) = 0;
	STDMETHOD(GetSessionCount)(DWORD *pnSessionCount) = 0;
}; 


// CSessionNameGenerator
// This is a helper class that generates random data for session key
// names. This class tries to use the CryptoApi to generate random
// bytes for the session key name. If the CryptoApi isn't available
// then the CRT rand() is used to generate the random bytes. This
// class's GetNewSessionName member function is used to actually
// generate the session name.
class CSessionNameGenerator :
	public CCryptProv
{
public:
	bool m_bCryptNotAvailable;

	CSessionNameGenerator() throw() :
		m_bCryptNotAvailable(false)
	{
		// Note that the crypto api is being
		// initialized with no private key
		// information
		HRESULT hr = InitVerifyContext();
		m_bCryptNotAvailable = FAILED(hr) ? true : false;
	}

	// This function creates a new session name and base64 encodes it.
	// The base64 encoding algorithm used needs at least MIN_SESSION_NAME_SIZE
	// bytes to work correctly. Since we stack allocate the temporary
	// buffer that holds the key name, the buffer must be less than or equal to
	// the MAX_SESSION_KEY_LEN in size.
	HRESULT GetNewSessionName(LPSTR szNewID, DWORD *pdwSize) throw()
	{
		HRESULT hr = E_FAIL;

		if (!pdwSize)
			return E_POINTER;

		if (*pdwSize < MIN_SESSION_NAME_SIZE ||
			*pdwSize > MAX_SESSION_KEY_LEN)
			return E_INVALIDARG;

		if (!szNewID)
			return E_POINTER;
			
		BYTE key[MAX_SESSION_KEY_LEN];
		ZeroMemory(key, sizeof(key));

		// calculate the number of bytes that will fit in the
		// buffer we've been passed
		DWORD dwDataSize = (DWORD)CalcMaxInputSize(*pdwSize);

		if (dwDataSize && *pdwSize >= (DWORD)(Base64EncodeGetRequiredLength(dwDataSize,
			ATL_BASE64_FLAG_NOCRLF)))
		{
			int dwKeySize = *pdwSize;
			hr = GenerateRandomName(key, dwDataSize);
			if (SUCCEEDED(hr))
			{
				if( Base64Encode((const unsigned char*)key, dwDataSize, szNewID,
					(int*)&dwKeySize, ATL_BASE64_FLAG_NOCRLF) )
				{
					//null terminate
					szNewID[dwKeySize]=0;
					*pdwSize = dwKeySize+1;
				}
				else
					hr = E_FAIL;
			}
			else
			{
				*pdwSize = (DWORD)(Base64EncodeGetRequiredLength(dwDataSize,
					ATL_BASE64_FLAG_NOCRLF));
				return E_OUTOFMEMORY;
			}
		}
		return hr;
	}

	int CalcMaxInputSize(int nOutputSize) throw()
	{
		if (nOutputSize < MIN_SESSION_NAME_SIZE)
			return 0;
		// subtract one from the output size to make room
		// for the NULL terminator in the output then
		// calculate the biggest number of input bytes that
		// when base64 encoded will fit in a buffer of size
		// nOutputSize (including base64 padding)
		int nInputSize = ((nOutputSize-1)*3)/4;
		int factor = ((nInputSize*4)/3)%4;
		if (factor)
			nInputSize -= factor;
		return nInputSize;
	}


	HRESULT GenerateRandomName(BYTE *pBuff, DWORD dwBuffSize) throw()
	{
		HRESULT hr = S_OK;

		if (!pBuff)
			return E_POINTER;

		if (!dwBuffSize)
			return E_UNEXPECTED;

		if (!m_bCryptNotAvailable && GetHandle())
		{
			// Use the crypto api to generate random data.
			hr = GenRandom(dwBuffSize, pBuff);
			return hr;
		}

		// CryptoApi isn't available so we generate
		// random data using rand. We seed the random
		// number generator with a seed that is a combination
		// of bytes from an arbitrary number and the system
		// time which changes every millisecond so it will
		// be different for every call to this function.
		FILETIME ft;
		GetSystemTimeAsFileTime(&ft);
		static DWORD dwVal = 0x21;
		DWORD dwSeed = (dwVal++ << 0x18) | (ft.dwLowDateTime & 0x00ffff00) | dwVal++ & 0x000000ff;
		srand(dwSeed);
		BYTE *pCurr = pBuff;
		// fill buffer with random bytes
		for (int i=0; i < (int)dwBuffSize; i++)
		{
			*pCurr = (BYTE) (rand() & 0x000000ff);
			pCurr++;
		}
		return S_OK;
	}
};

// Contains the data for the session variable accessors
class CSessionDataBase
{
public:
	TCHAR m_szSessionID[MAX_SESSION_KEY_LEN];
	TCHAR m_VariableName[MAX_VARIABLE_NAME_LENGTH];
	TCHAR m_VariableValue[MAX_VARIABLE_VALUE_LENGTH];
	long m_VarType;
	CSessionDataBase()
	{
		m_VarType = 0;
		m_szSessionID[0] = 0;
		m_VariableName[0] = 0;
		m_VariableValue[0] = 0;
	}
	HRESULT Assign(LPCTSTR szSessionID, LPCTSTR szVarName, LPCTSTR szVal,
		long nVarType)
	{
		HRESULT hr = S_OK;
		if ( szSessionID )
			if ( _tcslen(szSessionID)< MAX_SESSION_KEY_LEN)
				_tcscpy(m_szSessionID, szSessionID);
			else
				hr = E_OUTOFMEMORY;

		if (szVarName)
			if ( _tcslen(szVarName) < MAX_VARIABLE_NAME_LENGTH)
				_tcscpy(m_VariableName, szVarName);
			else
				hr = E_OUTOFMEMORY;
		
		if (szVal)
			if ( _tcslen(szVal) < MAX_VARIABLE_VALUE_LENGTH)
				_tcscpy(m_VariableValue, szVal);
			else
				hr = E_OUTOFMEMORY;

		if (hr == S_OK)
			m_VarType = nVarType;

		return hr;
	}
};

// Use to select a session variable given the name
// of a session and the name of a variable.
class CSessionDataSelector : public CSessionDataBase
{
public:
	BEGIN_COLUMN_MAP(CSessionDataSelector) 
		COLUMN_ENTRY(1, m_szSessionID)
		COLUMN_ENTRY(2, m_VariableName)
		COLUMN_ENTRY(3, m_VariableValue)
		COLUMN_ENTRY(4, m_VarType)
	END_COLUMN_MAP()
	BEGIN_PARAM_MAP(CSessionDataSelector) 
		SET_PARAM_TYPE(DBPARAMIO_INPUT)
		COLUMN_ENTRY(1, m_szSessionID)
		COLUMN_ENTRY(2, m_VariableName)
	END_PARAM_MAP()
};

// Use to select all session variables given the name of
// of a session.
class CAllSessionDataSelector : public CSessionDataBase
{
public:
	BEGIN_COLUMN_MAP(CAllSessionDataSelector) 
		COLUMN_ENTRY(1, m_szSessionID)
		COLUMN_ENTRY(2, m_VariableName)
		COLUMN_ENTRY(3, m_VariableValue)
		COLUMN_ENTRY(4, m_VarType)
	END_COLUMN_MAP()
	BEGIN_PARAM_MAP(CAllSessionDataSelector) 
		SET_PARAM_TYPE(DBPARAMIO_INPUT)
		COLUMN_ENTRY(1, m_szSessionID)
	END_PARAM_MAP()
};

// Use to update the value of a session variable
class CSessionDataUpdator : public CSessionDataBase
{
public:
	BEGIN_PARAM_MAP(CSessionDataUpdator) 
		SET_PARAM_TYPE(DBPARAMIO_INPUT)
		COLUMN_ENTRY(1, m_VariableValue)
		COLUMN_ENTRY(2, m_VarType)
		COLUMN_ENTRY(3, m_szSessionID)
		COLUMN_ENTRY(4, m_VariableName)
	END_PARAM_MAP()
};

// Use to delete a session variable given the
// session name and the name of the variable
class CSessionDataDeletor
{
public:
	TCHAR m_szSessionID[MAX_SESSION_KEY_LEN];
	TCHAR m_VariableName[MAX_VARIABLE_NAME_LENGTH];
	HRESULT Assign(LPCTSTR szSessionID, LPCTSTR szVarName)
	{
		if (szSessionID)
		{
			if (_tcslen(szSessionID) < MAX_SESSION_KEY_LEN)
				_tcscpy(m_szSessionID, szSessionID);
			else
				return E_OUTOFMEMORY;
		}
		else
			m_szSessionID[0]=0;

		if (szVarName)
		{
			if(_tcslen(szVarName) < MAX_VARIABLE_NAME_LENGTH)
				_tcscpy(m_VariableName, szVarName);
			else
				return E_OUTOFMEMORY;
		}
		else
			m_VariableName[0]=0;
		return S_OK;
	}

	BEGIN_PARAM_MAP(CSessionDataDeletor) 
		SET_PARAM_TYPE(DBPARAMIO_INPUT)
		COLUMN_ENTRY(1, m_szSessionID)
		COLUMN_ENTRY(2, m_VariableName)
	END_PARAM_MAP()
};

class CSessionDataDeleteAll
{
public:
	TCHAR m_szSessionID[MAX_SESSION_KEY_LEN];
	HRESULT Assign(LPCTSTR szSessionID)
	{
		if (!szSessionID)
			return E_INVALIDARG;

		if (_tcslen(szSessionID) < MAX_SESSION_KEY_LEN)
			_tcscpy(m_szSessionID, szSessionID);
		else
			return E_OUTOFMEMORY;

		return S_OK;
	}

	BEGIN_PARAM_MAP(CSessionDataDeleteAll) 
		SET_PARAM_TYPE(DBPARAMIO_INPUT)
		COLUMN_ENTRY(1, m_szSessionID)
	END_PARAM_MAP()
};

// Used for retrieving the count of session variables for
// a given session ID.
class CCountAccessor
{
public:
	LONG m_nCount;
	TCHAR m_szSessionID[MAX_SESSION_KEY_LEN];
	CCountAccessor()
	{
		m_nCount = 0;
	}

	HRESULT Assign(LPCTSTR szSessionID)
	{
		if (!szSessionID)
			return E_INVALIDARG;

		if (_tcslen(szSessionID) < MAX_SESSION_KEY_LEN)
			_tcscpy(m_szSessionID, szSessionID);
		else
			return E_OUTOFMEMORY;

		return S_OK;
	}

	BEGIN_COLUMN_MAP(CCountAccessor)
		COLUMN_ENTRY(1, m_nCount)
	END_COLUMN_MAP()
	BEGIN_PARAM_MAP(CCountAccessor)
		SET_PARAM_TYPE(DBPARAMIO_INPUT)
		COLUMN_ENTRY(1, m_szSessionID)
	END_PARAM_MAP()
	DEFINE_COMMAND(CCountAccessor, ATL_SESSVAR_GETCOUNT )
};


// Used for updating entries in the session
// references table, given a session ID
class CSessionRefUpdator
{
public:
	TCHAR m_SessionID[MAX_SESSION_KEY_LEN];
	HRESULT Assign(LPCTSTR szSessionID)
	{
		if (!szSessionID)
			return E_INVALIDARG;
		if (_tcslen(szSessionID) < MAX_SESSION_KEY_LEN)
			_tcscpy(m_SessionID, szSessionID);
		else
			return E_OUTOFMEMORY;
		return S_OK;
	}
	BEGIN_PARAM_MAP(CSessionRefUpdator)
		SET_PARAM_TYPE(DBPARAMIO_INPUT)
		COLUMN_ENTRY(1, m_SessionID)
	END_PARAM_MAP()
};

class CSessionRefIsExpired
{
public:
	TCHAR m_SessionID[MAX_SESSION_KEY_LEN];
	TCHAR m_SessionIDOut[MAX_SESSION_KEY_LEN];
	HRESULT Assign(LPCTSTR szSessionID)
	{
		m_SessionIDOut[0]=0;
		if (!szSessionID)
			return E_INVALIDARG;
		if (_tcslen(szSessionID) < MAX_SESSION_KEY_LEN)
			_tcscpy(m_SessionID, szSessionID);
		else
			return E_OUTOFMEMORY;
		return S_OK;
	}
	BEGIN_COLUMN_MAP(CSessionRefIsExpired)
		COLUMN_ENTRY(1, m_SessionIDOut)
	END_COLUMN_MAP()
	BEGIN_PARAM_MAP(CSessionRefIsExpired)
		SET_PARAM_TYPE(DBPARAMIO_INPUT)
		COLUMN_ENTRY(1, m_SessionID)
	END_PARAM_MAP()
};


class CSessionRefUpdateTimeout
{
public:
	TCHAR m_SessionID[MAX_SESSION_KEY_LEN];
	unsigned __int64 m_nNewTimeout;
	HRESULT Assign(LPCTSTR szSessionID, unsigned __int64 nNewTimeout)
	{
		if (!szSessionID)
			return E_INVALIDARG;

		if (_tcslen(szSessionID) < MAX_SESSION_KEY_LEN)
			_tcscpy(m_SessionID, szSessionID);
		else
			return E_OUTOFMEMORY;

		m_nNewTimeout = nNewTimeout;

		return S_OK;
	}

	BEGIN_PARAM_MAP(CSessionRefUpdateTimeout)
		SET_PARAM_TYPE(DBPARAMIO_INPUT)
		COLUMN_ENTRY(1, m_nNewTimeout)
		COLUMN_ENTRY(2, m_SessionID)
	END_PARAM_MAP()
};

class CSessionRefSelector
{
public:
	TCHAR m_SessionID[MAX_SESSION_KEY_LEN];
	int m_RefCount;
	HRESULT Assign(LPCTSTR szSessionID)
	{
		if (!szSessionID)
			return E_INVALIDARG;
		if (_tcslen(szSessionID) < MAX_SESSION_KEY_LEN)
			_tcscpy(m_SessionID, szSessionID);
		else
			return E_OUTOFMEMORY;
		return S_OK;
	}
	BEGIN_COLUMN_MAP(CSessionRefSelector)
		COLUMN_ENTRY(1, m_SessionID)
		COLUMN_ENTRY(3, m_RefCount)
	END_COLUMN_MAP()
	BEGIN_PARAM_MAP(CSessionRefSelector)
		SET_PARAM_TYPE(DBPARAMIO_INPUT)
		COLUMN_ENTRY(1, m_SessionID)
	END_PARAM_MAP()
};

class CSessionRefCount
{
public:
	LONG m_nCount;
	BEGIN_COLUMN_MAP(CSessionRefCount)
		COLUMN_ENTRY(1, m_nCount)
	END_COLUMN_MAP()
};

// Used for creating new entries in the session
// references table.
class CSessionRefCreator
{
public:
	TCHAR m_SessionID[MAX_SESSION_KEY_LEN];
	unsigned __int64 m_TimeoutMs;
	HRESULT Assign(LPCTSTR szSessionID, unsigned __int64 timeout)
	{
		if (!szSessionID)
			return E_INVALIDARG;
		if (_tcslen(szSessionID) < MAX_SESSION_KEY_LEN)
		{
			_tcscpy(m_SessionID, szSessionID);
			m_TimeoutMs = timeout;
		}
		else
			return E_OUTOFMEMORY;
		return S_OK;
	}
	BEGIN_PARAM_MAP(CSessionRefCreator)
		SET_PARAM_TYPE(DBPARAMIO_INPUT)
		COLUMN_ENTRY(1, m_SessionID)
		COLUMN_ENTRY(2, m_TimeoutMs)
	END_PARAM_MAP()
};


// CPersistSessionDB
// This session persistance class persists session variables to
// an OLEDB datasource. The OLEDB connection string is passed as the
// pInitParam parameter of the Initialize function of this class. The
// schema for the datasource should be something compatible with the following:

// TableName: SessionVariables
// Column		Name			Type							Description
// 1			SessionID		char[MAX_SESSION_KEY_LEN]		Session Key name
// 2			VariableName	char[MAX_VARIABLE_NAME_LENGTH]	Variable Name
// 3			VariableValue	char[MAX_VARIABLE_VALUE_LENGTH]	Variable Value
// 4			VarType			int								Variable Type (VARTYPE)
//
// TableName: SessionReferences
// Column		Name			Type							Description
// 1			SessionID		char[MAX_SESSION_KEY_LEN]		Session Key Name.
// 2			LastAccess		datetime						Date and time of last access to this session.
// 3			RefCount		int								Current references on this session.
// 4			TimeoutMS		int								Timeout value for the session in milli seconds

// 
// CDBSession
// Implements ISession for database persisted sessions.
//
class CDBSession:
	public ISession,
	public CComObjectRootEx<CComGlobalsThreadModel>

{
	typedef CCommand<CAccessor<CAllSessionDataSelector> >  iterator_accessor;
public:
	BEGIN_COM_MAP(CDBSession)
		COM_INTERFACE_ENTRY(ISession)
	END_COM_MAP()

	CDBSession() throw():
		m_dwTimeout(ATL_SESSION_TIMEOUT)
	{
		m_szSessionName[0] = 0;
	}

	~CDBSession() throw()
	{
	}

	void FinalRelease()throw()
	{
		SessionUnlock();
	}

	STDMETHOD(SetVariable)(LPCSTR szName, VARIANT Val) throw()
	{
		HRESULT hr = E_FAIL;

		// Get the data connection for this thread.
		CDataConnection dataconn;
		hr = GetSessionConnection(&dataconn, m_szConnectionString, m_spServiceProvider);
		if (hr != S_OK)
			return hr;

		// Update the last access time for this session
		Access();

		// Change the type of the variant to a string for storage in the database
		CComVariant vIn(Val);
		hr = vIn.ChangeType(VT_BSTR);
		if (SUCCEEDED(hr))
		{
			// Allocate an updator command and fill out it's input parameters.
			CCommand<CAccessor<CSessionDataUpdator> > command;
			CA2T name(szName);
			CA2T session(m_szSessionName);
			CW2T value(vIn.bstrVal);
			hr = command.Assign(session, name, value, Val.vt);
			if (hr != S_OK)
				return hr;

			// Try an update. Update will fail if the variable is not already there.
			long nRows = 0;
			if (hr == S_OK)
				hr = command.Open(dataconn, ATL_SESSVAR_UPDATE, NULL, &nRows, DBGUID_DEFAULT, false);
			if (hr == S_OK && nRows <= 0)
				hr = E_UNEXPECTED;
			if (hr != S_OK)
			{
				// Try an insert
				hr = command.Open(dataconn, ATL_SESSVAR_INSERT, NULL, &nRows, DBGUID_DEFAULT, false);
				if (hr == S_OK && nRows <=0)
					hr = E_UNEXPECTED;
			}
		}
		return hr;
	}

	// Warning: For string data types, depending on the configuration of
	// your database, strings might be returned with trailing white space.
	STDMETHOD(GetVariable)(LPCSTR szName, VARIANT *pVal) throw()
	{
		HRESULT hr = E_FAIL;

		// Get the data connection for this thread
		CDataConnection dataconn;
		hr = GetSessionConnection(&dataconn, m_szConnectionString, m_spServiceProvider);
		if (hr != S_OK)
			return hr;

		// Update the last access time for this session
		Access();

		// Allocate a command a fill out it's input parameters.
		CCommand<CAccessor<CSessionDataSelector> > command;
		CA2T name(szName);
		CA2T session(m_szSessionName);
		hr = command.Assign(session, name, NULL, NULL);
		if (hr == S_OK)
		{
			hr = command.Open(dataconn, ATL_SESSVAR_SELECTVAR);
			if (SUCCEEDED(hr))
			{
				if ( S_OK == (hr = command.MoveFirst()))
				{
					hr = copy_entry(command.m_VariableValue, command.m_VarType, pVal);
				}
			}
		}
		return hr;
	}

	STDMETHOD(RemoveVariable)(LPCSTR szName) throw()
	{
		HRESULT hr = E_FAIL;

		// Get the data connection for this thread.
		CDataConnection dataconn;
		hr = GetSessionConnection(&dataconn, m_szConnectionString, m_spServiceProvider);
		if (hr != S_OK)
			return hr;

		// update the last access time for this session
		Access();

		// allocate a command and set it's input parameters
		CCommand<CAccessor<CSessionDataDeletor> > command;
		CA2T name(szName);
		CA2T session(m_szSessionName);
		hr = command.Assign(session, name);

		// execute the command
		long nRows = 0;
		if (hr == S_OK)
			hr = command.Open(dataconn, ATL_SESSVAR_DELETEVAR, NULL, &nRows, DBGUID_DEFAULT, false);
		if (hr == S_OK && nRows <= 0)
			hr = E_UNEXPECTED;
		return hr;
	}

	// Gives the count of rows in the table for this session ID.
	STDMETHOD(GetCount)(long *pnCount) throw()
	{
		HRESULT hr = S_OK;
		if (!pnCount)
			return E_POINTER;

		// Get the database connection for this thread.
		CDataConnection dataconn;
		hr = GetSessionConnection(&dataconn, m_szConnectionString, m_spServiceProvider);
		if (hr != S_OK)
			return hr;
		Access();
		LPCTSTR szSQL;
		CCommand<CAccessor<CCountAccessor> > command;
		CA2T session(m_szSessionName);
		
		command.GetDefaultCommand(&szSQL);
		hr = command.Assign(session);
		if (hr == S_OK)
		{
			hr = command.Open(dataconn, szSQL);
			if (hr == S_OK)
			{
				if (S_OK == (hr = command.MoveFirst()))
				{
					*pnCount = command.m_nCount;
					hr = S_OK;
				}
			}
		}
		return hr;
	}

	STDMETHOD(RemoveAllVariables)() throw()
	{
		return RemoveSessionData();
	}

	// Iteration of variables works by taking a snapshot
	// of the sessions at the point in time BeginVariableEnum
	// is called, and then keeping an index variable that you use to
	// move through the snapshot rowset. It is important to know
	// that the handle returned in phEnum is not thread safe. It
	// should only be used by the calling thread.
	STDMETHOD(BeginVariableEnum)(HANDLE *phEnum, POSITION *pPOS) throw()
	{
		HRESULT hr = E_FAIL;
		if (!pPOS)
			return E_UNEXPECTED;

		// Get the data connection for this thread.
		CDataConnection dataconn;
		hr = GetSessionConnection(&dataconn, m_szConnectionString, m_spServiceProvider);
		if (hr != S_OK)
			return hr;

		// Update the last access time for this session.
		Access();

		// Allocate a new iterator accessor and initialize it's input parameters.
		iterator_accessor *pIteratorAccessor = new iterator_accessor;
		if (!pIteratorAccessor)
			return E_OUTOFMEMORY;

		CA2T session(m_szSessionName);
		hr = pIteratorAccessor->Assign(session, NULL, NULL, NULL);
		if (hr == S_OK)
		{
			// execute the command and move to the first row of the recordset.
			hr = pIteratorAccessor->Open(dataconn, ATL_SESSVAR_SELECTALLVARS);
			if (hr == S_OK)
			{
				hr = pIteratorAccessor->MoveFirst();
				if (hr == S_OK)
				{
					*pPOS = (POSITION) INVALID_DB_SESSION_POS + 1;
					*phEnum = (HANDLE)pIteratorAccessor;
				}
			}

			if (hr != S_OK)
			{
				*pPOS = INVALID_DB_SESSION_POS;
				*phEnum = NULL;
				delete pIteratorAccessor;
			}
		}
		return hr;
	}

	// The values for hEnum and pPos must have been initialized in a previous
	// call to BeginVariableEnum. On success, the out variant will hold the next
	// variable
	STDMETHOD(GetNextVariable)(HANDLE hEnum, POSITION *pPOS, LPSTR szName, DWORD dwLen, VARIANT *pVal) throw()
	{
		if (!pPOS || !pVal)
			return E_INVALIDARG;

		iterator_accessor *pIteratorAccessor = (iterator_accessor*)hEnum;
		if (!pIteratorAccessor || *pPOS <= INVALID_DB_SESSION_POS)
			return E_UNEXPECTED;

		// update the last access time.
		Access();

		HRESULT hr = S_OK;
		POSITION posCurrent = *pPOS;
		
		if (hr == S_OK)
		{
			if (szName)
			{
				// caller wants entry name
				size_t nNameLenChars = _tcslen(pIteratorAccessor->m_VariableName);
				if (dwLen > nNameLenChars)
					_tcscpy((LPTSTR)szName, pIteratorAccessor->m_VariableName);
				else
					hr = E_OUTOFMEMORY; // buffer not big enough
			}
			if (hr == S_OK)
			{
				hr = copy_entry(pIteratorAccessor->m_VariableValue,
					pIteratorAccessor->m_VarType, pVal);
			}

			if (hr == S_OK)
			{
				hr = pIteratorAccessor->MoveNext();
				*pPOS = ++posCurrent;
			}

		}

		if (hr == DB_S_ENDOFROWSET)
		{
			// We're done iterating, reset everything
			*pPOS = INVALID_DB_SESSION_POS;
			hr = S_OK;
		}

		if (hr != S_OK)
		{
			pVal->vt = VT_EMPTY;
			pVal->lVal = 0;
		}
		return hr;
	}

	// CloseEnum frees up any resources allocated by the iterator
	STDMETHOD(CloseEnum)(HANDLE hEnum)
	{
		iterator_accessor *pIteratorAccessor = (iterator_accessor*)hEnum;
		if (!pIteratorAccessor)
			return E_UNEXPECTED;
		pIteratorAccessor->Close();
		delete pIteratorAccessor;
		return S_OK;
	}

	// RemoveSessionData will remove all session variables for this session.
	STDMETHOD(RemoveSessionData)() throw()
	{
		HRESULT hr = E_UNEXPECTED;

		// Get the data connection for this thread.
		CDataConnection dataconn;
		hr = GetSessionConnection(&dataconn, m_szConnectionString, m_spServiceProvider);
		if (hr != S_OK)
			return hr;

		CCommand<CAccessor<CSessionDataDeleteAll> > command;
		CA2T session(m_szSessionName);
		hr = command.Assign(session);
		if (hr != S_OK)
			return hr;

		// delete all session variables
		hr = command.Open(dataconn, ATL_SESSVAR_DELETEALLVARS, NULL, NULL, DBGUID_DEFAULT, false);
		return hr;
	}


	//
	// Returns S_FALSE if it's not expired
	// S_OK if it is expired and an error HRESULT
	// if an error occurred.
	STDMETHOD(IsExpired)() throw()
	{
		HRESULT hrRet = S_FALSE;
		HRESULT hr = E_UNEXPECTED;

		// Get the data connection for this thread.
		CDataConnection dataconn;
		hr = GetSessionConnection(&dataconn, m_szConnectionString, m_spServiceProvider);
		if (hr != S_OK)
			return hr;

		CCommand<CAccessor<CSessionRefIsExpired> > command;
		CA2T session(m_szSessionName);
		hr = command.Assign(session);
		if (hr != S_OK)
			return hr;

		hr = command.Open(dataconn, ATL_SESSIONREF_ISEXPIRED, 
							NULL, NULL, DBGUID_DEFAULT, true);
		if (hr == S_OK)
		{
			if (S_OK == command.MoveFirst())
			{
				CA2T session(m_szSessionName);
				if (!_tcscmp(command.m_SessionIDOut, session))
					hrRet = S_OK;
			}
		}

		if (hr == S_OK)
			return hrRet;
		return hr;
	}

	STDMETHOD(SetTimeout)(unsigned __int64 dwNewTimeout)
	{
		HRESULT hr = E_UNEXPECTED;

		// Get the data connection for this thread.
		CDataConnection dataconn;
		hr = GetSessionConnection(&dataconn, m_szConnectionString, m_spServiceProvider);
		if (hr != S_OK)
			return hr;

		// allocate a command and set it's input parameters
		CCommand<CAccessor<CSessionRefUpdateTimeout> > command;
		CA2T session(m_szSessionName);
		hr = command.Assign(session, dwNewTimeout);
		if (hr != S_OK)
			return hr;

		hr = command.Open(dataconn, ATL_SESSIONREF_UPDATETIMEOUT,
						NULL, NULL, DBGUID_DEFAULT, false);

		return hr;
	}

	// SessionLock increments the session reference count for this session.
	// If there is not a session by this name in the session references table,
	// a new session entry is created in the the table.
	HRESULT SessionLock() throw()
	{
		HRESULT hr = E_UNEXPECTED;
		if (!m_szSessionName || m_szSessionName[0]==0)
			return hr; // no session to lock.

		// retrieve the data connection for this thread
		CDataConnection dataconn;
		hr = GetSessionConnection(&dataconn, m_szConnectionString, m_spServiceProvider);
		if (hr != S_OK)
			return hr;

		// first try to update a session with this name
		long nRows = 0;
		CCommand<CAccessor<CSessionRefUpdator> > updator;
		CA2T session(m_szSessionName);
		if (S_OK == updator.Assign(session))
		{
			if (S_OK != (hr = updator.Open(dataconn, ATL_SESSIONREF_ADDREF, NULL, &nRows, DBGUID_DEFAULT, false)) ||
				nRows == 0)
			{
				// No session to update. Use the creator accessor
				// to create a new session reference.
				CCommand<CAccessor<CSessionRefCreator> > creator;
				hr = creator.Assign(session, m_dwTimeout);
				if (hr == S_OK)
					hr = creator.Open(dataconn, ATL_SESSIONREF_CREATE, NULL, &nRows, DBGUID_DEFAULT, false);
			}
		}

		// We should have been able to create or update a session.
		ATLASSERT(nRows > 0);
		if (hr == S_OK && nRows <= 0)
			hr = E_UNEXPECTED;

		return hr;
	}

	// SessionUnlock decrements the session RefCount for this session.
	// Sessions cannot be removed from the database unless the session
	// refcount is 0
	HRESULT SessionUnlock() throw()
	{
		HRESULT hr = E_UNEXPECTED;
		if (!m_szSessionName ||
			m_szSessionName[0]==0)
			return hr; 

		// get the data connection for this thread
		CDataConnection dataconn;
		hr = GetSessionConnection(&dataconn, m_szConnectionString, m_spServiceProvider);
		if (hr != S_OK)
			return hr;

		// The session must exist at this point in order to unlock it
		// so we can just use the session updator here.
		long nRows = 0;
		CCommand<CAccessor<CSessionRefUpdator> > updator;
		CA2T session(m_szSessionName);
		hr = updator.Assign(session);
		if (hr == S_OK)
		{
			hr = updator.Open(	dataconn,
								ATL_SESSIONREF_REMOVEREF,
								NULL,
								&nRows,
								DBGUID_DEFAULT,
								false);
		}
		if (hr != S_OK)
			return hr;

        // delete the session from the database if 
		// nobody else is using it and it's expired.
		hr = FreeSession();
		return hr;
	}

	// Access updates the last access time for the session. The access
	// time for sessions is updated using the SQL GETDATE function on the
	// database server so that all clients will be using the same clock
	// to compare access times against.
	HRESULT Access() throw()
	{
		HRESULT hr = E_UNEXPECTED;

		if (!m_szSessionName || 
			m_szSessionName[0]==0)
			return hr; // no session to access

		// get the data connection for this thread
		CDataConnection dataconn;
		hr = GetSessionConnection(&dataconn, m_szConnectionString, m_spServiceProvider);
		if (hr != S_OK)
			return hr;

		// The session reference entry in the references table must
		// be created prior to calling this function so we can just
		// use an updator to update the current entry.
		CCommand<CAccessor<CSessionRefUpdator> > updator;
		CA2T session(m_szSessionName);
		long nRows = 0;
		hr = updator.Assign(session);
		if (hr == S_OK)
		{
			hr = updator.Open(	dataconn,
								ATL_SESSIONREF_ACCESS,
								NULL,
								&nRows,
								DBGUID_DEFAULT,
								false);
		}

		ATLASSERT(nRows > 0);
		if (hr == S_OK && nRows <= 0)
			hr = E_UNEXPECTED;
		return hr;
	}

	// If the session is expired and it's reference is 0,
	// it can be deleted. SessionUnlock calls this function to
	// unlock the session and delete it after we release a session
	// lock. Note that our SQL command will only delete the session
	// if it is expired and it's refcount is <= 0
	HRESULT FreeSession() throw()
	{
		HRESULT hr = E_UNEXPECTED;
		if (!m_szSessionName ||
			m_szSessionName[0]==0)
			return hr;

		// Get the data connection for this thread.
		CDataConnection dataconn;
		hr = GetSessionConnection(&dataconn, m_szConnectionString, m_spServiceProvider);
		if (hr != S_OK)
			return hr;

		CCommand<CAccessor<CSessionRefUpdator> > updator;

		// The SQL for this command only deletes the
		// session reference from the references table if it's access
		// count is 0 and it has expired.
		return updator.Open(dataconn,
							ATL_SESSIONREF_DELETE,
							NULL,
							NULL,
							DBGUID_DEFAULT,
							false);
	}

	// Initialize is called each time a new session is created.
	HRESULT Initialize(LPCSTR szSessionName, IServiceProvider *pServiceProvider) throw()
	{
		if (!pServiceProvider)
			return E_POINTER;
		m_spServiceProvider = pServiceProvider;

		if (strlen(szSessionName) < MAX_SESSION_KEY_LEN)
			strcpy(m_szSessionName, szSessionName);
		else
			return E_OUTOFMEMORY;
		return SessionLock();
	}

	static HRESULT GetSessionConnection(CDataConnection *pConn,
							LPCTSTR szConnStr, IServiceProvider *pProv)
	{
		HRESULT hr = E_FAIL;
		if (!pProv)
			return E_UNEXPECTED;

		hr = GetDataSource(pProv,
					ATL_DBSESSION_ID,
					szConnStr,
					pConn);

		if (hr != S_OK)
		{
			// The calling thread isn't an extension
			// worker that has a datasource cache on it.
			// Create one.
			hr = pConn->Open(szConnStr);
			if (hr == CO_E_NOTINITIALIZED)
			{
				ATLTRACE(_T("Warning: Initializing COM on thread %x to create a data connection for the session service\n"),
					GetCurrentThreadId());
				hr = CoInitialize(NULL);
				if (hr == S_OK)
				{
					hr = pConn->Open(szConnStr);
				}
			}
		}
		return hr;
	}

	static HRESULT SetConnectionString(LPTSTR szConnStr)
	{
		if (_tcslen(szConnStr) < MAX_CONNECTION_STRING_LEN)
		{
			_tcscpy(m_szConnectionString, szConnStr);
			return S_OK;
		}
		return E_OUTOFMEMORY;
	}
protected:

	HRESULT copy_entry(LPCTSTR szValue, long nValType, VARIANT *pVarOut) throw()
	{
		HRESULT hr = E_FAIL;
		CComVariant vOut;
		CComBSTR val(szValue);
		vOut.vt = VT_BSTR;
		vOut.bstrVal = val.Copy();
		hr = vOut.ChangeType((short)nValType);
		if (SUCCEEDED(hr))
			hr = VariantCopy(pVarOut, &vOut);
		return hr;
	}

	char m_szSessionName[MAX_SESSION_KEY_LEN];
	unsigned __int64 m_dwTimeout;
	CComPtr<IServiceProvider> m_spServiceProvider;
	static TCHAR m_szConnectionString[MAX_CONNECTION_STRING_LEN];

}; // CDBSession
__declspec(selectany)TCHAR CDBSession::m_szConnectionString[MAX_CONNECTION_STRING_LEN];

class CDBSessionServiceImpl
{
	TCHAR m_szConnectionString[MAX_CONNECTION_STRING_LEN];
	CComPtr<IServiceProvider> m_spServiceProvider;
public:
	CDBSessionServiceImpl()
	{
		m_dwTimeout = ATL_SESSION_TIMEOUT;
	}

	HRESULT Initialize(void *pData, DWORD dwSize, IServiceProvider *pProvider)
	{
		HRESULT hr = S_OK;
		if (!pData || !pProvider)
			return E_INVALIDARG;

		if (dwSize < MAX_CONNECTION_STRING_LEN)
		{
			_tcscpy(m_szConnectionString, (LPTSTR)pData); // assume TCHAR
			hr = CDBSession::SetConnectionString(m_szConnectionString);
		}
		else
			return E_OUTOFMEMORY;
		m_spServiceProvider = pProvider;
		return hr;
	}

	HRESULT CreateNewSession(LPSTR szNewID, DWORD *pdwSize, ISession** ppSession)
	{
		HRESULT hr = E_FAIL;
		CComObject<CDBSession> *pNewSession = NULL;

		if (!pdwSize)
			return E_UNEXPECTED;

		if (!ppSession)
			return E_POINTER;
		*ppSession = NULL;

		// Create new session
		CComObject<CDBSession>::CreateInstance(&pNewSession);
		if (pNewSession == NULL)
			return E_OUTOFMEMORY;

		// Create a session name and initialize the object
		hr = m_SessionNameGenerator.GetNewSessionName(szNewID, pdwSize);
		if (hr == S_OK)
		{
			hr = pNewSession->Initialize(szNewID, m_spServiceProvider);
			if (hr == S_OK)
			{
				// we don't hold a reference to the object
				hr = pNewSession->QueryInterface(__uuidof(ISession), (void**)ppSession);
			}
		}

		if (hr != S_OK)
			delete pNewSession;
		return hr;
	
	}

	HRESULT GetSession(LPCSTR szID, ISession **ppSession)
	{
		HRESULT hr = E_FAIL;
		if (!ppSession)
			return E_POINTER;

		CComObject<CDBSession> *pNewSession = NULL;

		// Check the DB to see if the session ID is a valid session
		CA2T session(szID);
		if (S_OK == IsValidSession(session))
		{
			// Create new session object to represent this session
			CComObject<CDBSession>::CreateInstance(&pNewSession);
			if (pNewSession == NULL)
				return E_OUTOFMEMORY;

			hr = pNewSession->Initialize(szID, m_spServiceProvider);
			if (hr == S_OK)
			{
				// we don't hold a reference to the object
				hr = pNewSession->QueryInterface(__uuidof(ISession), (void**)ppSession);
			}
		}

		if (hr != S_OK && pNewSession)
			delete pNewSession;
		return hr;
	}

	HRESULT CloseSession(LPCSTR szID)
	{
		CDataConnection conn;
		HRESULT hr = CDBSession::GetSessionConnection(&conn,
											m_szConnectionString,
											m_spServiceProvider);
		if (hr != S_OK)
			return hr;

		// set up accessors
		CCommand<CAccessor<CSessionRefUpdator> > updator;
		CA2T session(szID);
		hr = updator.Assign(session);
		if (hr != S_OK)
			return hr;

		CCommand<CAccessor<CSessionDataDeleteAll> > command;
		hr = command.Assign(session);
		if (hr != S_OK)
			return hr;

		// delete all session variables
		hr = command.Open(conn, ATL_SESSVAR_DELETEALLVARS, NULL, NULL, DBGUID_DEFAULT, false);
		if (hr != S_OK)
			return hr;

		// delete references in the session references table
		hr = updator.Open(conn,
							ATL_SESSIONREF_DELETEFINAL,
							NULL,
							NULL,
							DBGUID_DEFAULT,
							false);
		return hr;
	}

	HRESULT SetSessionTimeout(unsigned __int64 dwNewTimeout) throw()
	{
		// Get the data connection for this thread
		CDataConnection conn;
		HRESULT hr = CDBSession::GetSessionConnection(&conn,
											m_szConnectionString,
											m_spServiceProvider);
		if (hr != S_OK)
			return hr;

		// all sessions get the same timeout
		CCommand<CNoAccessor> command;
		TCHAR szSQL[DEFAULT_SQL_LEN];
		_stprintf(szSQL, _T( "UPDATE SessionReferences SET TimeoutMs=%I64d" ),
						 dwNewTimeout);
		hr = command.Open(conn, szSQL, NULL, NULL, DBGUID_DEFAULT, false);
		if (hr == S_OK)
			m_dwTimeout = dwNewTimeout;

		return hr;
	}


	HRESULT GetSessionTimeout(unsigned __int64* pnTimeout)
	{
		if (!pnTimeout)
			return E_INVALIDARG;

		*pnTimeout = m_dwTimeout;
		return S_OK;
	}

	HRESULT GetSessionCount(DWORD *pnCount)
	{
		if (!pnCount)
			return E_INVALIDARG;

		CCommand<CAccessor<CSessionRefCount> > command;
		CDataConnection conn;
		HRESULT hr = CDBSession::GetSessionConnection(&conn,
											m_szConnectionString,
											m_spServiceProvider);
		if (hr != S_OK)
			return hr;

		hr = command.Open(conn, ATL_SESSIONREF_GETCOUNT, NULL, NULL, DBGUID_DEFAULT, true);
		if (hr == S_OK)
		{
			hr = command.MoveFirst();
			if (hr == S_OK)
			{
				*pnCount = (DWORD)command.m_nCount;
			}
		}

		return hr;
	}

	void ReleaseAllSessions() throw()
	{
		// nothing to do
	}

	void SweepSessions()
	{
		// nothing to do
	}


	// Helpers
	HRESULT IsValidSession(LPCTSTR szID)
	{
		// Look in the sessionreferences table to see if there is an entry
		// for this session.
		if (m_szConnectionString[0] == 0)
			return E_UNEXPECTED;

		CDataConnection conn;
		HRESULT hr = CDBSession::GetSessionConnection(&conn,
											m_szConnectionString,
											m_spServiceProvider);
		if (hr != S_OK)
			return hr;
		
		// Check the session references table to see if
		// this is a valid session
		CCommand<CAccessor<CSessionRefSelector> > selector;
		hr = selector.Assign(szID);
		if (hr != S_OK)
			return hr;

		// The SQL for this command only deletes the
		// session reference from the references table if it's access
		// count is 0 and it has expired.
		hr = selector.Open(conn,
							ATL_SESSIONREF_SELECT,
							NULL,
							NULL,
							DBGUID_DEFAULT,
							true);
		return selector.MoveFirst();
	}

	CSessionNameGenerator m_SessionNameGenerator; // Object for generating session names
	unsigned __int64 m_dwTimeout;
}; // CDBSessionServiceImpl





//////////////////////////////////////////////////////////////////
//
// In-memory persisted session
//
//////////////////////////////////////////////////////////////////

// In-memory persisted session service keeps a pointer
// to the session obejct around in memory. The pointer is
// contained in a CComPtr, which is stored in a CAtlMap, so
// we have to have a CElementTraits class for that.
typedef CComPtr<ISession> SESSIONPTRTYPE;

template<>
class CElementTraitsBase<SESSIONPTRTYPE>
{
public:
	typedef CComPtr<ISession>* PINARGTYPE;
	typedef const CComPtr<ISession>* CPINARGTYPE;
	typedef const CComPtr<ISession>& INARGTYPE;
	typedef CComPtr<ISession>& OUTARGTYPE;
	typedef DWORD SORTKEY;
	typedef CUINTHash HASHTYPE;

	static SORTKEY GetSortKey( INARGTYPE t )
	{
		(void)t;
		return( 0 );
	}

	static ULONG Hash( INARGTYPE obj )
	{
		return( (ULONG)(ULONG_PTR)obj.p);
	}

	static void CopyElements( PINARGTYPE pDest, CPINARGTYPE pSrc, size_t nElements )
	{
		for( size_t iElement = 0; iElement < nElements; iElement++ )
		{
			*pDest = *pSrc;
		}
	}

	static void RelocateElements( PINARGTYPE pDest, PINARGTYPE pSrc, size_t nElements )
	{
		// A simple memmove works for nearly all types.
		// You'll have to override this for types that have pointers to their
		// own members.
		memmove( pDest, pSrc, nElements*sizeof( SESSIONPTRTYPE) );
	}

	static BOOL CompareElements( OUTARGTYPE element1, OUTARGTYPE element2 )
	{
		return element1.IsEqualObject(element2.p) ? TRUE : FALSE;
	}

	static int CompareElementsOrdered( INARGTYPE , INARGTYPE )
	{
		ATLASSERT(0); // NOT IMPLEMENTED
		return 0;
	}
};


// CMemSession
// This session persistance class persists session variables in memory.
// Note that this type of persistance should only be used on single server
// web sites.
class CMemSession :
	public ISession,
	public CComObjectRootEx<CComGlobalsThreadModel>
{
public:
	BEGIN_COM_MAP(CMemSession)
		COM_INTERFACE_ENTRY(ISession)
	END_COM_MAP()

	STDMETHOD(GetVariable)(LPCSTR szName, VARIANT *pVal) throw()
	{
		HRESULT hr = E_FAIL;
		CComVariant val;
		m_cs.Lock();
		hr = Access();
		if (hr != S_OK)
			return hr;

		if (m_Variables.Lookup(szName, val))
		{
			hr = VariantCopy(pVal, &val);
		}
		m_cs.Unlock();
		return hr;
	}

	STDMETHOD(SetVariable)(LPCSTR szName, VARIANT vNewVal) throw()
	{
		m_cs.Lock();
		HRESULT hr = Access();
		if (hr != S_OK)
			return hr;

		hr = m_Variables.SetAt(szName, vNewVal) ? S_OK : E_FAIL;
		m_cs.Unlock();
		return hr;
	}

	STDMETHOD(RemoveVariable)(LPCSTR szName) throw()
	{
		m_cs.Lock();
		HRESULT hr = Access();
		if (hr != S_OK)
			return hr;

		hr = m_Variables.RemoveKey(szName) ? S_OK : E_FAIL;
		m_cs.Unlock();
		return hr;
	}

	STDMETHOD(GetCount)(long *pnCount) throw()
	{
		HRESULT hr = E_POINTER;
		if (!pnCount)
			return hr;
		hr = Access();
		if (hr != S_OK)
			return hr;
		m_cs.Lock();
		*pnCount = (long) m_Variables.GetCount();
		m_cs.Unlock();
		return S_OK;
	}

	STDMETHOD(RemoveAllVariables)() throw()
	{
		HRESULT hr = E_FAIL;
		m_cs.Lock();
		hr = Access();
		m_Variables.RemoveAll();
		m_cs.Unlock();
		return S_OK;
	}

	STDMETHOD(BeginVariableEnum)(HANDLE *phEnumHandle,  POSITION *pPOS) throw()
	{
		if (phEnumHandle)
			*phEnumHandle = NULL;
		HRESULT hr = E_INVALIDARG;
		if (!pPOS)
			return hr;
		m_cs.Lock();
		hr = Access();
		if (hr != S_OK)
			return hr;

		POSITION pos = m_Variables.GetStartPosition();
		m_cs.Unlock();
		*pPOS = pos;
		return hr;
	}

	STDMETHOD(GetNextVariable)(HANDLE /*hEnum*/, POSITION *pPOS, LPSTR szName, DWORD dwLen, VARIANT *pVal) throw()
	{
		if (!pPOS || !pVal)
			return E_INVALIDARG;

		HRESULT hr = E_FAIL;
		CComVariant val;
		POSITION pos = *pPOS;
		m_cs.Lock();

		hr = Access();
		if (hr != S_OK)
			return hr;

		if (szName)
		{
			CStringA strName = m_Variables.GetKeyAt(pos);
			if (strName.GetLength())
			{
				if (dwLen > (DWORD)strName.GetLength())
					strcpy(szName, strName);
				else
					hr = E_OUTOFMEMORY;
			}
		}

		if (hr == S_OK)
		{
			val = m_Variables.GetNextValue(pos);
			hr = VariantCopy(pVal, &val);
			if (hr == S_OK)
				*pPOS = pos;
		}

		m_cs.Unlock();
		return hr;
	}

	STDMETHOD(CloseEnum)(HANDLE /*hEnumHandle*/) throw()
	{
		return S_OK;
	}

	STDMETHOD(IsExpired)() throw()
	{
		CTime tmNow = CTime::GetCurrentTime();
		m_cs.Lock();
		CTimeSpan span = tmNow-m_tLastAccess;
		m_cs.Unlock();
		if ((unsigned __int64)((span.GetTotalSeconds()*1000)) > m_dwTimeout)
			return S_OK;
		return S_FALSE;
	}

	HRESULT Access() throw()
	{
		m_cs.Lock();
		m_tLastAccess = CTime::GetCurrentTime();
		m_cs.Unlock();
		return S_OK;
	}

	STDMETHOD(SetTimeout)(unsigned __int64 dwNewTimeout) throw()
	{
		m_cs.Lock();
		m_dwTimeout = dwNewTimeout;
		m_cs.Unlock();
		return S_OK;
	}

	HRESULT SessionLock() throw()
	{
		Access();
		return S_OK;
	}

	HRESULT SessionUnlock() throw()
	{
		return S_OK;
	}

protected:
	typedef CAtlMap<CStringA,
					CComVariant,
					CStringElementTraits<CStringA> > VarMapType;
	unsigned __int64 m_dwTimeout;
	CTime m_tLastAccess;
	VarMapType m_Variables;
	CComAutoCriticalSection m_cs;

}; // CMemSession


//
// CMemSessionServiceImpl
// Implements the service part of in-memory persisted session services.
//
class CMemSessionServiceImpl
{
public:
	CMemSessionServiceImpl()
	{
		m_dwTimeout = ATL_SESSION_TIMEOUT;
	}

	HRESULT CreateNewSession(LPSTR szNewID, DWORD *pdwSize, ISession** ppSession)
	{
		HRESULT hr = E_FAIL;
		CComObject<CMemSession> *pNewSession = NULL;

		if (!pdwSize)
			return E_UNEXPECTED;

		if (!ppSession)
			return E_POINTER;
		*ppSession = NULL;

		// Create new session
		CComObject<CMemSession>::CreateInstance(&pNewSession);
		if (pNewSession == NULL)
			return E_OUTOFMEMORY;

		// Initialize and add to list of CSessionData
		hr = m_SessionNameGenerator.GetNewSessionName(szNewID, pdwSize);

		if (SUCCEEDED(hr))
		{
			CComPtr<ISession> spSession;
			hr = pNewSession->QueryInterface(__uuidof(ISession), (void**)&spSession);
			pNewSession->SetTimeout(m_dwTimeout);
			pNewSession->Access();
			if (SUCCEEDED(hr))
			{
				m_CritSec.Lock();
				m_Sessions.SetAt(szNewID, spSession);
				m_CritSec.Unlock();
				//spSession.QueryInterface(ppSession);
				*ppSession = spSession.Detach();
			}
		}
		return hr;
	
	}

	HRESULT GetSession(LPCSTR szID, ISession **ppSession)
	{
		HRESULT hr = E_FAIL;
		SessMapType::CPair *pPair = NULL;

		if (!ppSession)
			return E_POINTER;
		*ppSession = NULL;

		m_CritSec.Lock();
		pPair = m_Sessions.Lookup(szID); 
		if (pPair) // the session exists and is in our local map of sessions
		{
			hr = pPair->m_value.QueryInterface(ppSession);
		}
		m_CritSec.Unlock();
		return hr;	
	}

	HRESULT CloseSession(LPCSTR szID)
	{
		HRESULT hr = E_FAIL;
		m_CritSec.Lock();
		hr = m_Sessions.RemoveKey(szID) ? S_OK : E_FAIL;
		m_CritSec.Unlock();
		return hr;
	}

	void SweepSessions()
	{
		POSITION posRemove = NULL;
		const SessMapType::CPair *pPair = NULL;
		POSITION pos = NULL;

		m_CritSec.Lock();
		pos = m_Sessions.GetStartPosition();
		while (pos)
		{
			posRemove = pos;
			pPair = m_Sessions.GetNext(pos);
			if (pPair)
			{

				if (pPair->m_value.p &&
					S_OK == pPair->m_value->IsExpired())
				{
					// remove our reference on the session
					m_Sessions.RemoveAtPos(posRemove);
				}
			}
		}
		m_CritSec.Unlock();
	}

	HRESULT SetSessionTimeout(unsigned __int64 nTimeoutMS) throw()
	{
		HRESULT hr = S_OK;
		m_CritSec.Lock();
		CComPtr<ISession> spSession;
		m_dwTimeout = nTimeoutMS;
		POSITION pos = m_Sessions.GetStartPosition();
		while (pos)
		{
			SessMapType::CPair *pPair = const_cast<SessMapType::CPair*>(m_Sessions.GetNext(pos));
			if (pPair)
			{
				spSession = pPair->m_value;
				if (spSession)
				{
					// if we fail on any of the sets we will return the
					// error code immediately
					hr = spSession->SetTimeout(nTimeoutMS);
					spSession.Release();
					if (hr != S_OK)
						return hr;
				}
			}
		}
		m_CritSec.Unlock();
		return S_OK;
	}

	HRESULT GetSessionTimeout(unsigned __int64* pnTimeout)
	{
		if (!pnTimeout)
			return E_POINTER;

		*pnTimeout = m_dwTimeout;
		return S_OK;
	}

	HRESULT GetSessionCount(DWORD *pnCount)
	{
		if (!pnCount)
			return E_INVALIDARG;

		m_CritSec.Lock();
		*pnCount = (DWORD)m_Sessions.GetCount();
		m_CritSec.Unlock();
		return S_OK;
	}

	void ReleaseAllSessions() throw()
	{
		m_Sessions.RemoveAll();
	}

	HRESULT Initialize(void*, DWORD, IServiceProvider*)
	{
		m_CritSec.Init();
		return S_OK;
	}

	typedef CAtlMap<CStringA,
					SESSIONPTRTYPE,
					CStringElementTraits<CStringA>,
					CElementTraitsBase<SESSIONPTRTYPE> > SessMapType;

	SessMapType m_Sessions; // map for holding sessions in memory
	CComCriticalSection m_CritSec; // for synchronizing access to map
	CSessionNameGenerator m_SessionNameGenerator; // Object for generating session names
	unsigned __int64 m_dwTimeout;
}; // CMemSessionServiceImpl



//
// CSessionStateService
// This class implements the session state service which can be
// exposed to request handlers.
//
// Template Parameters:
// CMonitorClass: Provides periodic sweeping services for the session service class.
// TServiceImplClass: The class that actually implements the methods of the
//                    ISessionStateService and ISessionStateControl interfaces.
template <class CMonitorClass, class TServiceImplClass >
class CSessionStateService : 
	public ISessionStateService,
	public ISessionStateControl,
	public IWorkerThreadClient,
	public CComObjectRootEx<CComGlobalsThreadModel>
{
protected:
	CMonitorClass m_Monitor;
	HANDLE m_hTimer;
	CComPtr<IServiceProvider> m_spServiceProvider;
	TServiceImplClass m_SessionServiceImpl;
public:
	// Construction/Initialization
	CSessionStateService() throw() :
	  m_hTimer(NULL)
	  {
		
	  }

	BEGIN_COM_MAP(CSessionStateService)
		COM_INTERFACE_ENTRY(ISessionStateService)
		COM_INTERFACE_ENTRY(ISessionStateControl)
	END_COM_MAP()

// ISessionStateServie methods
	STDMETHOD(CreateNewSession)(LPSTR szNewID, DWORD *pdwSize, ISession** ppSession) throw()
	{
		return m_SessionServiceImpl.CreateNewSession(szNewID, pdwSize, ppSession);
	}

	STDMETHOD(GetSession)(LPCSTR szID, ISession **ppSession) throw()
	{
		return m_SessionServiceImpl.GetSession(szID, ppSession);
	}

	STDMETHOD(CloseSession)(LPCSTR szSessionID) throw()
	{
		return m_SessionServiceImpl.CloseSession(szSessionID);
	}

	STDMETHOD(SetSessionTimeout)(unsigned __int64 nTimeoutMS) throw()
	{
		return m_SessionServiceImpl.SetSessionTimeout(nTimeoutMS);
	}

	STDMETHOD(GetSessionTimeout)(unsigned __int64 *pnTimeoutMS) throw()
	{
		return m_SessionServiceImpl.GetSessionTimeout(pnTimeoutMS);
	}

	STDMETHOD(GetSessionCount)(DWORD *pnSessionCount) throw()
	{
		return m_SessionServiceImpl.GetSessionCount(pnSessionCount);
	}

	void SweepSessions() throw()
	{
		m_SessionServiceImpl.SweepSessions();
	}

	void ReleaseAllSessions() throw()
	{
		m_SessionServiceImpl.ReleaseAllSessions();
	}

	template <class ThreadTraits>
	HRESULT Initialize(
		CWorkerThread<ThreadTraits> *pWorker,
		IServiceProvider *pServiceProvider = NULL,
		unsigned __int64 dwTimeout = ATL_SESSION_TIMEOUT,
		void *pInitData = NULL,
		DWORD dwInitDataSize = NULL) throw()
	{
	
		HRESULT hr = S_OK;
		if (pServiceProvider)
			m_spServiceProvider = pServiceProvider;

		if (pInitData)
		{
			// if dwInitDataSize is not set, we assume that
			// pInitData is a TCHAR* and take the strlen of it.
			if(dwInitDataSize == NULL)
				dwInitDataSize = (DWORD) (_tcslen((LPTSTR)pInitData)+1)*sizeof(TCHAR);
		}

		hr = m_SessionServiceImpl.Initialize(pInitData, dwInitDataSize, pServiceProvider);
		if (hr != S_OK)
			return hr;

		hr = m_SessionServiceImpl.SetSessionTimeout(dwTimeout);
		if (hr != S_OK)
			return hr;

		if (hr == S_OK)
		{
			if (pWorker)
			{
				hr = m_Monitor.InitializeWorker(pWorker);
				if (hr == S_OK)
				{
					//sweep every 500ms
					hr = m_Monitor.AddTimer(ATL_SESSION_SWEEPER_TIMEOUT, this, 0, &m_hTimer);
				}
			}
		}
		return hr;
	}

	HRESULT Execute(DWORD /*dwParam*/, HANDLE /*hObject*/) throw()
	{
		SweepSessions();
		return S_OK;
	}

	HRESULT CloseHandle(HANDLE /*hHandle*/) throw()
	{
		// The monitor thread has closed our timer handle. 
		// The sweep function won't be called again!
		m_hTimer = NULL;
		return S_OK;
	}

	void Shutdown() throw()
	{
		if (m_hTimer)
		{
			m_Monitor.RemoveHandle(m_hTimer);
			m_hTimer = NULL;
		}
		ReleaseAllSessions();
	}
}; // CSessionStateService

} // namespace ATL