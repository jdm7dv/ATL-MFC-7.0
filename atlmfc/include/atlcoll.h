// This is a part of the Active Template Library.
// Copyright (C) 1996-2000 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLCOLL_H__
#define __ATLCOLL_H__

#pragma once

#pragma warning(push)
#pragma warning(disable: 4702)  // Unreachable code.  This file will have lots of it, especially without EH enabled.
#pragma warning(disable: 4512)  // assignment operator could not be generated
#pragma warning(disable: 4290)  // C++ Exception Specification ignored

// abstract iteration position
#ifndef _AFX
struct __POSITION
{
};
#endif
typedef __POSITION* POSITION;

#include <atlbase.h>
#include <new.h>

#ifndef _AFX_PACKING
#define _AFX_PACKING 4
#endif

namespace ATL {

struct CAtlPlex     // warning variable length structure
{
	CAtlPlex* pNext;
#if (_AFX_PACKING >= 8)
	DWORD dwReserved[1];    // align on 8 byte boundary
#endif
	// BYTE data[maxNum*elementSize];

	void* data() { return this+1; }

	static CAtlPlex* Create(CAtlPlex*& head, size_t nMax, size_t cbElement);
			// like 'calloc' but no zero fill
			// may throw memory exceptions

	void FreeDataChain();       // free this one and links
};

inline CAtlPlex* CAtlPlex::Create( CAtlPlex*& pHead, size_t nMax, size_t nElementSize )
{
	CAtlPlex* pPlex;

	ATLASSERT( nMax > 0 );
	ATLASSERT( nElementSize > 0 );

	pPlex = static_cast< CAtlPlex* >( malloc( sizeof( CAtlPlex )+(nMax*nElementSize) ) );
	if( pPlex == NULL )
	{
		return( NULL );
	}

	pPlex->pNext = pHead;
	pHead = pPlex;

	return( pPlex );
}

inline void CAtlPlex::FreeDataChain()
{
	CAtlPlex* pPlex;

	pPlex = this;
	while( pPlex != NULL )
	{
		CAtlPlex* pNext;

		pNext = pPlex->pNext;
		free( pPlex );
		pPlex = pNext;
	}
}

class CUINTHash
{
public:
	CUINTHash( UINT nHash ) :
		m_nHash( nHash )
	{
	}

	/*
	bool operator==( const CUINTHash& hash ) const
	{
		return( m_nHash == hash.m_nHash );
	}
	*/

	static bool StoresHash()
	{
		return( true );
	}

protected:
	UINT m_nHash;
};

class CVoidHash
{
public:
	CVoidHash( UINT nHash )
	{
		(void)nHash;
	}

	static bool StoresHash()
	{
		return( false );
	}

	/*
	bool operator==( const CVoidHash& hash ) const
	{
		(void)hash;
		return( true );
	}
	*/
};

template< typename T >
class CElementTraitsBase
{
public:
	typedef const T& INARGTYPE;
	typedef T& OUTARGTYPE;
	typedef DWORD SORTKEY;
	typedef CUINTHash HASHTYPE;
	
	static SORTKEY GetSortKey( INARGTYPE t )
	{
		(void)t;
		return( 0 );
	}

	static ULONG Hash( INARGTYPE t )
	{
		return( ULONG( ULONG_PTR( t ) ) );
	}

	static void CopyElements( T* pDest, const T* pSrc, size_t nElements )
	{
		for( size_t iElement = 0; iElement < nElements; iElement++ )
		{
			pDest[iElement] = pSrc[iElement];
		}
	}

	static void RelocateElements( T* pDest, T* pSrc, size_t nElements )
	{
		// A simple memmove works for nearly all types.
		// You'll have to override this for types that have pointers to their
		// own members.
		memmove( pDest, pSrc, nElements*sizeof( T ) );
	}

	static bool CompareElements( INARGTYPE element1, INARGTYPE element2 )
	{
		return( element1 == element2 );
	}

	static int CompareElementsOrdered( INARGTYPE element1, INARGTYPE element2 )
	{
		if( element1 < element2 )
		{
			return( -1 );
		}
		else if( element1 == element2 )
		{
			return( 0 );
		}
		else
		{
			ATLASSERT( element1 > element2 );
			return( 1 );
		}
	}
};

template<>
class CElementTraitsBase< GUID >
{
public:
	typedef const GUID& INARGTYPE;
	typedef GUID& OUTARGTYPE;
	typedef DWORD SORTKEY;
	typedef CUINTHash HASHTYPE;

	static SORTKEY GetSortKey( INARGTYPE t )
	{
		(void)t;
		return( 0 );
	}

	static ULONG Hash( INARGTYPE guid )
	{
		const DWORD* pdwData;

		pdwData = reinterpret_cast< const DWORD* >( &guid );

		return( pdwData[0]^pdwData[1]^pdwData[2]^pdwData[3] );
	}

	static void CopyElements( GUID* pDest, const GUID* pSrc, size_t nElements )
	{
		for( size_t iElement = 0; iElement < nElements; iElement++ )
		{
			pDest[iElement] = pSrc[iElement];
		}
	}

	static void RelocateElements( GUID* pDest, GUID* pSrc, size_t nElements )
	{
		// A simple memmove works for nearly all types.
		// You'll have to override this for types that have pointers to their
		// own members.
		memmove( pDest, pSrc, nElements*sizeof( GUID ) );
	}

	static BOOL CompareElements( INARGTYPE element1, INARGTYPE element2 )
	{
		return( InlineIsEqualGUID( element1, element2 ) ); 
	}

	static int CompareElementsOrdered( INARGTYPE element1, INARGTYPE element2 )
	{
		const DWORD* pdwData1;
		const DWORD* pdwData2;

		pdwData1 = reinterpret_cast< const DWORD* >( &element1 );
		pdwData2 = reinterpret_cast< const DWORD* >( &element2 );

		for( int iDWORD = 3; iDWORD >= 0; iDWORD-- )
		{
			if( pdwData1[iDWORD] > pdwData2[iDWORD] )
			{
				return( 1 );
			}
			else if( pdwData1[iDWORD] < pdwData2[iDWORD] )
			{
				return( -1 );
			}
		}

		return( 0 );
	}
};

template< >
class CElementTraitsBase< CComVariant >
{
public:
	typedef const VARIANT& INARGTYPE;
	typedef CComVariant& OUTARGTYPE;
	typedef DWORD SORTKEY;
	typedef CUINTHash HASHTYPE;
	
	static SORTKEY GetSortKey( INARGTYPE t )
	{
		(void)t;
		return( 0 );
	}

	static ULONG Hash( INARGTYPE t )
	{
		t;
		ATLASSERT(FALSE); // variant hashing is problematic
		return( 0 );
	}

	static void CopyElements( T* pDest, const T* pSrc, size_t nElements )
	{
		for( size_t iElement = 0; iElement < nElements; iElement++ )
		{
			pDest[iElement] = pSrc[iElement];
		}
	}

	static void RelocateElements( T* pDest, T* pSrc, size_t nElements )
	{
		// A simple memmove works for nearly all types.
		// You'll have to override this for types that have pointers to their
		// own members.
		memmove( pDest, pSrc, nElements*sizeof( T ) );
	}

	static bool CompareElements( INARGTYPE element1, INARGTYPE element2 )
	{
		return VarCmp(const_cast<VARIANT*>(&element1), const_cast<VARIANT*>(&element2), LOCALE_USER_DEFAULT, 0)==VARCMP_EQ;
	}

	static int CompareElementsOrdered( INARGTYPE element1, INARGTYPE element2 )
	{
		HRESULT hr = VarCmp(const_cast<VARIANT*>(&element1), const_cast<VARIANT*>(&element2), LOCALE_USER_DEFAULT, 0);
		if( hr == VARCMP_LT )
		{
			return( -1 );
		}
		else if( hr == VARCMP_GT )
		{
			return( 1 );
		}
		else
		{
			ATLASSERT( hr == VARCMP_EQ || hr == VARCMP_NULL );
			return( 0 );
		}
	}
};

template< typename T >
class CElementTraits :
	public CElementTraitsBase< T >
{
};

template< typename I, const IID* piid = &__uuidof( I ) >
class CComQIPtrElementTraits :
	public CElementTraitsBase< ATL::CComQIPtr< I, piid > >
{
public:
	typedef I* INARGTYPE;
};

template< typename T >
class CAutoPtrElementTraits :
	public CElementTraitsBase< ATL::CAutoPtr< T > >
{
public:
	typedef ATL::CAutoPtr< T >& INARGTYPE;
	typedef T*& OUTARGTYPE;
};

template< typename T >
class CAutoVectorPtrElementTraits :
	public CElementTraitsBase< ATL::CAutoVectorPtr< T > >
{
public:
	typedef ATL::CAutoVectorPtr< T >& INARGTYPE;
	typedef T*& OUTARGTYPE;
};

template< typename T, class Allocator = ATL::CCRTAllocator >
class CHeapPtrElementTraits :
	public CElementTraitsBase< ATL::CHeapPtr< T, Allocator > >
{
public:
	typedef ATL::CHeapPtr< T, Allocator >& INARGTYPE;
	typedef T*& OUTARGTYPE;
};

template< typename T >
class CStringElementTraits
{
public:
	typedef T::PCXSTR INARGTYPE;
	typedef T& OUTARGTYPE;
	typedef ULONGLONG SORTKEY;
	typedef CUINTHash HASHTYPE;
	
	static SORTKEY GetSortKey( INARGTYPE str )
	{
		(void)str;
		return( 0 );
	}

	static ULONG Hash( INARGTYPE str )
	{
		const T::XCHAR* pch;
		ULONG nHash;

		ATLASSERT( str != NULL );
		nHash = 0;
		pch = str;
		while( *pch != 0 )
		{
			nHash = (nHash<<5)+nHash+(*pch);
			pch++;
		}

		return( nHash );
	}

	static void CopyElements( T* pDest, const T* pSrc, size_t nElements )
	{
		for( size_t iElement = 0; iElement < nElements; iElement++ )
		{
			pDest[iElement] = pSrc[iElement];
		}
	}

	static void RelocateElements( T* pDest, T* pSrc, size_t nElements )
	{
		// A simple memmove works for nearly all types.
		// You'll have to override this for types that have pointers to their
		// own members.
		memmove( pDest, pSrc, nElements*sizeof( T ) );
	}

	static bool CompareElements( INARGTYPE str1, INARGTYPE str2 )
	{
		return( T::StrTraits::StringCompare( str1, str2 ) == 0 );
	}

	static int CompareElementsOrdered( INARGTYPE str1, INARGTYPE str2 )
	{
		return( T::StrTraits::StringCompare( str1, str2 ) );
	}
};

template < typename T >
class CDefaultCharTraits
{
};

template <>
class CDefaultCharTraits<char>
{
public:
    static char CharToUpper(char x)
    {
        return (char)toupper(x);
    }

    static char CharToLower(char x)
    {
        return (char)tolower(x);
    }
};

template <>
class CDefaultCharTraits<wchar_t>
{
public:
    static wchar_t CharToUpper(wchar_t x)
    {
        return (wchar_t)towupper(x);
    }

    static wchar_t CharToLower(wchar_t x)
    {
        return (wchar_t)towlower(x);
    }
};

template< typename T, class CharTraits = CDefaultCharTraits<T::XCHAR> >
class CStringElementTraitsI
{
public:
	typedef T::PCXSTR INARGTYPE;
	typedef T& OUTARGTYPE;
	typedef ULONGLONG SORTKEY;
	typedef CUINTHash HASHTYPE;
	
	static SORTKEY GetSortKey( INARGTYPE str )
	{
		(void)str;
		return( 0 );
	}

	static ULONG Hash( INARGTYPE str )
	{
		const T::XCHAR* pch;
		ULONG nHash;

		ATLASSERT( str != NULL );
		nHash = 0;
		pch = str;
		while( *pch != 0 )
		{
			nHash = (nHash<<5)+nHash+CharTraits::CharToUpper(*pch);
			pch++;
		}

		return( nHash );
	}

	static void CopyElements( T* pDest, const T* pSrc, size_t nElements )
	{
		for( size_t iElement = 0; iElement < nElements; iElement++ )
		{
			pDest[iElement] = pSrc[iElement];
		}
	}

	static void RelocateElements( T* pDest, T* pSrc, size_t nElements )
	{
		// A simple memmove works for nearly all types.
		// You'll have to override this for types that have pointers to their
		// own members.
		memmove( pDest, pSrc, nElements*sizeof( T ) );
	}

	static bool CompareElements( INARGTYPE str1, INARGTYPE str2 )
	{
		return( T::StrTraits::StringCompareIgnore( str1, str2 ) == 0 );
	}

	static int CompareElementsOrdered( INARGTYPE str1, INARGTYPE str2 )
	{
		return( T::StrTraits::StringCompareIgnore( str1, str2 ) );
	}
};

template< typename T >
class CStringRefElementTraits
{
public:
	typedef const T& INARGTYPE;
	typedef T& OUTARGTYPE;
	typedef ULONGLONG SORTKEY;
	typedef CUINTHash HASHTYPE;
	
