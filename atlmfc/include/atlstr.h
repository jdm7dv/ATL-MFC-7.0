// This is a part of the Active Template Library.
// Copyright (C) 1996-2000 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLSTR_H__
#define __ATLSTR_H__

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#include <atlbase.h>

#pragma once

#include <winnls.h>
#include <limits.h>

#include <cstringt.h>

namespace ATL
{

extern CAtlStringMgr g_strmgr;

class CExceptionTraitsATL
{
public:
	static void ThrowMemoryException()
	{
		AtlThrow( E_OUTOFMEMORY );
	}
};

template< typename _BaseType = char, class StringIterator = ChTraitsOS< _BaseType > >
class StrTraitATL : public StringIterator
{
public:
	typedef CExceptionTraitsATL ExceptionTraits;

	static HINSTANCE FindStringResourceInstance(UINT /* nID */)
	{
		return( _AtlBaseModule.GetResourceInstance() );
	}

	static IAtlStringMgr* GetDefaultManager()
	{
		return( &g_strmgr );
	}

	static void ThrowMemoryException()
	{
		ATL::AtlThrow( E_OUTOFMEMORY );
	}

	static void ThrowResourceException()
	{
		ATL::AtlThrow( E_FAIL );
	}
};

#if !defined(_ATL_CSTRING_NO_CRT) && defined(_ATL_MIN_CRT)
#define _ATL_CSTRING_NO_CRT
#endif

#ifndef _ATL_CSTRING_NO_CRT
typedef CStringT< wchar_t, StrTraitATL< wchar_t, ChTraitsCRT< wchar_t > > > CAtlStringW;
typedef CStringT< char, StrTraitATL< char, ChTraitsCRT< char > > > CAtlStringA;
typedef CStringT< TCHAR, StrTraitATL< TCHAR, ChTraitsCRT< TCHAR > > > CAtlString;
#else  // _ATL_CSTRING_NO_CRT
typedef CStringT< wchar_t, StrTraitATL< wchar_t > > CAtlStringW;
typedef CStringT< char, StrTraitATL< char > > CAtlStringA;
typedef CStringT< TCHAR, StrTraitATL< TCHAR > > CAtlString;
#endif  // _ATL_CSTRING_NO_CRT

#ifndef _AFX
typedef CAtlStringW CStringW;
typedef CAtlStringA CStringA;
typedef CAtlString CString;
#endif

#ifdef __ATLCOLL_H__

template<>
class CElementTraits< ATL::CAtlStringA > :
	public CStringElementTraits< ATL::CAtlStringA >
{
};

template<>
class CElementTraits< ATL::CAtlStringW > :
	public CStringElementTraits< ATL::CAtlStringW >
{
};

#endif  // __ATLCOLL_H__

}; //namespace ATL

#endif // __ATLSTR_H__

/////////////////////////////////////////////////////////////////////////////
