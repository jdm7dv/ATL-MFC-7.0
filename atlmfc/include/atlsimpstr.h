// This is a part of the Active Template Library.
// Copyright (C) 1996-2000 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLSIMPSTR_H__
#define __ATLSIMPSTR_H__

#pragma once

#include <atldef.h>
#include <atlbase.h>
#include <atlexcept.h>
#include <atlmem.h>

//REVIEW: Just to fix VSEE
#pragma push_macro("min")
#pragma push_macro("max")
#undef min
#undef max
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#define max(a,b)            (((a) > (b)) ? (a) : (b))

namespace ATL
{

class IAtlStringMgr;
struct CStringData;

class IAtlStringMgr
{
public:
	// Allocate a new CStringData
	virtual CStringData* Allocate( int nAllocLength, int nCharSize ) throw() = 0;
	// Free an existing CStringData
	virtual void Free( CStringData* pData ) throw() = 0;
	// Change the size of an existing CStringData
	virtual CStringData* Reallocate( CStringData* pData, int nAllocLength, int nCharSize ) throw() = 0;
	// Get the CStringData for a Nil string
	virtual CStringData* GetNilString() throw() = 0;
	virtual IAtlStringMgr* Clone() throw() = 0;
};

#ifdef _M_IX86
#ifndef _M_CEE
extern "C"
{
	LONG _InterlockedIncrement( LONG* pn );
	LONG _InterlockedDecrement( LONG* pn );
};

#pragma intrinsic( _InterlockedIncrement )
#pragma intrinsic( _InterlockedDecrement )
#else
#define _InterlockedIncrement InterlockedIncrement
#define _InterlockedDecrement InterlockedDecrement
#endif  // !_M_CEE
#endif  // _M_IX86_

struct CStringData
{
	IAtlStringMgr* pStringMgr;  // String manager for this CStringData
	int nDataLength;  // Length of currently used data in XCHARs
	int nAllocLength;  // Length of allocated data in XCHARs
	long nRefs;     // Reference count: negative == locked
	// XCHAR data[nAllocLength]

	void* data() throw()
	{
		return (BYTE*)(this+1);
	}

	void AddRef() throw()
	{
		ATLASSERT(nRefs > 0);
		_InterlockedIncrement(&nRefs);
	}
	bool IsLocked() const throw()
	{
		return nRefs < 0;
	}
	bool IsShared() const throw()
	{
		return( nRefs > 1 ); 
	}
	void Lock() throw()
	{
		ATLASSERT( nRefs <= 1 );
		nRefs--;
		if( nRefs == 0 )
		{
			nRefs = -1;
		}
	}
	void Release() throw()
	{
		ATLASSERT( nRefs != 0 );

		if( _InterlockedDecrement( &nRefs ) <= 0 )
		{
			pStringMgr->Free( this );
		}
	}
	void Unlock() throw()
	{
		ATLASSERT( IsLocked() );
		nRefs++;
		if( nRefs == 0 )
		{
			nRefs = 1;
		}
	}
};

class CNilStringData :
	public CStringData
{
public:
	CNilStringData() throw()
	{
		pStringMgr = NULL;
		nRefs = 2;  // Never gets freed by IAtlStringMgr
		nDataLength = 0;
		nAllocLength = 0;
		achNil[0] = 0;
		achNil[1] = 0;
	}

	void SetManager( IAtlStringMgr* pMgr ) throw()
	{
		ATLASSERT( pStringMgr == NULL );
		pStringMgr = pMgr;
	}

public:
	wchar_t achNil[2];
};

class CAtlStringMgr :
	public IAtlStringMgr
{
public:
	CAtlStringMgr( IAtlMemMgr* pMemMgr = NULL ) throw() :
		m_pMemMgr( pMemMgr )
	{
		m_nil.SetManager( this );
	}
	~CAtlStringMgr() throw()
	{
	}

	void SetMemoryManager( IAtlMemMgr* pMemMgr ) throw()
	{
		ATLASSERT( m_pMemMgr == NULL );
		m_pMemMgr = pMemMgr;
	}

// IAtlStringMgr
public:
	virtual CStringData* Allocate( int nChars, int nCharSize ) throw()
	{
		size_t nTotalSize;
		CStringData* pData;
		size_t nDataBytes;

		nChars = AtlAlignUp( nChars + 1, 8 );  // Prevent excessive reallocation.  The heap will usually round up anyway.

		nDataBytes = nChars*nCharSize;
		nTotalSize = sizeof( CStringData )+nDataBytes;
		pData = (CStringData*)m_pMemMgr->Allocate( nTotalSize );
		if( pData == NULL )
		{
			return( NULL );
		}
		pData->pStringMgr = this;
		pData->nRefs = 1;
		pData->nAllocLength = nChars - 1;
		pData->nDataLength = 0;

		return( pData );
	}
	virtual void Free( CStringData* pData ) throw()
	{
		ATLASSERT( pData->pStringMgr == this );
		m_pMemMgr->Free( pData );
	}
	virtual CStringData* Reallocate( CStringData* pData, int nChars, int nCharSize ) throw()
	{
		CStringData* pNewData;
		ULONG nTotalSize;
		ULONG nDataBytes;

		ATLASSERT( pData->pStringMgr == this );
		nChars = AtlAlignUp( nChars+1, 8 );  // Prevent excessive reallocation.  The heap will usually round up anyway.

		nDataBytes = nChars*nCharSize;
		nTotalSize = sizeof( CStringData )+nDataBytes;
		pNewData = (CStringData*)m_pMemMgr->Reallocate( pData, nTotalSize );
		if( pNewData == NULL )
		{
			return NULL;
		}
		pNewData->nAllocLength = nChars - 1;

		return pNewData;
	}
	virtual CStringData* GetNilString() throw()
	{
		m_nil.AddRef();
		return &m_nil;
	}
	virtual IAtlStringMgr* Clone() throw()
	{
		return this;
	}

protected:
	IAtlMemMgr* m_pMemMgr;
	CNilStringData m_nil;
};

};  // namespace ATL

