// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-2000 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

/////////////////////////////////////////////////////////////////////////////
// AFXSTR.H - Framework-independent, templateable string class

#ifndef __AFXSTR_H__
#define __AFXSTR_H__

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _AFX
#error afxstr.h can only be used in MFC projects.  Use atlstr.h
#endif

#include <mbstring.h>

HINSTANCE AFXAPI AfxGetResourceHandle();
HINSTANCE AFXAPI AfxFindStringResourceHandle(UINT nID);

UINT_PTR AFXAPI AfxReadStringLength(CArchive& ar, int& nCharSize);
void AFXAPI AfxWriteStringLength(CArchive& ar, UINT_PTR nLength, BOOL bUnicode);

#include <crtdbg.h>
#include <atlbase.h>
#include <cstringt.h>

IAtlStringMgr* AFXAPI AfxGetStringManager();

/*
class StrMgrTraitMFC
{
public:
	static IAtlStringMgr* GetDefaultManager()
	{
		return AfxGetStringManager();
	}
};
*/

class CExceptionTraitsMFC
{
public:
	static void ThrowMemoryException()
	{
		AfxThrowMemoryException();
	}
};

template< typename _CharType = char, class StringIterator = ChTraitsCRT< _CharType > >
class StrTraitMFC : 
	public StringIterator
{
public:
	typedef CExceptionTraitsMFC ExceptionTraits;

public:
	static HINSTANCE FindStringResourceInstance( UINT nID ) throw()
	{
		return( AfxFindStringResourceHandle( nID ) );
	}

	static IAtlStringMgr* GetDefaultManager() throw()
	{
		return( AfxGetStringManager() );
	}
};

// MFC-enabled compilation. Use MFC memory management and exceptions;
// also, use MFC module state.
typedef CStringT< wchar_t, StrTraitMFC< wchar_t > > CStringW;
typedef CStringT< char, StrTraitMFC< char > > CStringA;
typedef CStringT< TCHAR, StrTraitMFC< TCHAR > > CString;

template< typename BaseType, class StringTraits >
CArchive& AFXAPI operator<<(CArchive& ar,
	const CStringT<BaseType, StringTraits>& string)
{
	const BaseType* pch;
	UINT_PTR nCharsLeft;

	AfxWriteStringLength(ar, string.GetLength(), sizeof(BaseType) == sizeof(wchar_t));
	nCharsLeft = string.GetLength();
	pch = string;
	while (nCharsLeft > 0)
	{
		UINT nCharsToWrite;

		nCharsToWrite = (UINT)min(nCharsLeft, INT_MAX/sizeof(BaseType));
		ar.Write(pch, nCharsToWrite*sizeof(BaseType));
		pch += nCharsToWrite;
		nCharsLeft -= nCharsToWrite;
	}

	return ar;
}

template< typename BaseType, class StringTraits >
CArchive& AFXAPI operator>>(CArchive& ar,
	CStringT<BaseType, StringTraits>& string)
{
	UINT_PTR nLength;
	int nCharSize;  // 1 = char, 2 = wchar_t

	nLength = AfxReadStringLength(ar, nCharSize);
	if (nCharSize == sizeof(char))
	{
		UINT_PTR nCharsLeft;
		char* pch;
		CTempBuffer< char > pszBufferA(nLength+1);

		pszBufferA[nLength] = '\0';
		pch = pszBufferA;
		nCharsLeft = nLength;
		while (nCharsLeft > 0)
		{
			UINT nBytesRead;
			UINT nCharsToRead;

			nCharsToRead = (UINT)min(nCharsLeft, INT_MAX);
			nBytesRead = ar.Read(pch, nCharsToRead*sizeof(char));
			if (nBytesRead != (nCharsToRead*sizeof(char)))
				AfxThrowArchiveException(CArchiveException::endOfFile);
			pch += nCharsToRead;
			nCharsLeft -= nCharsToRead;
		}
		string = pszBufferA;
	}
	else
	{
		ASSERT(nCharSize == sizeof(wchar_t));

		UINT_PTR nCharsLeft;
		wchar_t* pch;
		CTempBuffer< wchar_t > pszBufferW( nLength+1 );

		pszBufferW[nLength] = L'\0';
		pch = pszBufferW;
		nCharsLeft = nLength;
		while (nCharsLeft > 0)
		{
			UINT nBytesRead;
			UINT nCharsToRead;

			nCharsToRead = (UINT)min(nCharsLeft, INT_MAX/2);
			nBytesRead = ar.Read(pch, nCharsToRead*sizeof(wchar_t));
			if (nBytesRead != (nCharsToRead*sizeof(wchar_t)))
				AfxThrowArchiveException(CArchiveException::endOfFile);
			pch += nCharsToRead;
			nCharsLeft -= nCharsToRead;
		}
		string = pszBufferW;
	}

	return ar;
}

//////////////////////////////////////////////////////////////////////////////
// Diagnostic support

#ifdef _DEBUG
template< typename BaseType, class StringTraits >
inline CDumpContext& AFXAPI operator<<(CDumpContext& dc, const CStringT<BaseType, 
	StringTraits>& string)
{
	dc << (const BaseType*)string;
	return dc;
}
#endif //_DEBUG

#endif	// __AFXSTR_H__ (whole file)