	static SORTKEY GetSortKey( INARGTYPE str )
	{
		(void)((T::PCXSTR)str);
		return( 0 );
	}

	static ULONG Hash( INARGTYPE str )
	{
		const T::XCHAR* pch;
		ULONG nHash;

		nHash = 0;
		pch = str;
		while( *pch != 0 )
		{
			nHash = (nHash<<5)+nHash+(*pch);
			pch++;
		}

		return( nHash );
	}

	static void CopyElements( T* pDest, const T* pSrc, size_t nElements )
	{
		for( size_t iElement = 0; iElement < nElements; iElement++ )
		{
			pDest[iElement] = pSrc[iElement];
		}
	}

	static void RelocateElements( T* pDest, T* pSrc, size_t nElements )
	{
		// A simple memmove works for nearly all types.
		// You'll have to override this for types that have pointers to their
		// own members.
		memmove( pDest, pSrc, nElements*sizeof( T ) );
	}

	static bool CompareElements( INARGTYPE element1, INARGTYPE element2 )
	{
		return( element1 == element2 );
	}

	static int CompareElementsOrdered( INARGTYPE str1, INARGTYPE str2 )
	{
		return( str1.Compare( str2 ) );
	}
};

template< typename T >
class CPrimitiveElementTraits :
	public CElementTraitsBase< T >
{
public:
	typedef T INARGTYPE;
	typedef T& OUTARGTYPE;
	typedef CVoidHash HASHTYPE;

	static ULONG Hash( INARGTYPE t )
	{
		return( ULONG( ULONG_PTR( t ) ) );
	}
};

#define _DECLARE_PRIMITIVE_TRAITS( T ) \
	template<> \
	class CElementTraits< T > : \
		public CPrimitiveElementTraits< T > \
	{ \
	};

_DECLARE_PRIMITIVE_TRAITS( unsigned char )
_DECLARE_PRIMITIVE_TRAITS( unsigned short )
_DECLARE_PRIMITIVE_TRAITS( unsigned int )
_DECLARE_PRIMITIVE_TRAITS( unsigned long )
//_DECLARE_PRIMITIVE_TRAITS( unsigned __int8 )
//_DECLARE_PRIMITIVE_TRAITS( unsigned __int16 )
//_DECLARE_PRIMITIVE_TRAITS( unsigned __int32 )
_DECLARE_PRIMITIVE_TRAITS( unsigned __int64 )
_DECLARE_PRIMITIVE_TRAITS( signed char )
_DECLARE_PRIMITIVE_TRAITS( short )
_DECLARE_PRIMITIVE_TRAITS( int )
_DECLARE_PRIMITIVE_TRAITS( long )
//_DECLARE_PRIMITIVE_TRAITS( __int8 )
//_DECLARE_PRIMITIVE_TRAITS( __int16 )
//_DECLARE_PRIMITIVE_TRAITS( __int32 )
_DECLARE_PRIMITIVE_TRAITS( __int64 )
_DECLARE_PRIMITIVE_TRAITS( float )
_DECLARE_PRIMITIVE_TRAITS( double )
_DECLARE_PRIMITIVE_TRAITS( bool )
#ifdef _AFX_WCHAR_T
_DECLARE_PRIMITIVE_TRAITS( wchar_t )
#endif
_DECLARE_PRIMITIVE_TRAITS( void* )

template< typename E, class ETraits = CElementTraits< E > >
class CAtlArray
{
public:
	typedef ETraits::INARGTYPE INARGTYPE;
	typedef ETraits::OUTARGTYPE OUTARGTYPE;

public:
	CAtlArray() throw();

	size_t GetCount() const throw();
	ATL_DEPRECATED size_t GetSize() const throw();
	bool IsEmpty() const throw();
	bool SetSize( size_t nNewSize, int nGrowBy = -1 );

	void FreeExtra() throw();
	void RemoveAll() throw();

	const E& GetAt( size_t iElement ) const throw();
	void SetAt( size_t iElement, INARGTYPE element );
	E& GetAt( size_t iElement ) throw();

	const E* GetData() const throw();
	E* GetData() throw();

	void SetAtGrow( size_t iElement, INARGTYPE element );
	// Add an empty element to the end of the array
	size_t Add();
	// Add an element to the end of the array
	size_t Add( INARGTYPE element );
	size_t Append( const CAtlArray< E, ETraits >& aSrc );
	void Copy( const CAtlArray< E, ETraits >& aSrc );

	const E& operator[]( size_t iElement ) const throw();
	E& operator[]( size_t iElement ) throw();

	void InsertAt( size_t iElement, INARGTYPE element, size_t nCount = 1 );
	void InsertArrayAt( size_t iStart, const CAtlArray< E, ETraits >* paNew );
	void RemoveAt( size_t iElement, size_t nCount = 1 );

#ifdef _DEBUG
	void AssertValid() const;
#endif  // _DEBUG

private:
	bool GrowBuffer( size_t nNewSize, int nGrowBy );

// Implementation
private:
	E* m_pData;
	size_t m_nSize;
	size_t m_nMaxSize;
	int m_nGrowBy;

private:
	static void CallConstructors( E* pElements, size_t nElements );
	static void CallDestructors( E* pElements, size_t nElements );

public:
	~CAtlArray() throw();
};

template< class I, const IID* piid = &__uuidof( I ) >
class CInterfaceArray :
	public CAtlArray< ATL::CComQIPtr< I, piid >, CComQIPtrElementTraits< I, piid > >
{
};

template< typename E >
class CAutoPtrArray :
	public CAtlArray< ATL::CAutoPtr< E >, CAutoPtrElementTraits< E > >
{
};

template< typename E, class Allocator = ATL::CCRTAllocator >
class CHeapPtrArray :
	public CAtlArray< ATL::CHeapPtr< E, Allocator >, CHeapPtrElementTraits< E, Allocator > >
{
};

template< typename E, class ETraits >
inline size_t CAtlArray< E, ETraits >::GetCount() const
{
	return( m_nSize );
}

template< typename E, class ETraits >
inline size_t CAtlArray< E, ETraits >::GetSize() const
{
	return( m_nSize );
}

template< typename E, class ETraits >
inline bool CAtlArray< E, ETraits >::IsEmpty() const
{
	return( m_nSize == 0 );
}

template< typename E, class ETraits >
inline void CAtlArray< E, ETraits >::RemoveAll()
{
	SetSize( 0, -1 );
}

template< typename E, class ETraits >
inline const E& CAtlArray< E, ETraits >::GetAt( size_t iElement ) const
{
	ATLASSERT( iElement < m_nSize );
	return( m_pData[iElement] );
}

template< typename E, class ETraits >
inline void CAtlArray< E, ETraits >::SetAt( size_t iElement, INARGTYPE element )
{
	ATLASSERT( iElement < m_nSize );
	m_pData[iElement] = element;
}

template< typename E, class ETraits >
inline E& CAtlArray< E, ETraits >::GetAt( size_t iElement )
{
	ATLASSERT( iElement < m_nSize );
	return( m_pData[iElement] );
}

template< typename E, class ETraits >
inline const E* CAtlArray< E, ETraits >::GetData() const
{
	return( m_pData );
}

template< typename E, class ETraits >
inline E* CAtlArray< E, ETraits >::GetData()
{
	return( m_pData );
}

template< typename E, class ETraits >
inline size_t CAtlArray< E, ETraits >::Add()
{
	size_t iElement;

	iElement = m_nSize;
	SetSize( m_nSize+1 );

	return( iElement );
}

#pragma push_macro("new")
#undef new

template< typename E, class ETraits >
inline size_t CAtlArray< E, ETraits >::Add( INARGTYPE element )
{
	size_t iElement;

	iElement = m_nSize;
	if( iElement >= m_nMaxSize )
	{
		bool bSuccess = GrowBuffer( iElement+1, -1 );
		if( !bSuccess )
		{
			ATL::AtlThrow( E_OUTOFMEMORY );
		}
	}
	::new( m_pData+iElement ) E( element );
	m_nSize++;

	return( iElement );
}

#pragma pop_macro("new")

template< typename E, class ETraits >
inline const E& CAtlArray< E, ETraits >::operator[]( size_t iElement ) const
{
	ATLASSERT( iElement < m_nSize );
	return( m_pData[iElement] );
}

template< typename E, class ETraits >
inline E& CAtlArray< E, ETraits >::operator[]( size_t iElement )
{
	ATLASSERT( iElement < m_nSize );
	return( m_pData[iElement] );
}

template< typename E, class ETraits >
CAtlArray< E, ETraits >::CAtlArray() :
	m_pData( NULL ),
	m_nSize( 0 ),
	m_nMaxSize( 0 ),
	m_nGrowBy( 0 )
{
}

template< typename E, class ETraits >
CAtlArray< E, ETraits >::~CAtlArray()
{
	if( m_pData != NULL )
	{
		CallDestructors( m_pData, m_nSize );
		free( m_pData );
	}
}

template< typename E, class ETraits >
bool CAtlArray< E, ETraits >::GrowBuffer( size_t nNewSize, int nGrowBy )
{
	if( nGrowBy != -1 )
	{
		m_nGrowBy = nGrowBy;  // set new size
	}

	if( nNewSize > m_nMaxSize )
	{
		if( m_pData == NULL )
		{
			m_pData = static_cast< E* >( malloc( nNewSize*sizeof( E ) ) );
			if( m_pData == NULL )
			{
				return( false );
			}
			m_nMaxSize = nNewSize;
		}
		else
		{
			// otherwise, grow array
			size_t nGrowBy = m_nGrowBy;
			if( nGrowBy == 0 )
			{
				// heuristically determine growth when nGrowBy == 0
				//  (this avoids heap fragmentation in many situations)
				nGrowBy = m_nSize/8;
				nGrowBy = (nGrowBy < 4) ? 4 : ((nGrowBy > 1024) ? 1024 : nGrowBy);
			}
			size_t nNewMax;
			if( nNewSize < (m_nMaxSize+nGrowBy) )
				nNewMax = m_nMaxSize+nGrowBy;  // granularity
			else
				nNewMax = nNewSize;  // no slush

			ATLASSERT( nNewMax >= m_nMaxSize );  // no wrap around
#ifdef SIZE_T_MAX
			ATLASSERT( nNewMax <= SIZE_T_MAX/sizeof( E ) ); // no overflow
#endif
			E* pNewData = static_cast< E* >( malloc( nNewMax*sizeof( E ) ) );
			if( pNewData == NULL )
			{
				return false;
			}

			// copy new data from old
			ETraits::RelocateElements( pNewData, m_pData, m_nSize );

			// get rid of old stuff (note: no destructors called)
			free( m_pData );
			m_pData = pNewData;
			m_nMaxSize = nNewMax;
		}
	}

	return true;
}	

template< typename E, class ETraits >
bool CAtlArray< E, ETraits >::SetSize( size_t nNewSize, int nGrowBy )
{
//	ASSERT_VALID(this);

	if( nNewSize == 0 )
	{
		// shrink to nothing
		if( m_pData != NULL )
		{
			CallDestructors( m_pData, m_nSize );
			free( m_pData );
			m_pData = NULL;
		}
		m_nSize = 0;
		m_nMaxSize = 0;
	}
	else if( nNewSize <= m_nMaxSize )
	{
		// it fits
		if( nNewSize > m_nSize )
		{
			// initialize the new elements
			CallConstructors( m_pData+m_nSize, nNewSize-m_nSize );
		}
		else if( m_nSize > nNewSize )
		{
			// destroy the old elements
			CallDestructors( m_pData+nNewSize, m_nSize-nNewSize );
		}
		m_nSize = nNewSize;
	}
	else
	{
		bool bSuccess;
		
		bSuccess = GrowBuffer( nNewSize, nGrowBy );
		if( !bSuccess )
		{
			return( false );
		}

		// construct new elements
		ATLASSERT( nNewSize > m_nSize );
		CallConstructors( m_pData+m_nSize, nNewSize-m_nSize );

		m_nSize = nNewSize;
	}

	return true;
}

template< typename E, class ETraits >
size_t CAtlArray< E, ETraits >::Append( const CAtlArray< E, ETraits >& aSrc )
{
//	ASSERT_VALID(this);
	ATLASSERT( this != &aSrc );   // cannot append to itself

	size_t nOldSize = m_nSize;
	SetSize( m_nSize+aSrc.m_nSize );
	ETraits::CopyElements( m_pData+nOldSize, aSrc.m_pData, aSrc.m_nSize );

	return( nOldSize );
}

template< typename E, class ETraits >
void CAtlArray< E, ETraits >::Copy( const CAtlArray< E, ETraits >& aSrc )
{
//	ASSERT_VALID(this);
	ATLASSERT( this != &aSrc );   // cannot append to itself

	SetSize( aSrc.m_nSize );
	ETraits::CopyElements( m_pData, aSrc.m_pData, aSrc.m_nSize );
}

template< typename E, class ETraits >
void CAtlArray< E, ETraits >::FreeExtra()
{
//	ASSERT_VALID(this);

	if( m_nSize != m_nMaxSize )
	{
		// shrink to desired size
#ifdef SIZE_T_MAX
		ATLASSERT( m_nSize <= (SIZE_T_MAX/sizeof( E )) ); // no overflow
#endif
		E* pNewData = NULL;
		if( m_nSize != 0 )
		{
			pNewData = (E*)malloc( m_nSize*sizeof( E ) );
			if( pNewData == NULL )
			{
				return;
			}

			// copy new data from old
			ETraits::RelocateElements( pNewData, m_pData, m_nSize );
		}

		// get rid of old stuff (note: no destructors called)
		free( m_pData );
		m_pData = pNewData;
		m_nMaxSize = m_nSize;
	}
}

template< typename E, class ETraits >
void CAtlArray< E, ETraits >::SetAtGrow( size_t iElement, INARGTYPE element )
{
//	ASSERT_VALID(this);
	size_t nOldSize;

	nOldSize = m_nSize;
	if( iElement >= m_nSize )
		SetSize( iElement+1, -1 );
	_ATLTRY
	{
		m_pData[iElement] = element;
	}
	_ATLCATCHALL()
	{
		if( m_nSize != nOldSize )
		{
			SetSize( nOldSize, -1 );
		}
		_ATLRETHROW;
	}
}

template< typename E, class ETraits >
void CAtlArray< E, ETraits >::InsertAt( size_t iElement, INARGTYPE element, size_t nElements /*=1*/)
{
//	ASSERT_VALID(this);
	ATLASSERT( nElements > 0 );     // zero size not allowed

	if( iElement >= m_nSize )
	{
		// adding after the end of the array
		SetSize( iElement+nElements, -1 );   // grow so nIndex is valid
	}
	else
	{
		// inserting in the middle of the array
		size_t nOldSize = m_nSize;
		SetSize( m_nSize+nElements, -1 );  // grow it to new size
		// destroy intial data before copying over it
		CallDestructors( m_pData+nOldSize, nElements );
		// shift old data up to fill gap
		ETraits::RelocateElements( m_pData+(iElement+nElements), m_pData+iElement,
			nOldSize-iElement );

		_ATLTRY
		{
			// re-init slots we copied from
			CallConstructors( m_pData+iElement, nElements );
		}
		_ATLCATCHALL()
		{
			ETraits::RelocateElements( m_pData+iElement, m_pData+(iElement+nElements),
				nOldSize-iElement );
			SetSize( nOldSize, -1 );
			_ATLRETHROW;
		}
	}

	// insert new value in the gap
	ATLASSERT( (iElement+nElements) <= m_nSize );
	for( size_t iNewElement = iElement; iNewElement < (iElement+nElements); iNewElement++ )
	{
		m_pData[iNewElement] = element;
	}
}

template< typename E, class ETraits >
void CAtlArray< E, ETraits >::RemoveAt( size_t iElement, size_t nElements )
{
//	ASSERT_VALID(this);
	ATLASSERT( (iElement+nElements) <= m_nSize );

	// just remove a range
	size_t nMoveCount = m_nSize-(iElement+nElements);
	CallDestructors( m_pData+iElement, nElements );
	if( nMoveCount > 0 )
	{
		ETraits::RelocateElements( m_pData+iElement, m_pData+(iElement+nElements),
			nMoveCount );
	}
	m_nSize -= nElements;
}

template< typename E, class ETraits >
void CAtlArray< E, ETraits >::InsertArrayAt( size_t iStartElement, 
	const CAtlArray< E, ETraits >* paNew )
{
//	ASSERT_VALID(this);
	ATLASSERT( paNew != NULL );
//	ASSERT_VALID(pNewArray);

	if( paNew->GetCount() > 0 )
	{
		InsertAt( iStartElement, paNew->GetAt( 0 ), paNew->GetCount() );
		for( size_t iElement = 0; iElement < paNew->GetCount(); iElement++ )
			SetAt( iStartElement+iElement, paNew->GetAt( iElement ) );
	}
}

#ifdef _DEBUG
template< typename E, class ETraits >
void CAtlArray< E, ETraits >::AssertValid() const
{
	if( m_pData == NULL )
	{
		ATLASSERT( m_nSize == 0 );
		ATLASSERT( m_nMaxSize == 0 );
	}
	else
	{
		ATLASSERT( m_nSize <= m_nMaxSize );
//		ASSERT( AfxIsValidAddress( m_pData, m_nMaxSize*sizeof( TYPE ) ) );
	}
}
#endif

#pragma push_macro("new")
#undef new

template< typename E, class ETraits >
void CAtlArray< E, ETraits >::CallConstructors( E* pElements, size_t nElements )
{
	size_t iElement;

	_ATLTRY
	{
		for( iElement = 0; iElement < nElements; iElement++ )
		{
			::new( pElements+iElement ) E;
		}
	}
	_ATLCATCHALL()
	{
		while( iElement > 0 )
		{
			iElement--;
			pElements[iElement].~E();
		}

		_ATLRETHROW;
	}
}

#pragma pop_macro("new")

template< typename E, class ETraits >
void CAtlArray< E, ETraits >::CallDestructors( E* pElements, size_t nElements )
{
	(void)pElements;  //REVIEW: Unreferenced formal warning if T doesn't have a real destructor

	for( size_t iElement = 0; iElement < nElements; iElement++ )
	{
		pElements[iElement].~E();
	}
}


template< typename E, class ETraits = CElementTraits< E > >
class CAtlList
{
public:
	typedef ETraits::INARGTYPE INARGTYPE;

private:
	class CNode :
		public __POSITION
	{
	public:
		CNode()
		{
		}
		CNode( INARGTYPE element ) :
			m_element( element )
		{
		}
		~CNode() throw()
		{
		}

	public:
		CNode* m_pNext;
		CNode* m_pPrev;
		E m_element;

	private:
		CNode( const CNode& ) throw();
	};

public:
	CAtlList( UINT nBlockSize = 10 ) throw();