template< typename BaseType, const int t_nSize >
class CStaticString
{
public:
	CStaticString( const BaseType* psz ) :
		m_psz( psz )
	{
	}

	operator const BaseType*() const
	{
		return m_psz;
	}

	static int GetLength() 
	{
		return (t_nSize/sizeof( BaseType ))-1;
	}

	const BaseType* m_psz;
};

#define _ST( psz ) CStaticString< TCHAR, sizeof( _T( psz ) ) >( _T( psz ) )
#define _SA( psz ) CStaticString< char, sizeof( psz ) >( psz )
#define _SW( psz ) CStaticString< wchar_t, sizeof( L##psz ) >( L##psz )
#define _SO( psz ) _SW( psz )

template< typename BaseType = char >
class ChTraitsBase
{
public:
	typedef char XCHAR;
	typedef LPSTR PXSTR;
	typedef LPCSTR PCXSTR;
	typedef wchar_t YCHAR;
	typedef LPWSTR PYSTR;
	typedef LPCWSTR PCYSTR;
};

template<>
class ChTraitsBase< wchar_t >
{
public:
	typedef wchar_t XCHAR;
	typedef LPWSTR PXSTR;
	typedef LPCWSTR PCXSTR;
	typedef char YCHAR;
	typedef LPSTR PYSTR;
	typedef LPCSTR PCYSTR;
};

using ATL::IAtlStringMgr;

