// This is a part of the Active Template Library.
// Copyright (C) 1996-2000 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the	
// Active Template Library product.

#ifndef __ATLTRACE_H__
#define __ATLTRACE_H__

#pragma once

#include <atldef.h>
#include <atlconv.h>

#ifdef _DEBUG
#include <stdio.h>
#include <stdarg.h>
#endif

#ifndef _ATL_NO_DEBUG_CRT
// Warning: if you define the above symbol, you will have
// to provide your own definition of the ATLASSERT(x) macro
// in order to compile ATL
	#include <crtdbg.h>
#endif

#ifdef _DEBUG
#include <atldebugapi.h>

namespace ATL
{
class CTrace
{
public:
	typedef int (__cdecl *fnCrtDbgReport_t)(int,const char *,int,const char *,const char *,...);

	CTrace(
#ifdef _ATL_NO_DEBUG_CRT
		fnCrtDbgReport_t pfnCrtDbgReport = NULL);
#else
		fnCrtDbgReport_t pfnCrtDbgReport = _CrtDbgReport);
#endif
	~CTrace(){AtlTraceUnregister(m_hInst);}
	bool ChangeCategory(UINT nCategory, UINT nLevel, ATLTRACESTATUS eStatus)
	{
		return 0 !=
			AtlTraceModifyCategory(m_hInst, nCategory, nLevel, eStatus);
	}

	bool GetCategory(UINT nCategory, UINT *pnLevel, ATLTRACESTATUS *peStatus)
	{
		ATLASSERT(pnLevel && peStatus);
		return 0 != AtlTraceGetCategory(m_hInst, nCategory, pnLevel, peStatus);
	}

	void __cdecl TraceV(const char *pszFileName, int nLine,
		UINT nCategory, UINT nLevel, LPCSTR pszFmt, va_list args) const
	{
		AtlTraceVA(m_hInst, pszFileName, nLine, nCategory, nLevel, pszFmt, args);
	}
	void __cdecl TraceV(const char *pszFileName, int nLine,
		UINT nCategory, UINT nLevel, LPCWSTR pszFmt, va_list args) const
	{
		AtlTraceVU(m_hInst, pszFileName, nLine, nCategory, nLevel, pszFmt, args);
	}

	void RegisterCategory(UINT nCategory, LPCTSTR pszCategory)
		{AtlTraceRegisterCategory(m_hInst, nCategory, pszCategory);}

	bool LoadSettings(LPCTSTR pszFileName = NULL) const
		{return 0 != AtlTraceLoadSettings(pszFileName, FALSE);}
	void SaveSettings(LPCTSTR pszFileName = NULL) const
		{AtlTraceSaveSettings(pszFileName);}

protected:
	HINSTANCE m_hInst;
};

__declspec(selectany) CTrace g_AtlTrace;

class CTraceFileAndLineInfo
{
public:
	CTraceFileAndLineInfo(const char *pszFileName, int nLineNo)
		: m_pszFileName(pszFileName), m_nLineNo(nLineNo)
	{}

	void __cdecl operator()(UINT nCategory, UINT nLevel, const char *pszFmt, ...) const
	{
		va_list ptr; va_start(ptr, pszFmt);
		g_AtlTrace.TraceV(m_pszFileName, m_nLineNo, nCategory, nLevel, pszFmt, ptr);
		va_end(ptr);
	}
	void __cdecl operator()(UINT nCategory, UINT nLevel, const wchar_t *pszFmt, ...) const
	{
		va_list ptr; va_start(ptr, pszFmt);
		g_AtlTrace.TraceV(m_pszFileName, m_nLineNo, nCategory, nLevel, pszFmt, ptr);
		va_end(ptr);
	}
	void __cdecl operator()(const char *pszFmt, ...) const
	{
		va_list ptr; va_start(ptr, pszFmt);
		g_AtlTrace.TraceV(m_pszFileName, m_nLineNo, (UINT)~0, (UINT)~0, pszFmt, ptr);
		va_end(ptr);
	}
	void __cdecl operator()(const wchar_t *pszFmt, ...) const
	{
		va_list ptr; va_start(ptr, pszFmt);
		g_AtlTrace.TraceV(m_pszFileName, m_nLineNo, (UINT)~0, (UINT)~0, pszFmt, ptr);
		va_end(ptr);
	}

private:
	const char *const m_pszFileName;
	const int m_nLineNo;
};

};  // namespace ATL

#endif

namespace ATL
{

enum atlTraceFlags
{
	// Application defined categories
	// REVIEW: user vals necessary?
	atlTraceUser        = 0x0000009c,
	atlTraceUser2       = 0x0000009d,
	atlTraceUser3       = 0x0000009e,
	atlTraceUser4       = 0x0000009f,
	// ATL defined categories
	atlTraceGeneral     = 0x00000080,
	atlTraceCOM         = 0x00000081,
	atlTraceQI			= 0x00000082,
	atlTraceRegistrar   = 0x00000083,
	atlTraceRefcount    = 0x00000084,
	atlTraceWindowing   = 0x00000085,
	atlTraceControls    = 0x00000086,
	atlTraceHosting     = 0x00000087,
	atlTraceDBClient    = 0x00000088,
	atlTraceDBProvider  = 0x00000089,
	atlTraceSnapin      = 0x00000090,
	atlTraceNotImpl     = 0x0000009a,
	atlTraceAllocation  = 0x0000009b,
	atlTraceException	= 0x0000009c,
	atlTraceTime		= 0x0000009d,
	atlTraceCache		= 0x0000009e,
	atlTraceStencil		= 0x0000009f,
	atlTraceString		= 0x000000a0,
	atlTraceMap 		= 0x000000a1,
	atlTraceUtil		= 0x000000a2,
	atlTraceSecurity	= 0x000000a3,
	atlTraceSync		= 0x000000a4,
	atlTraceISAPI		= 0x000000a5,