	size_t GetCount() const throw();
	ATL_DEPRECATED size_t GetSize() const throw();
	bool IsEmpty() const throw();

	E& GetHead() throw();
	const E& GetHead() const throw();
	E& GetTail() throw();
	const E& GetTail() const throw();

	E RemoveHead();
	E RemoveTail();
	void RemoveHeadNoReturn() throw();
	void RemoveTailNoReturn() throw();

	POSITION AddHead();
	POSITION AddHead( INARGTYPE element );
	void AddHeadList( const CAtlList< E, ETraits >* plNew );
	POSITION AddTail();
	POSITION AddTail( INARGTYPE element );
	void AddTailList( const CAtlList< E, ETraits >* plNew );

	void RemoveAll() throw();

	POSITION GetHeadPosition() const throw();
	POSITION GetTailPosition() const throw();
	E& GetNext( POSITION& pos ) throw();
	const E& GetNext( POSITION& pos ) const throw();
	E& GetPrev( POSITION& pos ) throw();
	const E& GetPrev( POSITION& pos ) const throw();

	E& GetAt( POSITION pos ) throw();
	const E& GetAt( POSITION pos ) const throw();
	void SetAt( POSITION pos, INARGTYPE element );
	void RemoveAt( POSITION pos ) throw();

	POSITION InsertBefore( POSITION pos, INARGTYPE element );
	POSITION InsertAfter( POSITION pos, INARGTYPE element );

	POSITION Find( INARGTYPE element, POSITION posStartAfter = NULL ) const throw();
	POSITION FindIndex( size_t iElement ) const throw();