template< typename BaseType >
class CSimpleStringT
{
public:
	typedef ChTraitsBase< BaseType >::XCHAR XCHAR;
	typedef ChTraitsBase< BaseType >::PXSTR PXSTR;
	typedef ChTraitsBase< BaseType >::PCXSTR PCXSTR;
	typedef ChTraitsBase< BaseType >::YCHAR YCHAR;
	typedef ChTraitsBase< BaseType >::PYSTR PYSTR;
	typedef ChTraitsBase< BaseType >::PCYSTR PCYSTR;
	typedef CSimpleStringT< BaseType > CThisSimpleString;

public:
	explicit CSimpleStringT( IAtlStringMgr* pStringMgr ) throw()
	{
		ATL::CStringData* pData;

		ATLASSERT( pStringMgr != NULL );
		pData = pStringMgr->GetNilString();
		Attach( pData );
	}
	CSimpleStringT( const CThisSimpleString& strSrc )
	{
		ATL::CStringData* pSrcData;
		ATL::CStringData* pNewData;

		pSrcData = strSrc.GetData();
		pNewData = CloneData( pSrcData );

		Attach( pNewData );
	}
	CSimpleStringT( PCXSTR pszSrc, IAtlStringMgr* pStringMgr )
	{
		ATL::CStringData* pData;
		int nLength;

		ATLASSERT( pStringMgr != NULL );

		nLength = StringLength( pszSrc );
		pData = pStringMgr->Allocate( nLength, sizeof( XCHAR ) );
		if( pData == NULL )
		{
			ThrowMemoryException();
		}
		Attach( pData );
		SetLength( nLength );
		CopyChars( m_pszData, pszSrc, nLength );
	}
	CSimpleStringT( const XCHAR* pchSrc, int nLength, IAtlStringMgr* pStringMgr )
	{
		ATL::CStringData* pData;

		ATLASSERT( pStringMgr != NULL );

		pData = pStringMgr->Allocate( nLength, sizeof( XCHAR ) );
		if( pData == NULL )
		{
			ThrowMemoryException();
		}
		Attach( pData );
		SetLength( nLength );
		CopyChars( m_pszData, pchSrc, nLength );
	}
	~CSimpleStringT() throw()
	{
		ATL::CStringData* pData;

		pData = GetData();
		pData->Release();
	}

	CThisSimpleString& operator=( const CThisSimpleString& strSrc )
	{
		ATL::CStringData* pSrcData;
		ATL::CStringData* pNewData;
		ATL::CStringData* pOldData;

		pSrcData = strSrc.GetData();
		pOldData = GetData();
		if( pSrcData != pOldData )
		{
			if( pOldData->IsLocked() )
			{
				SetString( strSrc.GetString(), strSrc.GetLength() );
			}
			else
			{
				pNewData = CloneData( pSrcData );
				pOldData->Release();
				Attach( pNewData );
			}
		}

		return( *this );
	}
	CThisSimpleString& operator=( PCXSTR pszSrc )
	{
		SetString( pszSrc );

		return( *this );
	}

	CThisSimpleString& operator+=( const CThisSimpleString& strSrc )
	{
		Append( strSrc );

		return( *this );
	}
	CThisSimpleString& operator+=( PCXSTR pszSrc )
	{
		Append( pszSrc );

		return( *this );
	}
	template< int t_nSize >
	CThisSimpleString& operator+=( const CStaticString< XCHAR, t_nSize >& strSrc )
	{
		Append( strSrc.m_psz, strSrc.GetLength() );

		return( *this );
	}
	CThisSimpleString& operator+=( char ch )
	{
		XCHAR chTemp = XCHAR( ch );
		Append( &chTemp, 1 );

		return( *this );
	}
	CThisSimpleString& operator+=( wchar_t ch )
	{
		XCHAR chTemp = XCHAR( ch );
		Append( &chTemp, 1 );

		return( *this );
	}

	XCHAR operator[]( int iChar ) const throw()
	{
		ATLASSERT( (iChar >= 0) && (iChar <= GetLength()) );  // Indexing the '\0' is OK
		return( m_pszData[iChar] );
	}

	operator PCXSTR() const throw()
	{
		return( m_pszData );
	}