	// Beginning of user-defined categories
	// REVIEW: necessary?
	atlTraceUserBase	= 0x10000000
};

#ifdef _DEBUG

#ifndef _ATL_NO_DEBUG_CRT
class CNoUIAssertHook
{
public:
	CNoUIAssertHook()
	{
		ATLASSERT( s_pfnPrevHook == NULL );
		s_pfnPrevHook = _CrtSetReportHook(CrtHookProc);
	}
	~CNoUIAssertHook()
	{
		_CrtSetReportHook(s_pfnPrevHook);
		s_pfnPrevHook = NULL;
	}

private:
	static int __cdecl CrtHookProc(int eReportType, char* pszMessage, int* pnRetVal)
	{
		if (eReportType == _CRT_ASSERT)
		{
			::OutputDebugStringA( "ASSERTION FAILED\n" );
			::OutputDebugStringA( pszMessage );
			*pnRetVal = 1;
			return TRUE;
		}

		if (s_pfnPrevHook != NULL)
		{
			return s_pfnPrevHook(eReportType, pszMessage, pnRetVal);
		}
		else
		{
			return FALSE;
		}
	}

private:
	static _CRT_REPORT_HOOK s_pfnPrevHook;
};

__declspec( selectany ) _CRT_REPORT_HOOK CNoUIAssertHook::s_pfnPrevHook = NULL;

#define DECLARE_NOUIASSERT() ATL::CNoUIAssertHook _g_NoUIAssertHook;

#endif  // _ATL_NO_DEBUG_CRT

#ifndef ATLTRACE
#define ATLTRACE ATL::CTraceFileAndLineInfo(__FILE__, __LINE__)
#define ATLTRACE2 ATLTRACE
#endif

inline void _cdecl AtlTrace(LPCSTR pszFormat, ...)
{
	va_list ptr;
	va_start(ptr, pszFormat);
	g_AtlTrace.TraceV(NULL, -1, (UINT)~0, (UINT)~0, pszFormat, ptr);
	va_end(ptr);
}

inline void _cdecl AtlTrace(LPCWSTR pszFormat, ...)
{
	va_list ptr;
	va_start(ptr, pszFormat);
	g_AtlTrace.TraceV(NULL, -1, (UINT)~0, (UINT)~0, pszFormat, ptr);
	va_end(ptr);
}

inline void _cdecl AtlTrace2(DWORD dwCategory, UINT nLevel, LPCSTR pszFormat, ...)
{
	va_list ptr;
	va_start(ptr, pszFormat);
	g_AtlTrace.TraceV(NULL, -1, dwCategory, nLevel, pszFormat, ptr);
	va_end(ptr);
}

inline void _cdecl AtlTrace2(DWORD dwCategory, UINT nLevel, LPCWSTR pszFormat, ...)
{
	va_list ptr;
	va_start(ptr, pszFormat);
	g_AtlTrace.TraceV(NULL, -1, dwCategory, nLevel, pszFormat, ptr);
	va_end(ptr);
}

#define ATLTRACENOTIMPL(funcname)   ATLTRACE(ATL::atlTraceNotImpl, 0, _T("ATL: %s not implemented.\n"), funcname); return E_NOTIMPL

#else // !DEBUG

inline void _cdecl AtlTraceNull(...){}
inline void _cdecl AtlTrace(LPCSTR , ...){}
inline void _cdecl AtlTrace2(DWORD, UINT, LPCSTR , ...){}
inline void _cdecl AtlTrace(LPCWSTR , ...){}
inline void _cdecl AtlTrace2(DWORD, UINT, LPCWSTR , ...){}
#ifndef ATLTRACE

// BUG BUG BUG  ATLTRACE != AtlTrace2!!!
#define ATLTRACE            __noop
#define ATLTRACE2           __noop
#endif //ATLTRACE
#define ATLTRACENOTIMPL(funcname)   return E_NOTIMPL
#define DECLARE_NOUIASSERT()

#endif //!_DEBUG

// Declare an instance of this class to automatically register a custom trace category at startup
class CTraceCategory
{
public:
	CTraceCategory( UINT idCategory, LPCTSTR pszCategoryName, UINT nStartingLevel = 0 ) 
		throw()
	{
#ifdef _DEBUG
		g_AtlTrace.RegisterCategory( idCategory, pszCategoryName );
		g_AtlTrace.ChangeCategory( idCategory, nStartingLevel, ATLTRACESTATUS_INHERIT);
#else
		(void)idCategory;
		(void)pszCategoryName;
		(void)nStartingLevel;
#endif
	}
};

};  // namespace ATL

#endif  // __ATLTRACE_H__
