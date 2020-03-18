// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-2000 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "fixalloc.h"
#include <afxtempl.h>

#ifdef AFX_CORE1_SEG
#pragma code_seg(AFX_CORE1_SEG)
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

LPWSTR AFXAPI AfxA2WHelper(LPWSTR lpw, LPCSTR lpa, int nChars)
{
	if (lpa == NULL)
		return NULL;
	ASSERT(lpw != NULL);
	// verify that no illegal character present
	// since lpw was allocated based on the size of lpa
	// don't worry about the number of chars
	lpw[0] = '\0';
	VERIFY(MultiByteToWideChar(CP_ACP, 0, lpa, -1, lpw, nChars));
	return lpw;
}

LPSTR AFXAPI AfxW2AHelper(LPSTR lpa, LPCWSTR lpw, int nChars)
{
	if (lpw == NULL)
		return NULL;
	ASSERT(lpa != NULL);
	// verify that no illegal character present
	// since lpa was allocated based on the size of lpw
	// don't worry about the number of chars
	lpa[0] = '\0';
	VERIFY(WideCharToMultiByte(CP_ACP, 0, lpw, -1, lpa, nChars, NULL, NULL));
	return lpa;
}

///////////////////////////////////////////////////////////////////////////////
// CComBSTR support for template collections

template<> UINT AFXAPI HashKey<CComBSTR> (CComBSTR key)
{
	UINT nHash = 0;
	int iLength = key.Length();
	BSTR str = key.m_str;

	for( int i = 0; i < iLength; i++ )
	{
		nHash = (nHash<<5) + nHash + *str;
		str++;
	}
	return nHash;
}

template<>
void AFXAPI SerializeElements< CComBSTR >(CArchive& ar, CComBSTR* pElements, INT_PTR nCount)
{
	SerializeElementsInsertExtract(ar, pElements, nCount);
}

///////////////////////////////////////////////////////////////////////////////
// CString support for template collections

#undef new

#if _MSC_VER >= 1100
template<> UINT AFXAPI HashKey<LPCWSTR> (LPCWSTR key)
#else
UINT AFXAPI HashKey(LPCWSTR key)
#endif
{
	UINT nHash = 0;
	while (*key)
		nHash = (nHash<<5) + nHash + *key++;
	return nHash;
}

#if _MSC_VER >= 1100
template<> UINT AFXAPI HashKey<LPCSTR> (LPCSTR key)
#else
UINT AFXAPI HashKey(LPCSTR key)
#endif
{
	UINT nHash = 0;
	while (*key)
		nHash = (nHash<<5) + nHash + *key++;
	return nHash;
}

template<>
void AFXAPI SerializeElements< CStringA >(CArchive& ar, CStringA* pElements, INT_PTR nCount)
{
	SerializeElementsInsertExtract(ar, pElements, nCount);
}

template<>
void AFXAPI SerializeElements< CStringW >(CArchive& ar, CStringW* pElements, INT_PTR nCount)
{
	SerializeElementsInsertExtract(ar, pElements, nCount);
}

#pragma warning(disable: 4074)
#pragma init_seg(compiler)

class CAfxStringMgr :
	public IAtlStringMgr
{
public:
	CAfxStringMgr()
	{
		m_nil.SetManager( this );
	}

// IAtlStringMgr
public:
	virtual CStringData* Allocate( int nChars, int nCharSize );
	virtual void Free( CStringData* pData );
	virtual CStringData* Reallocate( CStringData* pData, int nChars, int nCharSize );
	virtual CStringData* GetNilString();
	virtual IAtlStringMgr* Clone();

protected:
	CNilStringData m_nil;
};

CAfxStringMgr afxStringManager;

IAtlStringMgr* AFXAPI AfxGetStringManager()
{
	return &afxStringManager;
}

CStringData* CAfxStringMgr::Allocate( int nChars, int nCharSize )
{
	size_t nTotalSize;
	CStringData* pData;
	size_t nDataBytes;

	nDataBytes = (nChars+1)*nCharSize;
	nTotalSize = sizeof( CStringData )+nDataBytes;
	pData = (CStringData*)malloc( nTotalSize );
	if (pData == NULL)
		return NULL;
	pData->pStringMgr = this;
	pData->nRefs = 1;
	pData->nAllocLength = nChars;
	pData->nDataLength = 0;

	return pData;
}

void CAfxStringMgr::Free( CStringData* pData )
{
	free(pData);
}

CStringData* CAfxStringMgr::Reallocate( CStringData* pData, int nChars, int nCharSize )
{
	CStringData* pNewData;
	size_t nTotalSize;
	size_t nDataBytes;

	nDataBytes = (nChars+1)*nCharSize;
	nTotalSize = sizeof( CStringData )+nDataBytes;
	pNewData = (CStringData*)realloc( pData, nTotalSize );
	if( pNewData == NULL )
	{
		return NULL;
	}
	pNewData->nAllocLength = nChars;

	return pNewData;
}


CStringData* CAfxStringMgr::GetNilString()
{
	m_nil.AddRef();
	return &m_nil;
}

IAtlStringMgr* CAfxStringMgr::Clone()
{
	return this;
}

/////////////////////////////////////////////////////////////////////////////