	void Append( PCXSTR pszSrc )
	{
		Append( pszSrc, StringLength( pszSrc ) );
	}
	void Append( PCXSTR pszSrc, int nLength )
	{
		UINT nOldLength;
		INT nNewLength;
		PXSTR pszBuffer;
		UINT_PTR nOffset;

		// See comment in SetString() about why we do this
		nOffset = pszSrc-GetString();

		nOldLength = GetLength();
		nNewLength = nOldLength+nLength;
		pszBuffer = GetBuffer( nNewLength );
		if( nOffset <= nOldLength )
		{
			pszSrc = pszBuffer+nOffset;
			// No need to call CopyCharsOverlapped, since the destination is
			// beyond the end of the original buffer
		}
		CopyChars( pszBuffer+nOldLength, pszSrc, nLength );
		ReleaseBuffer( nNewLength );
	}
	void Append( const CThisSimpleString& strSrc )
	{
		Append( strSrc.GetString(), strSrc.GetLength() );
	}
	void Empty() throw()
	{
		ATL::CStringData* pOldData;
		IAtlStringMgr* pStringMgr;
		
		pOldData = GetData();
		pStringMgr = pOldData->pStringMgr;
		if( pOldData->nDataLength == 0 )
		{
			return;
		}

		if( pOldData->IsLocked() )
		{
			// Don't reallocate a locked buffer that's shrinking
			SetLength( 0 );
		}
		else
		{
			ATL::CStringData* pNewData;

			pOldData->Release();
			pNewData = pStringMgr->GetNilString();
			Attach( pNewData );
		}
	}
	void FreeExtra() throw()
	{
		ATL::CStringData* pOldData;
		int nLength;
		IAtlStringMgr* pStringMgr;

		pOldData = GetData();
		nLength = pOldData->nDataLength;
		pStringMgr = pOldData->pStringMgr;
		if( pOldData->nAllocLength == nLength )
		{
			return;
		}

		if( !pOldData->IsLocked() )  // Don't reallocate a locked buffer that's shrinking
		{
			ATL::CStringData* pNewData;

			pNewData = pStringMgr->Allocate( nLength, sizeof( XCHAR ) );
			if( pNewData == NULL )
			{
				SetLength( nLength );
				return;
			}
			CopyChars( PXSTR( pNewData->data() ), PCXSTR( pOldData->data() ), nLength );
			pOldData->Release();
			Attach( pNewData );
			SetLength( nLength );
		}
	}

	int GetAllocLength() const throw()
	{
		return( GetData()->nAllocLength );
	}
	XCHAR GetAt( int iChar ) const throw()
	{
		ATLASSERT( (iChar >= 0) && (iChar <= GetLength()) );  // Indexing the '\0' is OK
		return( m_pszData[iChar] );
	}
	PXSTR GetBuffer()
	{
		ATL::CStringData* pData;
		
		pData = GetData();
		if( pData->IsShared() )
		{
			Fork( pData->nDataLength );
		}

		return( m_pszData );
	}
	PXSTR GetBuffer( int nMinBufferLength )
	{
		return( PrepareWrite( nMinBufferLength ) );
	}
	PXSTR GetBufferSetLength( int nLength )
	{
		PXSTR pszBuffer;

		pszBuffer = GetBuffer( nLength );
		SetLength( nLength );

		return( pszBuffer );
	}
	int GetLength() const throw()
	{
		return( GetData()->nDataLength );
	}
	IAtlStringMgr* GetManager() const throw()
	{
		return( GetData()->pStringMgr->Clone() );
	}