	void MoveToHead( POSITION pos ) throw();
	void MoveToTail( POSITION pos ) throw();
	void SwapElements( POSITION pos1, POSITION pos2 ) throw();

#ifdef _DEBUG
	void AssertValid() const;
#endif  // _DEBUG

// Implementation
private:
	CNode* m_pHead;
	CNode* m_pTail;
	size_t m_nElements;
	CAtlPlex* m_pBlocks;
	CNode* m_pFree;
	UINT m_nBlockSize;

private:
	void GetFreeNode();
	CNode* NewNode( CNode* pPrev, CNode* pNext );
	CNode* NewNode( INARGTYPE element, CNode* pPrev, CNode* pNext );
	void FreeNode( CNode* pNode ) throw();

public:
	~CAtlList() throw();
};

template< class I, const IID* piid = &__uuidof( I ) >
class CInterfaceList :
	public CAtlList< ATL::CComQIPtr< I, piid >, CComQIPtrElementTraits< I, piid > >
{
};

template< typename E >
class CAutoPtrList :
	public CAtlList< ATL::CAutoPtr< E >, CAutoPtrElementTraits< E > >
{
};

template< typename E, class Allocator = ATL::CCRTAllocator >
class CHeapPtrList :
	public CAtlList< ATL::CHeapPtr< E, Allocator >, CHeapPtrElementTraits< E, Allocator > >
{
};

template< typename E, class ETraits >
inline size_t CAtlList< E, ETraits >::GetCount() const
{
	return( m_nElements );
}

template< typename E, class ETraits >
inline size_t CAtlList< E, ETraits >::GetSize() const
{
	return( m_nElements );
}

template< typename E, class ETraits >
inline bool CAtlList< E, ETraits >::IsEmpty() const
{
	return( m_nElements == 0 );
}

template< typename E, class ETraits >
inline E& CAtlList< E, ETraits >::GetHead() 
{
	ATLASSERT( m_pHead != NULL );
	return( m_pHead->m_element );
}

template< typename E, class ETraits >
inline const E& CAtlList< E, ETraits >::GetHead() const
{
	ATLASSERT( m_pHead != NULL );
	return( m_pHead->m_element );
}

template< typename E, class ETraits >
inline E& CAtlList< E, ETraits >::GetTail() 
{
	ATLASSERT( m_pTail != NULL );
	return( m_pTail->m_element );
}

template< typename E, class ETraits >
inline const E& CAtlList< E, ETraits >::GetTail() const
{
	ATLASSERT( m_pTail != NULL );
	return( m_pTail->m_element );
}

template< typename E, class ETraits >
inline POSITION CAtlList< E, ETraits >::GetHeadPosition() const
{
	return( POSITION( m_pHead ) );
}

template< typename E, class ETraits >
inline POSITION CAtlList< E, ETraits >::GetTailPosition() const
{
	return( POSITION( m_pTail ) );
}

template< typename E, class ETraits >
inline E& CAtlList< E, ETraits >::GetNext( POSITION& pos )
{
	CNode* pNode;

	ATLASSERT( pos != NULL );
	pNode = (CNode*)pos;
	pos = POSITION( pNode->m_pNext );
	
	return( pNode->m_element );
}

template< typename E, class ETraits >
inline const E& CAtlList< E, ETraits >::GetNext( POSITION& pos ) const
{
	CNode* pNode;

	ATLASSERT( pos != NULL );
	pNode = (CNode*)pos;
	pos = POSITION( pNode->m_pNext );
	
	return( pNode->m_element );
}

template< typename E, class ETraits >
inline E& CAtlList< E, ETraits >::GetPrev( POSITION& pos )
{
	CNode* pNode;

	ATLASSERT( pos != NULL );
	pNode = (CNode*)pos;
	pos = POSITION( pNode->m_pPrev );
	
	return( pNode->m_element );
}

template< typename E, class ETraits >
inline const E& CAtlList< E, ETraits >::GetPrev( POSITION& pos ) const
{
	CNode* pNode;

	ATLASSERT( pos != NULL );
	pNode = (CNode*)pos;
	pos = POSITION( pNode->m_pPrev );
	
	return( pNode->m_element );
}

template< typename E, class ETraits >
inline E& CAtlList< E, ETraits >::GetAt( POSITION pos )
{
	CNode* pNode;

	ATLASSERT( pos != NULL );
	pNode = (CNode*)pos;
	
	return( pNode->m_element );
}

template< typename E, class ETraits >
inline const E& CAtlList< E, ETraits >::GetAt( POSITION pos ) const
{
	CNode* pNode;

	ATLASSERT( pos != NULL );
	pNode = (CNode*)pos;
	
	return( pNode->m_element );
}

template< typename E, class ETraits >
inline void CAtlList< E, ETraits >::SetAt( POSITION pos, INARGTYPE element )
{
	CNode* pNode;

	ATLASSERT( pos != NULL );
	pNode = (CNode*)pos;
	pNode->m_element = element;
}

template< typename E, class ETraits >
CAtlList< E, ETraits >::CAtlList( UINT nBlockSize ) :
	m_nElements( 0 ),
	m_pHead( NULL ),
	m_pTail( NULL ),
	m_nBlockSize( nBlockSize ),
	m_pBlocks( NULL ),
	m_pFree( NULL )
{
	ATLASSERT( nBlockSize > 0 );
}

template< typename E, class ETraits >
void CAtlList< E, ETraits >::RemoveAll()
{
	while( m_nElements > 0 )
	{
		CNode* pKill;

		pKill = m_pHead;
		ATLASSERT( pKill != NULL );
		m_pHead = m_pHead->m_pNext;
		FreeNode( pKill );
	}
	ATLASSERT( m_nElements == 0 );
	m_pHead = NULL;
	m_pTail = NULL;
	m_pFree = NULL;
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;
}

template< typename E, class ETraits >
CAtlList< E, ETraits >::~CAtlList()
{
	RemoveAll();
	ATLASSERT( m_nElements == 0 );
}

#pragma push_macro("new")
#undef new

template< typename E, class ETraits >
void CAtlList< E, ETraits >::GetFreeNode()
{
	if( m_pFree == NULL )
	{
		CAtlPlex* pPlex;
		CNode* pNode;

		pPlex = CAtlPlex::Create( m_pBlocks, m_nBlockSize, sizeof( CNode ) );
		if( pPlex == NULL )
		{
			ATL::AtlThrow( E_OUTOFMEMORY );
		}
		pNode = (CNode*)pPlex->data();
		pNode += m_nBlockSize-1;
		for( int iBlock = m_nBlockSize-1; iBlock >= 0; iBlock-- )
		{
			pNode->m_pNext = m_pFree;
			m_pFree = pNode;
			pNode--;
		}
	}
	ATLASSERT( m_pFree != NULL );
}

template< typename E, class ETraits >
CAtlList< E, ETraits >::CNode* CAtlList< E, ETraits >::NewNode( CNode* pPrev, CNode* pNext )
{
	GetFreeNode();

	CNode* pNewNode = m_pFree;
	CNode* pNextFree = m_pFree->m_pNext;

	::new( pNewNode ) CNode;

	m_pFree = pNextFree;
	pNewNode->m_pPrev = pPrev;
	pNewNode->m_pNext = pNext;
	m_nElements++;
	ATLASSERT( m_nElements > 0 );

	return( pNewNode );
}

template< typename E, class ETraits >
CAtlList< E, ETraits >::CNode* CAtlList< E, ETraits >::NewNode( INARGTYPE element, CNode* pPrev, 
	CNode* pNext )
{
	GetFreeNode();

	CNode* pNewNode = m_pFree;
	CNode* pNextFree = m_pFree->m_pNext;

	::new( pNewNode ) CNode( element );

	m_pFree = pNextFree;
	pNewNode->m_pPrev = pPrev;
	pNewNode->m_pNext = pNext;
	m_nElements++;
	ATLASSERT( m_nElements > 0 );

	return( pNewNode );
}

#pragma pop_macro("new")

template< typename E, class ETraits >
void CAtlList< E, ETraits >::FreeNode( CNode* pNode )
{
	pNode->~CNode();
	pNode->m_pNext = m_pFree;
	m_pFree = pNode;
	ATLASSERT( m_nElements > 0 );
	m_nElements--;
	if( m_nElements == 0 )
	{
		RemoveAll();
	}
}

template< typename E, class ETraits >
POSITION CAtlList< E, ETraits >::AddHead()
{
	CNode* pNode = NewNode( NULL, m_pHead );
	if( m_pHead != NULL )
	{
		m_pHead->m_pPrev = pNode;
	}
	else
	{
		m_pTail = pNode;
	}
	m_pHead = pNode;

	return( POSITION( pNode ) );
}

template< typename E, class ETraits >
POSITION CAtlList< E, ETraits >::AddHead( INARGTYPE element )
{
	CNode* pNode;

	pNode = NewNode( element, NULL, m_pHead );
	
	if( m_pHead != NULL )
	{
		m_pHead->m_pPrev = pNode;
	}
	else
	{
		m_pTail = pNode;
	}
	m_pHead = pNode;

	return( POSITION( pNode ) );
}

template< typename E, class ETraits >
POSITION CAtlList< E, ETraits >::AddTail()
{
	CNode* pNode = NewNode( m_pTail, NULL );
	if( m_pTail != NULL )
	{
		m_pTail->m_pNext = pNode;
	}
	else
	{
		m_pHead = pNode;
	}
	m_pTail = pNode;

	return( POSITION( pNode ) );
}

template< typename E, class ETraits >
POSITION CAtlList< E, ETraits >::AddTail( INARGTYPE element )
{
	CNode* pNode;

	pNode = NewNode( element, m_pTail, NULL );
	
	if( m_pTail != NULL )
	{
		m_pTail->m_pNext = pNode;
	}
	else
	{
		m_pHead = pNode;
	}
	m_pTail = pNode;

	return( POSITION( pNode ) );
}

template< typename E, class ETraits >
void CAtlList< E, ETraits >::AddHeadList( const CAtlList< E, ETraits >* plNew )
{
	POSITION pos;

	ATLASSERT( plNew != NULL );

	pos = plNew->GetTailPosition();
	while( pos != NULL )
	{
		INARGTYPE element = plNew->GetPrev( pos );
		AddHead( element );
	}
}

template< typename E, class ETraits >
void CAtlList< E, ETraits >::AddTailList( const CAtlList< E, ETraits >* plNew )
{
	POSITION pos;

	ATLASSERT( plNew != NULL );

	pos = plNew->GetHeadPosition();
	while( pos != NULL )
	{
		INARGTYPE element = plNew->GetNext( pos );
		AddTail( element );
	}
}

template< typename E, class ETraits >
E CAtlList< E, ETraits >::RemoveHead()
{
	CNode* pNode;

	ATLASSERT( m_pHead != NULL );

	pNode = m_pHead;
	E element( pNode->m_element );

	m_pHead = pNode->m_pNext;
	if( m_pHead != NULL )
	{
		m_pHead->m_pPrev = NULL;
	}
	else
	{
		m_pTail = NULL;
	}
	FreeNode( pNode );

	return( element );
}

template< typename E, class ETraits >
void CAtlList< E, ETraits >::RemoveHeadNoReturn() throw()
{
	ATLASSERT( m_pHead != NULL );

	CNode* pNode = m_pHead;
	m_pHead = pNode->m_pNext;
	if( m_pHead != NULL )
	{
		m_pHead->m_pPrev = NULL;
	}
	else
	{
		m_pTail = NULL;
	}
	FreeNode( pNode );
}

template< typename E, class ETraits >
E CAtlList< E, ETraits >::RemoveTail()
{
	CNode* pNode;

	ATLASSERT( m_pTail != NULL );

	pNode = m_pTail;
	E element( pNode->m_element );

	m_pTail = pNode->m_pPrev;
	if( m_pTail != NULL )
	{
		m_pTail->m_pNext = NULL;
	}
	else
	{
		m_pHead = NULL;
	}
	FreeNode( pNode );

	return( element );
}

template< typename E, class ETraits >
void CAtlList< E, ETraits >::RemoveTailNoReturn() throw()
{
	ATLASSERT( m_pTail != NULL );

	CNode* pNode = m_pTail;

	m_pTail = pNode->m_pPrev;
	if( m_pTail != NULL )
	{
		m_pTail->m_pNext = NULL;
	}
	else
	{
		m_pHead = NULL;
	}
	FreeNode( pNode );
}

template< typename E, class ETraits >
POSITION CAtlList< E, ETraits >::InsertBefore( POSITION pos, INARGTYPE element )
{
//	ASSERT_VALID(this);

	if( pos == NULL )
		return AddHead( element ); // insert before nothing -> head of the list

	// Insert it before position
	CNode* pOldNode = (CNode*)pos;
	CNode* pNewNode = NewNode( element, pOldNode->m_pPrev, pOldNode );

	if( pOldNode->m_pPrev != NULL )
	{
//		ASSERT(AfxIsValidAddress(pOldNode->pPrev, sizeof(CNode)));
		pOldNode->m_pPrev->m_pNext = pNewNode;
	}
	else
	{
		ATLASSERT( pOldNode == m_pHead );
		m_pHead = pNewNode;
	}
	pOldNode->m_pPrev = pNewNode;

	return( POSITION( pNewNode ) );
}

template< typename E, class ETraits >
POSITION CAtlList< E, ETraits >::InsertAfter( POSITION pos, INARGTYPE element )
{
//	ASSERT_VALID(this);

	if( pos == NULL )
		return AddTail( element ); // insert after nothing -> tail of the list

	// Insert it after position
	CNode* pOldNode = (CNode*)pos;
	CNode* pNewNode = NewNode( element, pOldNode, pOldNode->m_pNext );

	if( pOldNode->m_pNext != NULL )
	{
//		ASSERT(AfxIsValidAddress(pOldNode->pNext, sizeof(CNode)));
		pOldNode->m_pNext->m_pPrev = pNewNode;
	}
	else
	{
		ATLASSERT( pOldNode == m_pTail );
		m_pTail = pNewNode;
	}
	pOldNode->m_pNext = pNewNode;

	return( POSITION( pNewNode ) );
}

template< typename E, class ETraits >
void CAtlList< E, ETraits >::RemoveAt( POSITION pos )
{
//	ASSERT_VALID(this);

	CNode* pOldNode = (CNode*)pos;
//	ASSERT(AfxIsValidAddress(pOldNode, sizeof(CNode)));

	// remove pOldNode from list
	if( pOldNode == m_pHead )
	{
		m_pHead = pOldNode->m_pNext;
	}
	else
	{
//		ASSERT(AfxIsValidAddress(pOldNode->pPrev, sizeof(CNode)));
		pOldNode->m_pPrev->m_pNext = pOldNode->m_pNext;
	}
	if( pOldNode == m_pTail )
	{
		m_pTail = pOldNode->m_pPrev;
	}
	else
	{
//		ASSERT(AfxIsValidAddress(pOldNode->pNext, sizeof(CNode)));
		pOldNode->m_pNext->m_pPrev = pOldNode->m_pPrev;
	}
	FreeNode( pOldNode );
}

template< typename E, class ETraits >
POSITION CAtlList< E, ETraits >::FindIndex( size_t iElement ) const
{
//	ASSERT_VALID(this);

	if( iElement >= m_nElements )
		return NULL;  // went too far

	CNode* pNode = m_pHead;
	for( size_t iSearch = 0; iSearch < iElement; iSearch++ )
	{
		pNode = pNode->m_pNext;
	}

	return( POSITION( pNode ) );
}

template< typename E, class ETraits >
void CAtlList< E, ETraits >::MoveToHead( POSITION pos ) throw()
{
	ATLASSERT( pos != NULL );

	CNode* pNode = static_cast< CNode* >( pos );
	if( pNode == m_pHead )
	{
		// Already at the head
		return;
	}

	if( pNode->m_pNext == NULL )
	{
		ATLASSERT( pNode == m_pTail );
		m_pTail = pNode->m_pPrev;
	}
	else
	{
		pNode->m_pNext->m_pPrev = pNode->m_pPrev;
	}
	ATLASSERT( pNode->m_pPrev != NULL );  // This node can't be the head, since we already checked that case
	pNode->m_pPrev->m_pNext = pNode->m_pNext;

	m_pHead->m_pPrev = pNode;
	pNode->m_pNext = m_pHead;
	pNode->m_pPrev = NULL;
	m_pHead = pNode;
}

template< typename E, class ETraits >
void CAtlList< E, ETraits >::MoveToTail( POSITION pos ) throw()
{
	ATLASSERT( pos != NULL );

	CNode* pNode = static_cast< CNode* >( pos );
	if( pNode == m_pTail )
	{
		// Already at the tail
		return;
	}

	if( pNode->m_pPrev == NULL )
	{
		ATLASSERT( pNode == m_pHead );
		m_pHead = pNode->m_pNext;
	}
	else
	{
		pNode->m_pPrev->m_pNext = pNode->m_pNext;
	}
	ATLASSERT( pNode->m_pNext != NULL );  // This node can't be the tail, since we already checked that case
	pNode->m_pNext->m_pPrev = pNode->m_pPrev;

	m_pTail->m_pNext = pNode;
	pNode->m_pPrev = m_pTail;
	pNode->m_pNext = NULL;
	m_pTail = pNode;
}

template< typename E, class ETraits >
void CAtlList< E, ETraits >::SwapElements( POSITION pos1, POSITION pos2 ) throw()
{
	ATLASSERT( pos1 != NULL );
	ATLASSERT( pos2 != NULL );

	if( pos1 == pos2 )
	{
		// Nothing to do
		return;
	}

	CNode* pNode1 = static_cast< CNode* >( pos1 );
	CNode* pNode2 = static_cast< CNode* >( pos2 );
	if( pNode2->m_pNext == pNode1 )
	{
		// Swap pNode2 and pNode1 so that the next case works
		CNode* pNodeTemp = pNode1;
		pNode1 = pNode2;
		pNode2 = pNodeTemp;
	}
	if( pNode1->m_pNext == pNode2 )
	{
		// Node1 and Node2 are adjacent
		pNode2->m_pPrev = pNode1->m_pPrev;
		if( pNode1->m_pPrev != NULL )
		{
			pNode1->m_pPrev->m_pNext = pNode2;
		}
		else
		{
			ATLASSERT( m_pHead == pNode1 );
			m_pHead = pNode2;
		}
		pNode1->m_pNext = pNode2->m_pNext;
		if( pNode2->m_pNext != NULL )
		{
			pNode2->m_pNext->m_pPrev = pNode1;
		}
		else
		{
			ATLASSERT( m_pTail == pNode2 );
			m_pTail = pNode1;
		}
		pNode2->m_pNext = pNode1;
		pNode1->m_pPrev = pNode2;
	}
	else
	{
		// The two nodes are not adjacent
		CNode* pNodeTemp;

		pNodeTemp = pNode1->m_pPrev;
		pNode1->m_pPrev = pNode2->m_pPrev;
		pNode2->m_pPrev = pNodeTemp;

		pNodeTemp = pNode1->m_pNext;
		pNode1->m_pNext = pNode2->m_pNext;
		pNode2->m_pNext = pNodeTemp;

		if( pNode1->m_pNext != NULL )
		{
			pNode1->m_pNext->m_pPrev = pNode1;
		}
		else
		{
			ATLASSERT( m_pTail == pNode2 );
			m_pTail = pNode1;
		}
		if( pNode1->m_pPrev != NULL )
		{
			pNode1->m_pPrev->m_pNext = pNode1;
		}
		else
		{
			ATLASSERT( m_pHead == pNode2 );
			m_pHead = pNode1;
		}
		if( pNode2->m_pNext != NULL )
		{
			pNode2->m_pNext->m_pPrev = pNode2;
		}
		else
		{
			ATLASSERT( m_pTail == pNode1 );
			m_pTail = pNode2;
		}
		if( pNode2->m_pPrev != NULL )
		{
			pNode2->m_pPrev->m_pNext = pNode2;
		}
		else
		{
			ATLASSERT( m_pHead == pNode1 );
			m_pHead = pNode2;
		}
	}
}

template< typename E, class ETraits >
POSITION CAtlList< E, ETraits >::Find( INARGTYPE element, POSITION posStartAfter ) const
{
//	ASSERT_VALID(this);

	CNode* pNode = (CNode*)posStartAfter;
	if( pNode == NULL )
	{
		pNode = m_pHead;  // start at head
	}
	else
	{
//		ASSERT(AfxIsValidAddress(pNode, sizeof(CNode)));
		pNode = pNode->m_pNext;  // start after the one specified
	}

	for( ; pNode != NULL; pNode = pNode->m_pNext )
	{
		if( ETraits::CompareElements( pNode->m_element, element ) )
			return( POSITION( pNode ) );
	}

	return( NULL );
}

#ifdef _DEBUG
template< typename E, class ETraits >
void CAtlList< E, ETraits >::AssertValid() const
{
	if( IsEmpty() )
	{
		// empty list
		ATLASSERT(m_pHead == NULL);
		ATLASSERT(m_pTail == NULL);
	}
	else
	{
		// non-empty list
//		ASSERT(AfxIsValidAddress(m_pNodeHead, sizeof(CNode)));
//		ASSERT(AfxIsValidAddress(m_pNodeTail, sizeof(CNode)));
	}
}
#endif

template< typename K, typename V, class KTraits = CElementTraits< K >, class VTraits = CElementTraits< V > >
class CAtlMap
{
public:
	typedef KTraits::INARGTYPE KINARGTYPE;
	typedef KTraits::OUTARGTYPE KOUTARGTYPE;
	typedef VTraits::INARGTYPE VINARGTYPE;
	typedef VTraits::OUTARGTYPE VOUTARGTYPE;

	class CPair :
		public __POSITION
	{
	protected:
		CPair( KINARGTYPE key ) :
			m_key( key )
		{
		}

	public:
		const K m_key;
		V m_value;
	};

private:
	class CNode :
		public CPair
	{
	public:
		CNode( KINARGTYPE key, UINT nHash ) :
			CPair( key ),
			m_nHash( nHash )
		{
		}

	public:
		UINT GetHash() const throw()
		{
			return( m_nHash );
		}

	public:
		CNode* m_pNext;
		UINT m_nHash;
	};

public:
	CAtlMap( UINT nBins = 17, float fOptimalLoad = 0.75f, 
		float fLoThreshold = 0.25f, float fHiThreshold = 2.25f, UINT nBlockSize = 10 ) throw();

	size_t GetCount() const throw();
	ATL_DEPRECATED size_t GetSize() const throw();
	bool IsEmpty() const throw();

	bool Lookup( KINARGTYPE key, VOUTARGTYPE value ) const;
	const CPair* Lookup( KINARGTYPE key ) const throw();
	CPair* Lookup( KINARGTYPE key ) throw();
	V& operator[]( KINARGTYPE key ) throw();

	POSITION SetAt( KINARGTYPE key, VINARGTYPE value );
	void SetAtPos( POSITION pos, VINARGTYPE value );

	bool RemoveKey( KINARGTYPE key ) throw();
	void RemoveAll() throw();
	void RemoveAtPos( POSITION pos ) throw();

	POSITION GetStartPosition() const throw();
	void GetNextAssoc( POSITION& pos, KOUTARGTYPE key, VOUTARGTYPE value ) const;
	const CPair* GetNext( POSITION& pos ) const throw();
	CPair* GetNext( POSITION& pos ) throw();
	const K& GetNextKey( POSITION& pos ) const throw();
	const V& GetNextValue( POSITION& pos ) const throw();
	V& GetNextValue( POSITION& pos ) throw();
	void GetAtPos( POSITION pos, KOUTARGTYPE key, VOUTARGTYPE value ) const;
	const K& GetKeyAt( POSITION pos ) const throw();
	const V& GetValueAt( POSITION pos ) const throw();
	V& GetValueAt( POSITION pos ) throw();