	PCXSTR GetString() const throw()
	{
		return( m_pszData );
	}
	bool IsEmpty() const throw()
	{
		return( GetLength() == 0 );
	}
	PXSTR LockBuffer()
	{
		ATL::CStringData* pData;

		pData = GetData();
		if( pData->IsShared() )
		{
			Fork( pData->nDataLength );
			pData = GetData();  // Do it again, because the fork might have changed it
		}
		pData->Lock();

		return( m_pszData );
	}
	void UnlockBuffer()
	{
		ATL::CStringData* pData;

		pData = GetData();
		pData->Unlock();
	}
	void Preallocate( int nLength )
	{
		PrepareWrite( nLength );
	}
	void ReleaseBuffer( int nNewLength = -1 ) throw()
	{
		if( nNewLength == -1 )
		{
			nNewLength = StringLength( m_pszData );
		}
		SetLength( nNewLength );
	}
	void Truncate( int nNewLength )
	{
		ATLASSERT( nNewLength <= GetLength() );
		GetBuffer( nNewLength );
		ReleaseBuffer( nNewLength );
	}
	void SetAt( int iChar, XCHAR ch )
	{
		PXSTR pszBuffer;
		int nLength;

		ATLASSERT( (iChar >= 0) && (iChar < GetLength()) );
		nLength = GetLength();
		pszBuffer = GetBuffer();
		pszBuffer[iChar] = ch;
		ReleaseBuffer( nLength );
	}
	void SetManager( IAtlStringMgr* pStringMgr )
	{
		ATL::CStringData* pData;

		ATLASSERT( IsEmpty() );

		pData = GetData();
		pData->Release();
		pData = pStringMgr->GetNilString();
		Attach( pData );
	}
	void SetString( PCXSTR pszSrc )
	{
		SetString( pszSrc, StringLength( pszSrc ) );
	}
	void SetString( PCXSTR pszSrc, int nLength )
	{
		if( nLength == 0 )
		{
			Empty();
		}
		else
		{
			PXSTR pszBuffer;
			UINT_PTR nOffset;
			UINT nOldLength;

			// It is possible that pszSrc points to a location inside of our 
			// buffer.  GetBuffer() might change m_pszData if (1) the buffer 
			// is shared or (2) the buffer is too small to hold the new 
			// string.  We detect this aliasing, and modify pszSrc to point
			// into the newly allocated buffer instead.

			nOldLength = GetLength();
			nOffset = pszSrc-GetString();
			// If 0 <= nOffset <= nOldLength, then pszSrc points into our 
			// buffer

			pszBuffer = GetBuffer( nLength );
			if( nOffset <= nOldLength )
			{
				CopyCharsOverlapped( pszBuffer, pszBuffer+nOffset, nLength );
			}
			else
			{
				CopyChars( pszBuffer, pszSrc, nLength );
			}
			ReleaseBuffer( nLength );
		}
	}

public:
	friend CThisSimpleString operator+(
		const CThisSimpleString& str1,
		const CThisSimpleString& str2 )
	{
		CThisSimpleString s( str1.GetManager() );

		Concatenate( s, str1, str1.GetLength(), str2, str2.GetLength() );

		return( s );
	}

	friend CThisSimpleString operator+(
		const CThisSimpleString& str1,
		PCXSTR psz2 )
	{
		CThisSimpleString s( str1.GetManager() );

		Concatenate( s, str1, str1.GetLength(), psz2, StringLength( psz2 ) );

		return( s );
	}

	friend CThisSimpleString operator+(
		PCXSTR psz1,
		const CThisSimpleString& str2 )
	{
		CThisSimpleString s( str2.GetManager() );

		Concatenate( s, psz1, StringLength( psz1 ), str2, str2.GetLength() );

		return( s );
	}

	static void CopyChars( XCHAR* pchDest, const XCHAR* pchSrc, int nChars ) throw()
	{
		memcpy( pchDest, pchSrc, nChars*sizeof( XCHAR ) );
	}
	static void CopyCharsOverlapped( XCHAR* pchDest, const XCHAR* pchSrc, int nChars ) throw()
	{
		memmove( pchDest, pchSrc, nChars*sizeof( XCHAR ) );
	}
	ATL_NOINLINE static int StringLength( PCXSTR psz ) throw()
	{
		int nLength;
		const XCHAR* pch;

		nLength = 0;
		if( psz != NULL )
		{
			pch = psz;
			while( *pch != 0 )
			{
				nLength++;
				pch++;
			}
		}

		return( nLength );
	}

protected:
	static void Concatenate( CThisSimpleString& strResult, PCXSTR psz1, int nLength1, PCXSTR psz2, int nLength2 )
	{
		PXSTR pszBuffer;
		int nNewLength;

		nNewLength = nLength1+nLength2;
		pszBuffer = strResult.GetBuffer( nNewLength );
		CopyChars( pszBuffer, psz1, nLength1 );
		CopyChars( pszBuffer+nLength1, psz2, nLength2 );
		strResult.ReleaseBuffer( nNewLength );
	}
	ATL_NOINLINE static void ThrowMemoryException()
	{
		ATL::AtlThrow( E_OUTOFMEMORY );
	}

// Implementation
private:
	void Attach( ATL::CStringData* pData ) throw()
	{
		m_pszData = static_cast< PXSTR >( pData->data() );
	}
	ATL_NOINLINE void Fork( int nLength )
	{
		ATL::CStringData* pOldData;
		ATL::CStringData* pNewData;
		int nOldLength;

		pOldData = GetData();
		nOldLength = pOldData->nDataLength;
		pNewData = pOldData->pStringMgr->Clone()->Allocate( nLength, sizeof( XCHAR ) );
		if( pNewData == NULL )
		{
			ThrowMemoryException();
		}
		CopyChars( PXSTR( pNewData->data() ), PCXSTR( pOldData->data() ), min(nOldLength, nLength)+1 );  // Copy '\0'
		pNewData->nDataLength = nOldLength;
		pOldData->Release();
		Attach( pNewData );
	}
	ATL::CStringData* GetData() const throw()
	{
		return( reinterpret_cast< ATL::CStringData* >( m_pszData )-1 );
	}
	PXSTR PrepareWrite( int nLength )
	{
		ATL::CStringData* pOldData;
		int nShared;
		int nTooShort;

		pOldData = GetData();
		nShared = 1-pOldData->nRefs;  // nShared < 0 means true, >= 0 means false
		nTooShort = pOldData->nAllocLength-nLength;  // nTooShort < 0 means true, >= 0 means false
		if( (nShared|nTooShort) < 0 )
		{
			PrepareWrite2( nLength );
		}

		return( m_pszData );
	}
	ATL_NOINLINE void PrepareWrite2( int nLength )
	{
		ATL::CStringData* pOldData;

		pOldData = GetData();
		nLength = max( nLength, pOldData->nDataLength );
		if( pOldData->IsShared() )
		{
			Fork( nLength );
		}
		else if( pOldData->nAllocLength < nLength )
		{
			int nNewLength;

			// Grow exponentially, until we hit 1K.
			nNewLength = pOldData->nAllocLength;
			if( nNewLength > 1024 )
			{
				nNewLength += 1024;
			}
			else
			{
				nNewLength *= 2;
			}
			if( nNewLength < nLength )
			{
				nNewLength = nLength;
			}
			Reallocate( nNewLength );
		}
	}
	ATL_NOINLINE void Reallocate( int nLength )
	{
		ATL::CStringData* pNewData;
		ATL::CStringData* pOldData;
		IAtlStringMgr* pStringMgr;

		pOldData = GetData();
		ATLASSERT( pOldData->nAllocLength < nLength );
		pStringMgr = pOldData->pStringMgr;
		pNewData = pStringMgr->Reallocate( pOldData, nLength, sizeof( XCHAR ) );
		if( pNewData == NULL )
		{
			ThrowMemoryException();
		}
		Attach( pNewData );
	}

	void SetLength( int nLength ) throw()
	{
		ATLASSERT( nLength >= 0 );
		ATLASSERT( nLength <= GetData()->nAllocLength );

		GetData()->nDataLength = nLength;
		m_pszData[nLength] = 0;
	}

	static ATL::CStringData* CloneData( ATL::CStringData* pData )
	{
		IAtlStringMgr* pNewStringMgr;
		ATL::CStringData* pNewData;

		pNewStringMgr = pData->pStringMgr->Clone();
		if( !pData->IsLocked() && (pNewStringMgr == pData->pStringMgr) )
		{
			pNewData = pData;
			pNewData->AddRef();
		}
		else
		{
			pNewData = pNewStringMgr->Allocate( pData->nDataLength, sizeof( XCHAR ) );
			if( pNewData == NULL )
			{
				ThrowMemoryException();
			}
			pNewData->nDataLength = pData->nDataLength;
			CopyChars( PXSTR( pNewData->data() ), PCXSTR( pData->data() ), pData->nDataLength+1 );  // Copy '\0'
		}

		return( pNewData );
	}

private:
	PXSTR m_pszData;
};

namespace ATL
{

typedef CSimpleStringT< TCHAR > CSimpleString;
typedef CSimpleStringT< char > CSimpleStringA;
typedef CSimpleStringT< wchar_t > CSimpleStringW;

};  // namespace ATL

//REVIEW: Just to fix VSEE
#pragma pop_macro("min")
#pragma pop_macro("max")

#endif  // __ATLSIMPSTR_H__