	UINT GetHashTableSize() const throw();
	bool InitHashTable( UINT nBins, bool bAllocNow = true );
	//REVIEW: replaced by DisableAutoRehash
	void Lock() throw();
	//REVIEW: replaced by EnableAutoRehash
	void Unlock() throw();
	void EnableAutoRehash() throw();
	void DisableAutoRehash() throw();
	void Rehash( UINT nBins = 0 );
	void SetOptimalLoad( float fOptimalLoad, float fLoThreshold, float fHiThreshold, 
		bool bRehashNow = false );

#ifdef _DEBUG
	void AssertValid() const;
#endif  // _DEBUG

// Implementation
private:
	CNode** m_ppBins;
	size_t m_nElements;
	UINT m_nBins;
	float m_fOptimalLoad;
	float m_fLoThreshold;
	float m_fHiThreshold;
	size_t m_nHiRehashThreshold;
	size_t m_nLoRehashThreshold;
	ULONG m_nLockCount;
	UINT m_nBlockSize;
	CAtlPlex* m_pBlocks;
	CNode* m_pFree;

private:
	bool IsLocked() const throw();
	UINT PickSize( size_t nElements ) const throw();
	CNode* NewNode( KINARGTYPE key, UINT iBin, UINT nHash );
	void FreeNode( CNode* pNode ) throw();
	void FreePlexes() throw();
	CNode* GetNode( KINARGTYPE key, UINT& iBin, UINT& nHash, CNode*& pPrev ) const throw();
	CNode* CreateNode( KINARGTYPE key, UINT iBin, UINT nHash );
	void RemoveNode( CNode* pNode, CNode* pPrev ) throw();
	CNode* FindNextNode( CNode* pNode ) const throw();
	void UpdateRehashThresholds() throw();

public:
	~CAtlMap() throw();
};

template< typename K, typename I, class KTraits = CElementTraits< K > >
class CMapToInterface :
	public CAtlMap< K, ATL::CComQIPtr< I >, KTraits, CComQIPtrElementTraits< I > >
{
public:
	CMapToInterface( UINT nBins = 17 ) throw();
};

template< typename K, typename I, class KTraits >
inline CMapToInterface< K, I, KTraits >::CMapToInterface( UINT nBins ) :
	CAtlMap< K, ATL::CComQIPtr< I >, KTraits, CComQIPtrElementTraits< I > >( nBins )
{
}

template< typename K, typename V, class KTraits = CElementTraits< K > >
class CMapToAutoPtr :
	public CAtlMap< K, ATL::CAutoPtr< V >, KTraits, CAutoPtrElementTraits< V > >
{
public:
	CMapToAutoPtr( UINT nBins = 17 ) throw();
};

template< typename K, typename V, class KTraits >
inline CMapToAutoPtr< K, V, KTraits >::CMapToAutoPtr( UINT nBins ) :
	CAtlMap< K, ATL::CAutoPtr< V >, KTraits, CAutoPtrElementTraits< V > >( nBins )
{
}

template< typename K, typename V, class KTraits, class VTraits >
inline size_t CAtlMap< K, V, KTraits, VTraits >::GetCount() const
{
	return( m_nElements );
}

template< typename K, typename V, class KTraits, class VTraits >
inline size_t CAtlMap< K, V, KTraits, VTraits >::GetSize() const
{
	return( m_nElements );
}

template< typename K, typename V, class KTraits, class VTraits >
inline bool CAtlMap< K, V, KTraits, VTraits >::IsEmpty() const
{
	return( m_nElements == 0 );
}

template< typename K, typename V, class KTraits, class VTraits >
inline V& CAtlMap< K, V, KTraits, VTraits >::operator[]( KINARGTYPE key )
{
	CNode* pNode;
	UINT iBin;
	UINT nHash;
	CNode* pPrev;

	pNode = GetNode( key, iBin, nHash, pPrev );
	if( pNode == NULL )
	{
		pNode = CreateNode( key, iBin, nHash );
	}

	return( pNode->m_value );
}

template< typename K, typename V, class KTraits, class VTraits >
inline UINT CAtlMap< K, V, KTraits, VTraits >::GetHashTableSize() const
{
	return( m_nBins );
}

template< typename K, typename V, class KTraits, class VTraits >
inline void CAtlMap< K, V, KTraits, VTraits >::GetAtPos( POSITION pos, KOUTARGTYPE key, VOUTARGTYPE value ) const
{
	CNode* pNode;

	ATLASSERT( pos != NULL );

	pNode = static_cast< CNode* >( pos );
	key = pNode->m_key;
	value = pNode->m_value;
}

template< typename K, typename V, class KTraits, class VTraits >
inline const K& CAtlMap< K, V, KTraits, VTraits >::GetKeyAt( POSITION pos ) const
{
	CNode* pNode;

	ATLASSERT( pos != NULL );
	
	pNode = (CNode*)pos;

	return( pNode->m_key );
}

template< typename K, typename V, class KTraits, class VTraits >
inline const V& CAtlMap< K, V, KTraits, VTraits >::GetValueAt( POSITION pos ) const
{
	CNode* pNode;

	ATLASSERT( pos != NULL );
	
	pNode = (CNode*)pos;

	return( pNode->m_value );
}

template< typename K, typename V, class KTraits, class VTraits >
inline V& CAtlMap< K, V, KTraits, VTraits >::GetValueAt( POSITION pos )
{
	CNode* pNode;

	ATLASSERT( pos != NULL );
	
	pNode = (CNode*)pos;

	return( pNode->m_value );
}

template< typename K, typename V, class KTraits, class VTraits >
inline void CAtlMap< K, V, KTraits, VTraits >::Lock()
{
	m_nLockCount++;
}

template< typename K, typename V, class KTraits, class VTraits >
inline void CAtlMap< K, V, KTraits, VTraits >::Unlock()
{
	ATLASSERT( m_nLockCount > 0 );
	m_nLockCount--;
}

template< typename K, typename V, class KTraits, class VTraits >
inline void CAtlMap< K, V, KTraits, VTraits >::DisableAutoRehash() throw()
{
	m_nLockCount++;
}

template< typename K, typename V, class KTraits, class VTraits >
inline void CAtlMap< K, V, KTraits, VTraits >::EnableAutoRehash() throw()
{
	ATLASSERT( m_nLockCount > 0 );
	m_nLockCount--;
}

template< typename K, typename V, class KTraits, class VTraits >
inline bool CAtlMap< K, V, KTraits, VTraits >::IsLocked() const
{
	return( m_nLockCount != 0 );
}

template< typename K, typename V, class KTraits, class VTraits >
UINT CAtlMap< K, V, KTraits, VTraits >::PickSize( size_t nElements ) const
{
	// List of primes such that s_anPrimes[i] is the smallest prime greater than 2^(5+i/3)
	static const UINT s_anPrimes[] =
	{
		17, 23, 29, 37, 41, 53, 67, 83, 103, 131, 163, 211, 257, 331, 409, 521, 647, 821, 
		1031, 1291, 1627, 2053, 2591, 3251, 4099, 5167, 6521, 8209, 10331, 
		13007, 16411, 20663, 26017, 32771, 41299, 52021, 65537, 82571, 104033, 
		131101, 165161, 208067, 262147, 330287, 416147, 524309, 660563, 
		832291, 1048583, 1321139, 1664543, 2097169, 2642257, 3329023, 4194319, 
		5284493, 6658049, 8388617, 10568993, 13316089, UINT_MAX
	};

	UINT nBinsEstimate = min( UINT_MAX, (size_t)(nElements/m_fOptimalLoad) );

	// Find the smallest prime greater than our estimate
	int iPrime = 0;
	while( nBinsEstimate > s_anPrimes[iPrime] )
	{
		iPrime++;
	}

	if( s_anPrimes[iPrime] == UINT_MAX )
	{
		return( nBinsEstimate );
	}
	else
	{
		return( s_anPrimes[iPrime] );
	}
}

template< typename K, typename V, class KTraits, class VTraits >
CAtlMap< K, V, KTraits, VTraits >::CNode* CAtlMap< K, V, KTraits, VTraits >::CreateNode( 
	KINARGTYPE key, UINT iBin, UINT nHash )
{
	CNode* pNode;

	if( m_ppBins == NULL )
	{
		bool bSuccess;

		bSuccess = InitHashTable( m_nBins );
		if( !bSuccess )
		{
			ATL::AtlThrow( E_OUTOFMEMORY );
		}
	}

	pNode = NewNode( key, iBin, nHash );

	return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
POSITION CAtlMap< K, V, KTraits, VTraits >::GetStartPosition() const
{
	if( IsEmpty() )
	{
		return( NULL );
	}

	for( UINT iBin = 0; iBin < m_nBins; iBin++ )
	{
		if( m_ppBins[iBin] != NULL )
		{
			return( POSITION( m_ppBins[iBin] ) );
		}
	}
	ATLASSERT( false );

	return( NULL );
}

template< typename K, typename V, class KTraits, class VTraits >
POSITION CAtlMap< K, V, KTraits, VTraits >::SetAt( KINARGTYPE key, VINARGTYPE value )
{
	CNode* pNode;
	UINT iBin;
	UINT nHash;
	CNode* pPrev;

	pNode = GetNode( key, iBin, nHash, pPrev );
	if( pNode == NULL )
	{
		pNode = CreateNode( key, iBin, nHash );
		_ATLTRY
		{
			pNode->m_value = value;
		}
		_ATLCATCHALL()
		{
			RemoveAtPos( POSITION( pNode ) );
			_ATLRETHROW;
		}
	}
	else
	{
		pNode->m_value = value;
	}

	return( POSITION( pNode ) );
}

template< typename K, typename V, class KTraits, class VTraits >
void CAtlMap< K, V, KTraits, VTraits >::SetAtPos( POSITION pos, VINARGTYPE value )
{
	CNode* pNode;

	ATLASSERT( pos != NULL );

	pNode = static_cast< CNode* >( pos );
	pNode->m_value = value;
}

template< typename K, typename V, class KTraits, class VTraits >
CAtlMap< K, V, KTraits, VTraits >::CAtlMap( UINT nBins, float fOptimalLoad, 
	float fLoThreshold, float fHiThreshold, UINT nBlockSize ) :
	m_ppBins( NULL ),
	m_nBins( nBins ),
	m_nElements( 0 ),
	m_nLockCount( 0 ),  // Start unlocked
	m_fOptimalLoad( fOptimalLoad ),
	m_fLoThreshold( fLoThreshold ),
	m_fHiThreshold( fHiThreshold ),
	m_nHiRehashThreshold( UINT_MAX ),
	m_nLoRehashThreshold( 0 ),
	m_pBlocks( NULL ),
	m_pFree( NULL ),
	m_nBlockSize( nBlockSize )
{
	ATLASSERT( nBins > 0 );
	ATLASSERT( nBlockSize > 0 );

	SetOptimalLoad( fOptimalLoad, fLoThreshold, fHiThreshold, false );
}

template< typename K, typename V, class KTraits, class VTraits >
void CAtlMap< K, V, KTraits, VTraits >::SetOptimalLoad( float fOptimalLoad, float fLoThreshold,
	float fHiThreshold, bool bRehashNow )
{
	ATLASSERT( fOptimalLoad > 0 );
	ATLASSERT( (fLoThreshold >= 0) && (fLoThreshold < fOptimalLoad) );
	ATLASSERT( fHiThreshold > fOptimalLoad );

	m_fOptimalLoad = fOptimalLoad;
	m_fLoThreshold = fLoThreshold;
	m_fHiThreshold = fHiThreshold;

	UpdateRehashThresholds();

	if( bRehashNow && ((m_nElements > m_nHiRehashThreshold) || 
		(m_nElements < m_nLoRehashThreshold)) )
	{
		Rehash( PickSize( m_nElements ) );
	}
}

template< typename K, typename V, class KTraits, class VTraits >
void CAtlMap< K, V, KTraits, VTraits >::UpdateRehashThresholds() throw()
{
	m_nHiRehashThreshold = size_t( m_fHiThreshold*m_nBins );
	m_nLoRehashThreshold = size_t( m_fLoThreshold*m_nBins );
	if( m_nLoRehashThreshold < 17 )
	{
		m_nLoRehashThreshold = 0;
	}
}

template< typename K, typename V, class KTraits, class VTraits >
bool CAtlMap< K, V, KTraits, VTraits >::InitHashTable( UINT nBins, bool bAllocNow )
{
	ATLASSERT( m_nElements == 0 );
	ATLASSERT( nBins > 0 );

	if( m_ppBins != NULL )
	{
		delete[] m_ppBins;
		m_ppBins = NULL;
	}

	if( bAllocNow )
	{
		ATLTRY( m_ppBins = new CNode*[nBins] );
		if( m_ppBins == NULL )
		{
			return false;
		}
		memset( m_ppBins, 0, sizeof( CNode* )*nBins );
	}
	m_nBins = nBins;

	UpdateRehashThresholds();

	return true;
}

template< typename K, typename V, class KTraits, class VTraits >
void CAtlMap< K, V, KTraits, VTraits >::RemoveAll()
{
	DisableAutoRehash();
	if( m_ppBins != NULL )
	{
		for( UINT iBin = 0; iBin < m_nBins; iBin++ )
		{
			CNode* pNext;

			pNext = m_ppBins[iBin];
			while( pNext != NULL )
			{
				CNode* pKill;

				pKill = pNext;
				pNext = pNext->m_pNext;
				FreeNode( pKill );
			}
		}
	}

	delete[] m_ppBins;
	m_ppBins = NULL;
	m_nElements = 0;

	if( !IsLocked() )
	{
		InitHashTable( PickSize( m_nElements ), false );
	}

	FreePlexes();
	EnableAutoRehash();
}

template< typename K, typename V, class KTraits, class VTraits >
CAtlMap< K, V, KTraits, VTraits >::~CAtlMap()
{
	RemoveAll();
}

#pragma push_macro("new")
#undef new

template< typename K, typename V, class KTraits, class VTraits >
CAtlMap< K, V, KTraits, VTraits >::CNode* CAtlMap< K, V, KTraits, VTraits >::NewNode( 
	KINARGTYPE key, UINT iBin, UINT nHash )
{
	CNode* pNewNode;

	if( m_pFree == NULL )
	{
		CAtlPlex* pPlex;
		CNode* pNode;

		pPlex = CAtlPlex::Create( m_pBlocks, m_nBlockSize, sizeof( CNode ) );
		if( pPlex == NULL )
		{
			ATL::AtlThrow( E_OUTOFMEMORY );
		}
		pNode = (CNode*)pPlex->data();
		pNode += m_nBlockSize-1;
		for( int iBlock = m_nBlockSize-1; iBlock >= 0; iBlock-- )
		{
			pNode->m_pNext = m_pFree;
			m_pFree = pNode;
			pNode--;
		}
	}
	ATLASSERT( m_pFree != NULL );
	pNewNode = m_pFree;
	m_pFree = pNewNode->m_pNext;

	_ATLTRY
	{
		::new( pNewNode ) CNode( key, nHash );
	}
	_ATLCATCHALL()
	{
		pNewNode->m_pNext = m_pFree;
		m_pFree = pNewNode;

		_ATLRETHROW;
	}
	m_nElements++;

	pNewNode->m_pNext = m_ppBins[iBin];
	m_ppBins[iBin] = pNewNode;

	if( (m_nElements > m_nHiRehashThreshold) && !IsLocked() )
	{
		Rehash( PickSize( m_nElements ) );
	}

	return( pNewNode );
}

#pragma pop_macro("new")

template< typename K, typename V, class KTraits, class VTraits >
void CAtlMap< K, V, KTraits, VTraits >::FreeNode( CNode* pNode )
{
	ATLASSERT( pNode != NULL );

	pNode->~CNode();
	pNode->m_pNext = m_pFree;
	m_pFree = pNode;

	ATLASSERT( m_nElements > 0 );
	m_nElements--;

	if( (m_nElements < m_nLoRehashThreshold) && !IsLocked() )
	{
		Rehash( PickSize( m_nElements ) );
	}

	if( m_nElements == 0 )
	{
		FreePlexes();
	}
}

template< typename K, typename V, class KTraits, class VTraits >
void CAtlMap< K, V, KTraits, VTraits >::FreePlexes() throw()
{
	m_pFree = NULL;
	if( m_pBlocks != NULL )
	{
		m_pBlocks->FreeDataChain();
		m_pBlocks = NULL;
	}
}

template< typename K, typename V, class KTraits, class VTraits >
CAtlMap< K, V, KTraits, VTraits >::CNode* CAtlMap< K, V, KTraits, VTraits >::GetNode(
	KINARGTYPE key, UINT& iBin, UINT& nHash, CNode*& pPrev ) const
{
	CNode* pFollow;

	nHash = KTraits::Hash( key );
	iBin = nHash%m_nBins;

	if( m_ppBins == NULL )
	{
		return( NULL );
	}

	pFollow = NULL;
	pPrev = NULL;
	for( CNode* pNode = m_ppBins[iBin]; pNode != NULL; pNode = pNode->m_pNext )
	{
		if( (pNode->GetHash() == nHash) && KTraits::CompareElements( pNode->m_key, key ) )
		{
			pPrev = pFollow;
			return( pNode );
		}
		pFollow = pNode;
	}

	return( NULL );
}

template< typename K, typename V, class KTraits, class VTraits >
bool CAtlMap< K, V, KTraits, VTraits >::Lookup( KINARGTYPE key, VOUTARGTYPE value ) const
{
	UINT iBin;
	UINT nHash;
	CNode* pNode;
	CNode* pPrev;

	pNode = GetNode( key, iBin, nHash, pPrev );
	if( pNode == NULL )
	{
		return( false );
	}

	value = pNode->m_value;

	return( true );
}

template< typename K, typename V, class KTraits, class VTraits >
const CAtlMap< K, V, KTraits, VTraits >::CPair* CAtlMap< K, V, KTraits, VTraits >::Lookup( KINARGTYPE key ) const
{
	UINT iBin;
	UINT nHash;
	CNode* pNode;
	CNode* pPrev;

	pNode = GetNode( key, iBin, nHash, pPrev );
	
	return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
CAtlMap< K, V, KTraits, VTraits >::CPair* CAtlMap< K, V, KTraits, VTraits >::Lookup( KINARGTYPE key )
{
	UINT iBin;
	UINT nHash;
	CNode* pNode;
	CNode* pPrev;

	pNode = GetNode( key, iBin, nHash, pPrev );
	
	return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
bool CAtlMap< K, V, KTraits, VTraits >::RemoveKey( KINARGTYPE key )
{
	CNode* pNode;
	UINT iBin;
	UINT nHash;
	CNode* pPrev;

	pPrev = NULL;
	pNode = GetNode( key, iBin, nHash, pPrev );
	if( pNode == NULL )
	{
		return( false );
	}

	RemoveNode( pNode, pPrev );

	return( true );
}

template< typename K, typename V, class KTraits, class VTraits >
void CAtlMap< K, V, KTraits, VTraits >::RemoveNode( CNode* pNode, CNode* pPrev )
{
	UINT iBin;

	ATLASSERT( pNode != NULL );

	iBin = pNode->GetHash()%m_nBins;
	if( pPrev == NULL )
	{
		ATLASSERT( m_ppBins[iBin] == pNode );
		m_ppBins[iBin] = pNode->m_pNext;
	}
	else
	{
		ATLASSERT( pPrev->m_pNext == pNode );
		pPrev->m_pNext = pNode->m_pNext;
	}
	FreeNode( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
void CAtlMap< K, V, KTraits, VTraits >::RemoveAtPos( POSITION pos )
{
	CNode* pNode;
	CNode* pPrev;
	UINT iBin;

	ATLASSERT( pos != NULL );
	pNode = static_cast< CNode* >( pos );
	iBin = pNode->GetHash()%m_nBins;

	ATLASSERT( m_ppBins[iBin] != NULL );
	if( pNode == m_ppBins[iBin] )
	{
		pPrev = NULL;
	}
	else
	{
		pPrev = m_ppBins[iBin];
		while( pPrev->m_pNext != pNode )
		{
			pPrev = pPrev->m_pNext;
			ATLASSERT( pPrev != NULL );
		}
	}
	RemoveNode( pNode, pPrev );
}

template< typename K, typename V, class KTraits, class VTraits >
void CAtlMap< K, V, KTraits, VTraits >::Rehash( UINT nBins )
{
	CNode** ppBins = NULL;

	if( nBins == 0 )
	{
		nBins = PickSize( m_nElements );
	}

	if( nBins == m_nBins )
	{
		return;
	}

	ATLTRACE(atlTraceMap, 2, _T("Rehash: %u bins\n"), nBins );

	if( m_ppBins == NULL )
	{
		// Just set the new number of bins
		InitHashTable( nBins, false );
		return;
	}

	ATLTRY(ppBins = new CNode*[nBins]);
	if (ppBins == NULL)
	{
		ATL::AtlThrow( E_OUTOFMEMORY );
	}

	memset( ppBins, 0, nBins*sizeof( CNode* ) );

	// Nothing gets copied.  We just rewire the old nodes
	// into the new bins.
	for( UINT iSrcBin = 0; iSrcBin < m_nBins; iSrcBin++ )
	{
		CNode* pNode;

		pNode = m_ppBins[iSrcBin];
		while( pNode != NULL )
		{
			CNode* pNext;
			UINT iDestBin;

			pNext = pNode->m_pNext;  // Save so we don't trash it
			iDestBin = pNode->GetHash()%nBins;
			pNode->m_pNext = ppBins[iDestBin];
			ppBins[iDestBin] = pNode;

			pNode = pNext;
		}
	}

	delete[] m_ppBins;
	m_ppBins = ppBins;
	m_nBins = nBins;

	UpdateRehashThresholds();
}

template< typename K, typename V, class KTraits, class VTraits >
void CAtlMap< K, V, KTraits, VTraits >::GetNextAssoc( POSITION& pos, KOUTARGTYPE key,
	VOUTARGTYPE value ) const
{
	CNode* pNode;
	CNode* pNext;

	ATLASSERT( m_ppBins != NULL );
	ATLASSERT( pos != NULL );

	pNode = (CNode*)pos;
	pNext = FindNextNode( pNode );

	pos = POSITION( pNext );
	key = pNode->m_key;
	value = pNode->m_value;
}

template< typename K, typename V, class KTraits, class VTraits >
const CAtlMap< K, V, KTraits, VTraits >::CPair* CAtlMap< K, V, KTraits, VTraits >::GetNext( POSITION& pos ) const
{
	CNode* pNode;
	CNode* pNext;

	ATLASSERT( m_ppBins != NULL );
	ATLASSERT( pos != NULL );

	pNode = (CNode*)pos;
	pNext = FindNextNode( pNode );

	pos = POSITION( pNext );

	return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
CAtlMap< K, V, KTraits, VTraits >::CPair* CAtlMap< K, V, KTraits, VTraits >::GetNext( 
	POSITION& pos ) throw()
{
	ATLASSERT( m_ppBins != NULL );
	ATLASSERT( pos != NULL );

	CNode* pNode = static_cast< CNode* >( pos );
	CNode* pNext = FindNextNode( pNode );

	pos = POSITION( pNext );

	return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
const K& CAtlMap< K, V, KTraits, VTraits >::GetNextKey( POSITION& pos ) const
{
	CNode* pNode;
	CNode* pNext;

	ATLASSERT( m_ppBins != NULL );
	ATLASSERT( pos != NULL );

	pNode = (CNode*)pos;
	pNext = FindNextNode( pNode );

	pos = POSITION( pNext );
	
	return( pNode->m_key );
}

template< typename K, typename V, class KTraits, class VTraits >
const V& CAtlMap< K, V, KTraits, VTraits >::GetNextValue( POSITION& pos ) const
{
	CNode* pNode;
	CNode* pNext;

	ATLASSERT( m_ppBins != NULL );
	ATLASSERT( pos != NULL );

	pNode = (CNode*)pos;
	pNext = FindNextNode( pNode );

	pos = POSITION( pNext );
	
	return( pNode->m_value );
}

template< typename K, typename V, class KTraits, class VTraits >
V& CAtlMap< K, V, KTraits, VTraits >::GetNextValue( POSITION& pos )
{
	CNode* pNode;
	CNode* pNext;

	ATLASSERT( m_ppBins != NULL );
	ATLASSERT( pos != NULL );

	pNode = (CNode*)pos;
	pNext = FindNextNode( pNode );

	pos = POSITION( pNext );
	
	return( pNode->m_value );
}

template< typename K, typename V, class KTraits, class VTraits >
CAtlMap< K, V, KTraits, VTraits >::CNode* CAtlMap< K, V, KTraits, VTraits >::FindNextNode( CNode* pNode ) const
{
	CNode* pNext;

	if( pNode->m_pNext != NULL )
	{
		pNext = pNode->m_pNext;
	}
	else
	{
		UINT iBin;

		pNext = NULL;
		iBin = (pNode->GetHash()%m_nBins)+1;
		while( (pNext == NULL) && (iBin < m_nBins) )
		{
			if( m_ppBins[iBin] != NULL )
			{
				pNext = m_ppBins[iBin];
			}

			iBin++;
		}
	}

	return( pNext );
}

#ifdef _DEBUG
template< typename K, typename V, class KTraits, class VTraits >
void CAtlMap< K, V, KTraits, VTraits >::AssertValid() const
{
	ATLASSERT( m_nBins > 0 );
	// non-empty map should have hash table
	ATLASSERT( IsEmpty() || (m_ppBins != NULL) );
}
#endif

#pragma push_macro("new")
#undef new

template< typename K, typename V, class KTraits = CElementTraits< K >, class VTraits = CElementTraits< V > >
class CRBTree
{
public:
	typedef KTraits::INARGTYPE KINARGTYPE;
	typedef KTraits::OUTARGTYPE KOUTARGTYPE;
	typedef VTraits::INARGTYPE VINARGTYPE;
	typedef VTraits::OUTARGTYPE VOUTARGTYPE;

public:
	class CPair : 
		public __POSITION
	{
	protected:
		CPair( KINARGTYPE key, VINARGTYPE value ) : 
			m_key( key ),
			m_value( value )
		{
		}
		~CPair() throw()
		{
		}

	public:
		const K m_key;
		V m_value;
	};

private:
	class CNode : 
		public CPair
	{
	public:
		enum RB_COLOR
		{
			RB_RED, 
			RB_BLACK
		};

	public:
		RB_COLOR m_eColor;
		CNode* m_pLeft;
		CNode* m_pRight;
		CNode* m_pParent;

		CNode( KINARGTYPE key, VINARGTYPE value ) : 
			CPair( key, value ),
			m_pParent( NULL ),
			m_eColor( RB_BLACK )
		{
		}
		~CNode() throw()
		{
		}
	};

private:
	CNode* m_pRoot;
	size_t m_nCount;
	CNode* m_pFree;
	CAtlPlex* m_pBlocks;
	size_t m_nBlockSize;

	// methods

	CNode* NewNode( KINARGTYPE key, VINARGTYPE value ) throw( ... )
	{
		CNode* pNewNode;

		if( m_pFree == NULL )
		{
			CAtlPlex* pPlex;
			CNode* pNode;

			pPlex = CAtlPlex::Create( m_pBlocks, m_nBlockSize, sizeof( CNode ) );
			if( pPlex == NULL )
			{
				AtlThrow( E_OUTOFMEMORY );
			}
			pNode = (CNode*)pPlex->data();
			pNode += m_nBlockSize-1;
			for( INT_PTR iBlock = m_nBlockSize-1; iBlock >= 0; iBlock-- )
			{
				pNode->m_pLeft = m_pFree;
				m_pFree = pNode;
				pNode--;
			}
		}
		ATLASSERT( m_pFree != NULL );
		pNewNode = m_pFree;

		::new( pNewNode ) CNode( key, value );

		m_pFree = m_pFree->m_pLeft;
		pNewNode->m_pLeft = NULL;
		pNewNode->m_pRight = NULL;
		m_nCount++;
		ATLASSERT( m_nCount > 0 );

		return( pNewNode );
	}

	void FreeNode(CNode* pNode) throw()
	{
		ATLASSERT(pNode != NULL);
		pNode->~CNode();
		pNode->m_pLeft = m_pFree;
		m_pFree = pNode;
		ATLASSERT( m_nCount > 0 );
		m_nCount--;
	}

	void RemovePostOrder(CNode* pNode)
	{
		if (!pNode)
			return;
		RemovePostOrder(pNode->m_pLeft);
		RemovePostOrder(pNode->m_pRight);
		FreeNode( pNode );
	}

	CNode* LeftRotate(CNode* pNode) throw()
	{
		ATLASSERT(pNode != NULL);
		CNode* pRight(pNode->m_pRight);
		pNode->m_pRight = pRight->m_pLeft;
		if (pRight->m_pLeft != NULL)
			pRight->m_pLeft->m_pParent = pNode;

		pRight->m_pParent = pNode->m_pParent;
		if (pNode->m_pParent == NULL)
			m_pRoot = pRight;
		else if (pNode == pNode->m_pParent->m_pLeft)
			pNode->m_pParent->m_pLeft = pRight;
		else 
			pNode->m_pParent->m_pRight = pRight;

		pRight->m_pLeft = pNode;
		pNode->m_pParent = pRight;
		return pNode;

	}

	CNode* RightRotate(CNode* pNode) throw()
	{
		ATLASSERT(pNode != NULL);
		CNode* pLeft(pNode->m_pLeft);
		pNode->m_pLeft = pLeft->m_pRight;
		if (pLeft->m_pRight != NULL)
			pLeft->m_pRight->m_pParent = pNode;

		pLeft->m_pParent = pNode->m_pParent;
		if (pNode->m_pParent == NULL)
			m_pRoot = pLeft;
		else if (pNode == pNode->m_pParent->m_pRight)
			pNode->m_pParent->m_pRight = pLeft;
		else
			pNode->m_pParent->m_pLeft = pLeft;

		pLeft->m_pRight = pNode;
		pNode->m_pParent = pLeft;
		return pNode;
	}

	CNode* Find(KINARGTYPE key) const throw()
	{
		CNode* pNode;
		CNode* pKey;

		pKey = NULL;
		pNode = m_pRoot;
		while( (pNode != NULL) && (pKey == NULL) )
		{
			int nCompare;

			nCompare = KTraits::CompareElementsOrdered( key, pNode->m_key );
			if( nCompare == 0 )
			{
				pKey = pNode;
			}
			else
			{
				if( nCompare < 0 )
				{
					pNode = pNode->m_pLeft;
				}
				else
				{
					pNode = pNode->m_pRight;
				}
			}
		}

		if( pKey == NULL )
		{
			return( NULL );
		}

		while( true )
		{
			CNode* pPrev;

			pPrev = Predecessor( pKey );
			if( (pPrev != NULL) && KTraits::CompareElements( key, pPrev->m_key ) )
			{
				pKey = pPrev;
			}
			else
			{
				return( pKey );
			}
		}
	}

	CNode* FindPrefix( KINARGTYPE key ) const throw()
	{
		CNode* pNode;
		CNode* pParent;
		CNode* pKey;

		pParent = NULL;
		pKey = NULL;
		pNode = m_pRoot;
		while( (pNode != NULL) && (pKey == NULL) )
		{
			int nCompare;
			
			pParent = pNode;
			nCompare = KTraits::CompareElementsOrdered( key, pNode->m_key );
			if( nCompare == 0 )
			{
				pKey = pNode;
			}
			else if( nCompare < 0 )
			{
				pNode = pNode->m_pLeft;
			}
			else
			{
				pNode = pNode->m_pRight;
			}
		}

		if( pKey != NULL )
		{
			while( true )
			{
				CNode* pPrev;

				pPrev = Predecessor( pKey );
				if( (pPrev != NULL) && KTraits::CompareElements( key, pPrev->m_key ) )
				{
					pKey = pPrev;
				}
				else
				{
					return( pKey );
				}
			}
		}
		else
		{
			int nCompare;

			nCompare = KTraits::CompareElements( key, pParent->m_key );
			if( nCompare < 0 )
			{
				pKey = pParent;
			}
			else
			{
				_ASSERTE( nCompare > 0 );
				pKey = Successor( pParent );
			}
		}

		return( pKey );
	}

	void SwapNode(CNode* pDest, CNode* pSrc) throw()
	{
		ATLASSERT(pDest != NULL);
		ATLASSERT(pSrc != NULL);

		if (pSrc->m_pParent)
		{
			pDest->m_pParent = pSrc->m_pParent;
			if (pSrc->m_pParent->m_pLeft == pSrc)
				pSrc->m_pParent->m_pLeft = pDest;
			else
				pSrc->m_pParent->m_pRight = pDest;
		}

		pDest->m_pRight = pSrc->m_pRight;
		pDest->m_pLeft = pSrc->m_pLeft;
		pDest->m_eColor = pSrc->m_eColor;
		if (pDest->m_pRight)
			pDest->m_pRight->m_pParent = pDest;
		if (pDest->m_pLeft)
			pDest->m_pLeft->m_pParent = pDest;
		if (m_pRoot == pSrc)
			m_pRoot = pDest;
	}

	CNode* InsertImpl( KINARGTYPE key, VINARGTYPE value ) throw( ... )
	{
		CNode* pNew = NULL;

		pNew = NewNode( key, value );

		CNode* pY = NULL;
		CNode* pX = m_pRoot;

		while (pX != NULL)
		{
			pY = pX;
			if( KTraits::CompareElementsOrdered( key, pX->m_key ) <= 0 )
				pX = pX->m_pLeft;
			else
				pX = pX->m_pRight;
		}

		pNew->m_pParent = pY;
		if (pY == NULL)
		{
			m_pRoot = pNew;
		}
		else if( KTraits::CompareElementsOrdered( key, pY->m_key ) <= 0 )
			pY->m_pLeft = pNew;
		else
			pY->m_pRight = pNew;

		pNew->m_value = value;
		return pNew;
	}

	bool RBDeleteFixup(CNode* pNode) throw()
	{
		if (pNode == NULL)
			return true;

		CNode* pX = pNode;
		CNode* pW = NULL;

		while (pX != m_pRoot && pX->m_eColor == CNode::RB_BLACK)
		{
			if (pX == pX->m_pParent->m_pLeft)
			{
				pW = pX->m_pParent->m_pRight;
				if (pW->m_eColor == CNode::RB_RED)
				{
					pW->m_eColor = CNode::RB_BLACK;
					pW->m_pParent->m_eColor = CNode::RB_RED;
					LeftRotate(pX->m_pParent);
					pW = pX->m_pParent->m_pRight;
				}
				if (pW->m_pLeft->m_eColor == CNode::RB_BLACK && pW->m_pRight->m_eColor == CNode::RB_BLACK)
				{
					pW->m_eColor = CNode::RB_RED;
					pX = pX->m_pParent;
				}
				else
				{
					if (pW->m_pRight->m_eColor == CNode::RB_BLACK)
					{
						pW->m_pLeft->m_eColor = CNode::RB_BLACK;
						pW->m_eColor = CNode::RB_RED;
						RightRotate(pW);
						pW = pX->m_pParent->m_pRight;
					}
					pW->m_eColor = pX->m_pParent->m_eColor;
					pX->m_pParent->m_eColor = CNode::RB_BLACK;
					pW->m_pRight->m_eColor = CNode::RB_BLACK;
					LeftRotate(pX->m_pParent);
					pX = m_pRoot;
				}
			}
			else
			{
				pW = pX->m_pParent->m_pLeft;
				if (pW->m_eColor == CNode::RB_RED)
				{
					pW->m_eColor = CNode::RB_BLACK;
					pW->m_pParent->m_eColor = CNode::RB_RED;
					RightRotate(pX->m_pParent);
					pW = pX->m_pParent->m_pLeft;
				}
				if (pW->m_pRight->m_eColor == CNode::RB_BLACK && pW->m_pLeft->m_eColor == CNode::RB_BLACK)
				{
					pW->m_eColor = CNode::RB_RED;
					pX = pX->m_pParent;
				}
				else
				{
					if (pW->m_pLeft->m_eColor == CNode::RB_BLACK)
					{
						pW->m_pRight->m_eColor = CNode::RB_BLACK;
						pW->m_eColor = CNode::RB_RED;
						LeftRotate(pW);
						pW = pX->m_pParent->m_pLeft;
					}
					pW->m_eColor = pX->m_pParent->m_eColor;
					pX->m_pParent->m_eColor = CNode::RB_BLACK;
					pW->m_pLeft->m_eColor = CNode::RB_BLACK;
					RightRotate(pX->m_pParent);
					pX = m_pRoot;
				}
			}

			pX->m_eColor = CNode::RB_BLACK;
		}

		return true;
	}


	bool RBDelete(CNode* pZ) throw()
	{
		if (pZ == NULL)
			return false;

		CNode* pY = NULL;
		CNode* pX = NULL;
		if (pZ->m_pLeft == NULL || pZ->m_pRight == NULL)
			pY = pZ;
		else
			pY = Successor(pZ);

		if (pY->m_pLeft != NULL)
			pX = pY->m_pLeft;
		else
			pX = pY->m_pRight;

		if (pX != NULL)
			pX->m_pParent = pY->m_pParent;

		if (pY->m_pParent == NULL)
			m_pRoot = pX;
		else if (pY == pY->m_pParent->m_pLeft)
			pY->m_pParent->m_pLeft = pX;
		else
			pY->m_pParent->m_pRight = pX;

		if (pY != pZ)
			SwapNode(pY, pZ);

		FreeNode( pZ );

		if (pY->m_eColor == CNode::RB_BLACK)
			return RBDeleteFixup(pX);

		if (m_pRoot != NULL)
			m_pRoot->m_pParent = NULL;

		return true;
	}

	CNode* Minimum(CNode* pNode) const throw()
	{
		if (pNode == NULL)
			return NULL;
		CNode* pMin = pNode;
		while (pMin->m_pLeft != NULL)
			pMin = pMin->m_pLeft;

		return pMin;
	}

	CNode* Maximum(CNode* pNode) const throw()
	{
		if (pNode == NULL)
			return NULL;
		CNode* pMax = pNode;
		while (pMax->m_pRight != NULL)
			pMax = pMax->m_pRight;

		return pMax;
	}

	CNode* Predecessor( CNode* pNode ) const throw()
	{
		if( pNode == NULL )
		{
			return( NULL );
		}
		if( pNode->m_pLeft != NULL )
		{
			return( Maximum( pNode->m_pLeft ) );
		}

		CNode* pParent = pNode->m_pParent;
		CNode* pLeft = pNode;
		while( (pParent != NULL) && (pLeft == pParent->m_pLeft) )
		{
			pLeft = pParent;
			pParent = pParent->m_pParent;
		}

		return( pParent );
	}

	CNode* Successor(CNode* pNode) const throw()
	{
		if (pNode == NULL)
			return NULL;
		if (pNode->m_pRight != NULL)
			return Minimum(pNode->m_pRight);
		
		CNode* pParent = pNode->m_pParent;
		CNode* pRight = pNode;
		while (pParent != NULL && pRight == pParent->m_pRight)
		{
			pRight = pParent;
			pParent = pParent->m_pParent;
		}

		return pParent;
	}

	POSITION RBInsert( KINARGTYPE key, VINARGTYPE value ) throw( ... )
	{
		CNode* pNewNode;
		CNode* pX;

		pNewNode = InsertImpl( key, value );

		pX = pNewNode;
		pX->m_eColor = CNode::RB_RED;
		CNode* pY = NULL;
		while (pX != m_pRoot && pX->m_pParent->m_eColor == CNode::RB_RED)
		{
			if (pX->m_pParent == pX->m_pParent->m_pParent->m_pLeft)
			{
				pY = pX->m_pParent->m_pParent->m_pRight;
				if (pY != NULL && pY->m_eColor == CNode::RB_RED)
				{
					pX->m_pParent->m_eColor = CNode::RB_BLACK;
					pY->m_eColor = CNode::RB_BLACK;
					pX->m_pParent->m_pParent->m_eColor = CNode::RB_RED;
					pX = pX->m_pParent->m_pParent;
				}
				else
				{
					if (pX == pX->m_pParent->m_pRight)
					{
						pX = pX->m_pParent;
						LeftRotate(pX);
					}
					pX->m_pParent->m_eColor = CNode::RB_BLACK;
					pX->m_pParent->m_pParent->m_eColor = CNode::RB_RED;
					RightRotate(pX->m_pParent->m_pParent);
				}
			}
			else
			{
				pY = pX->m_pParent->m_pParent->m_pLeft;
				if (pY != NULL && pY->m_eColor == CNode::RB_RED)
				{
					pX->m_pParent->m_eColor = CNode::RB_BLACK;
					pY->m_eColor = CNode::RB_BLACK;
					pX->m_pParent->m_pParent->m_eColor = CNode::RB_RED;
					pX = pX->m_pParent->m_pParent;
				}
				else
				{
					if (pX == pX->m_pParent->m_pLeft)
					{
						pX = pX->m_pParent;
						RightRotate(pX);
					}
					pX->m_pParent->m_eColor = CNode::RB_BLACK;
					pX->m_pParent->m_pParent->m_eColor = CNode::RB_RED;
					LeftRotate(pX->m_pParent->m_pParent);
				}
			}
		}

		m_pRoot->m_eColor = CNode::RB_BLACK;
		m_pRoot->m_pParent = NULL;

		return( pNewNode );
	}

public:
	explicit CRBTree( size_t nBlockSize = 10 ) throw() :
		m_pRoot( NULL ),
		m_nCount( 0 ),
		m_nBlockSize( nBlockSize ),
		m_pFree( NULL ),
		m_pBlocks( NULL )
	{
		ATLASSERT( nBlockSize > 0 );
	}

	~CRBTree() throw()
	{
		RemoveAll();
	}

	void RemoveAll() throw()
	{
		if (m_pRoot)
			RemovePostOrder(m_pRoot);
		m_nCount = 0;
		m_pBlocks->FreeDataChain();
		m_pBlocks = NULL;
		m_pFree = NULL;
	}

	size_t GetCount() const throw()
	{
		return m_nCount;
	}

	ATL_DEPRECATED size_t GetSize() const throw()
	{
		return m_nCount;
	}

	bool IsEmpty() const throw()
	{
		return( m_nCount == 0 );
	}

	bool Lookup( KINARGTYPE key, VOUTARGTYPE value ) throw( ... )
	{
		CNode* pLookup = Find( key );
		if( pLookup == NULL )
			return false;

		value = pLookup->m_value;
		return true;
	}

	POSITION FindFirstKeyAfter( KINARGTYPE key ) const throw()
	{
		return( FindPrefix( key ) );
	}

	POSITION FindFirstWithKey( KINARGTYPE key ) const throw()
	{
		return( Find( key ) );
	}

	const CPair* GetNextWithKey( POSITION& pos, KINARGTYPE key ) const throw()
	{
		CNode* pNode;
		CNode* pNextNode;

		ATLASSERT( pos != NULL );
		pNode = static_cast< CNode* >( pos );
		pNextNode = Successor( pNode );
		if( (pNextNode != NULL) && KTraits::CompareElements( pNextNode->m_key, key ) )
		{
			pos = pNextNode;
		}
		else
		{
			pos = NULL;
		}

		return( pNode );
	}

	const V& GetNextValueWithKey( POSITION& pos, KINARGTYPE key ) const throw()
	{
		const CPair* pPair;

		pPair = GetNextWithKey( pos, key );

		return( pPair->m_value );
	}

	const CPair* Lookup( KINARGTYPE key ) const throw()
	{
		return Find(key);
	}

	bool RemoveKey( KINARGTYPE key ) throw()
	{
		CNode* pDel = Find(key);
		return RBDelete(pDel);
	}

	POSITION Insert( KINARGTYPE key, VINARGTYPE value ) throw( ... )
	{
		return( RBInsert( key, value ) );
	}

	void RemoveAt(POSITION pos) throw()
	{
		ATLASSERT(pos != NULL);
		RBDelete((CNode*)(pos));
	}

	POSITION GetStartPosition() const throw()
	{
		return static_cast<POSITION>(Minimum(m_pRoot));
	}
	POSITION GetHeadPosition() const throw()
	{
		return( Minimum( m_pRoot ) );
	}
	POSITION GetTailPosition() const throw()
	{
		return( Maximum( m_pRoot ) );
	}

	void GetNextAssoc( POSITION& pos, KOUTARGTYPE key, VOUTARGTYPE value ) const
	{
		ATLASSERT(pos != NULL);
		CNode* pNode = (CNode*)(pos);
		
		key = pNode->m_key;
		value = pNode->m_value;

		pos = static_cast<POSITION>(Successor(pNode));
	}

	const CPair* GetNext(POSITION& pos) const throw()
	{
		ATLASSERT(pos != NULL);
		CNode* pNode = (CNode*)(pos);
		pos = static_cast<POSITION>(Successor(pNode));
		return static_cast<CPair*>(pNode);
	}

	const CPair* GetPrev(POSITION& pos) const throw()
	{
		ATLASSERT(pos != NULL);
		CNode* pNode = Predecessor((CNode*)pos);
		pos = static_cast<POSITION>(pNode);
		
		return static_cast<CPair*>(pNode);
	}

	const K& GetNextKey(POSITION& pos) const throw()
	{
		ATLASSERT(pos != NULL);
		CNode* pNode = (CNode*)(pos);
		pos = static_cast<POSITION>(Successor(pNode));
		
		return pNode->m_key;
	}

	const V& GetNextValue(POSITION& pos) const throw()
	{
		ATLASSERT(pos != NULL);
		CNode* pNode = (CNode*)(pos);
		pos = static_cast<POSITION>(Successor(pNode));
		
		return pNode->m_Value;
	}

	V& GetNextValue(POSITION& pos) throw()
	{
		ATLASSERT(pos != NULL);
		CNode* pNode = (CNode*)(pos);
		pos = static_cast<POSITION>(Successor(pNode));
		
		return pNode->m_value;
	}

	CPair* GetAt( POSITION pos ) throw()
	{
		ATLASSERT( pos != NULL );

		return( static_cast< CPair* >( pos ) );
	}

	const CPair* GetAt( POSITION pos ) const throw()
	{
		ATLASSERT( pos != NULL );

		return( static_cast< const CPair* >( pos ) );
	}

	void GetAtPos(POSITION pos, KOUTARGTYPE key, VOUTARGTYPE value) const
	{
		ATLASSERT(pos != NULL);
		key = ((CNode*)(pos))->m_key;
		value = ((CNode*)(pos))->m_value;
	}

	const K& GetKeyAt(POSITION pos) const throw()
	{
		ATLASSERT(pos != NULL);
		return ((CNode*)(pos))->m_key;
	}

	const V& GetValueAt(POSITION pos) const throw()
	{
		ATLASSERT(pos != NULL);
		return ((CNode*)(pos))->m_value;
	}

	V& GetValueAt(POSITION pos) throw()
	{
		ATLASSERT(pos != NULL);
		return ((CNode*)(pos))->m_value;
	}
};

#pragma pop_macro("new")

#pragma warning(pop)

}; // namespace ATL

#endif  // __ATLCOLL_H__
