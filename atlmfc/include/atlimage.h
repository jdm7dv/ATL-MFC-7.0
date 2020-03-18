// This is a part of the Active Template Library.
// Copyright (C) 1996-2000 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLIMAGE_H__
#define __ATLIMAGE_H__

#pragma once

#include <atldef.h>
#include <atlbase.h>
#include <atlsimpstr.h>
#include <atlsimpcoll.h>
#include <atltypes.h>

#include <bmio.h>
#include <atlbmid.h>

#include <shlwapi.h>

#ifndef _ATL_NO_DEFAULT_LIBS
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shlwapi.lib")
#if WINVER >= 0x0500
#pragma comment(lib, "msimg32.lib")
#endif  // WINVER >= 0x0500
#endif  // !_ATL_NO_DEFAULT_LIBS

#pragma pack(push, _ATL_PACKING)

namespace ATL
{

class CStreamOnFILE :
	public ISequentialStream
{
public:
	CStreamOnFILE( HANDLE hFile ) :
		m_nRefCount( 0 ),
		m_hFile( hFile )
	{
	}
	~CStreamOnFILE()
	{
		::CloseHandle( m_hFile );
	}

// IUnknown
public:
	STDMETHOD_( ULONG, AddRef )()
	{
		m_nRefCount++;

		return( m_nRefCount );
	}
	STDMETHOD( QueryInterface )( REFIID iid, void** ppInterface )
	{
		if( ppInterface == NULL )
		{
			return( E_POINTER );
		}
		*ppInterface = NULL;

		if( (iid == __uuidof(IUnknown)) || (iid == __uuidof(ISequentialStream)) )
		{
			*ppInterface = static_cast< ISequentialStream* >( this );
		}
		else
		{
			return( E_NOINTERFACE );
		}

		((IUnknown*)(*ppInterface))->AddRef();

		return( S_OK );
	}
	STDMETHOD_( ULONG, Release )()
	{
		m_nRefCount--;

		if( m_nRefCount == 0 )
		{
			delete this;
			return( 0 );
		}

		return( m_nRefCount );
	}

// ISequentialStream
public:
	STDMETHOD( Read )( void* pBuffer, ULONG nBytes, ULONG* pnBytesRead )
	{
		ULONG nBytesRead;

		if( pnBytesRead != NULL )
		{
			*pnBytesRead = 0;
		}
		if( pBuffer == NULL )
		{
			return( E_POINTER );
		}
		if( nBytes == 0 )
		{
			return( E_INVALIDARG );
		}

		nBytesRead = 0;
		::ReadFile( m_hFile, pBuffer, nBytes, &nBytesRead, NULL );
		if( pnBytesRead != NULL )
		{
			*pnBytesRead = nBytesRead;
		}

		if( nBytesRead != nBytes )
		{
			return( E_FAIL );
		}

		return( S_OK );
	}
	STDMETHOD( Write )( const void* pBuffer, ULONG nBytes, ULONG* pnBytesWritten )
	{
		ULONG nBytesWritten;

		if( pnBytesWritten != NULL )
		{
			*pnBytesWritten = 0;
		}
		if( (pBuffer == NULL) || (nBytes == 0) )
		{
			return( E_INVALIDARG );
		}

		::WriteFile( m_hFile, pBuffer, nBytes, &nBytesWritten, NULL );
		if( pnBytesWritten != NULL )
		{
			*pnBytesWritten = nBytesWritten;
		}

		if( nBytesWritten != nBytes )
		{
			return( E_FAIL );
		}

		return( S_OK );
	}

protected:
	ULONG m_nRefCount;
	HANDLE m_hFile;
};

const int CIMAGE_DC_CACHE_SIZE = 4;

class CImage
{
private:
	class CDCCache
	{
	public:
		CDCCache() throw();
		~CDCCache() throw();

		HDC GetDC() throw();
		void ReleaseDC( HDC ) throw();

	private:
		HDC m_ahDCs[CIMAGE_DC_CACHE_SIZE];
	};

public:
	enum PaletteOptions
	{
		palOptimal = 0x00000000,
		palHalftone = 0x00000001,
		palPalGenOptions = 0x000000ff,

		palAvoidSystemColors = 0x00000000,
		palUseSystemColors = 0x00000100,
		palOverwriteSystemColors = 0x00000200,
		palSystemColorOptions = 0x0000ff00,

		palDitherNone = 0x00000000,
		palDitherErrorDiffusion = 0x00010000,
		palDitherOptions = 0x00ff0000
	};
	enum DIBFormatFlags
	{
		fmt1BPP = DIBTARGET_1BPP,
		fmt2BPP = DIBTARGET_2BPP,
		fmt4BPP = DIBTARGET_4BPP,
		fmt8BPP = DIBTARGET_8BPP,
		fmt16BPP = DIBTARGET_16BPP,
		fmt24BPP = DIBTARGET_24BPP,
		fmt32BPP = DIBTARGET_32BPP,
		fmt32BPP_ALPHA = DIBTARGET_32BPP_ALPHA,
		fmtAnyIndexed = DIBTARGET_ANYINDEXED,
		fmtAnyRGB = DIBTARGET_ANYRGB,
		fmtAny = DIBTARGET_ANY
	};
	enum CreateFlags
	{
		createAlphaChannel = 0x01
	};
	enum DIBOrientation
	{
		DIBOR_DEFAULT,
		DIBOR_TOPDOWN,
		DIBOR_BOTTOMUP
	};

public:
	CImage() throw();
	virtual ~CImage() throw();

	operator HBITMAP() const throw();
#if WINVER >= 0x0500
	BOOL AlphaBlend( HDC hDestDC, int xDest, int yDest, BYTE bSrcAlpha = 0xff, 
		BYTE bBlendOp = AC_SRC_OVER ) const throw();
	BOOL AlphaBlend( HDC hDestDC, const POINT& pointDest, BYTE bSrcAlpha = 0xff, 
		BYTE bBlendOp = AC_SRC_OVER ) const throw();
	BOOL AlphaBlend( HDC hDestDC, int xDest, int yDest, int nDestWidth, 
		int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, 
		BYTE bSrcAlpha = 0xff, BYTE bBlendOp = AC_SRC_OVER ) const throw();
	BOOL AlphaBlend( HDC hDestDC, const RECT& rectDest, const RECT& rectSrc, 
		BYTE bSrcAlpha = 0xff, BYTE bBlendOp = AC_SRC_OVER ) const throw();
#endif  // WINVER >= 0x0500
	void Attach( HBITMAP hBitmap, DIBOrientation eOrientation = DIBOR_DEFAULT ) throw();
	BOOL BitBlt( HDC hDestDC, int xDest, int yDest, DWORD dwROP = SRCCOPY ) const throw();
	BOOL BitBlt( HDC hDestDC, const POINT& pointDest, DWORD dwROP = SRCCOPY ) const throw();
	BOOL BitBlt( HDC hDestDC, int xDest, int yDest, int nDestWidth, 
		int nDestHeight, int xSrc, int ySrc, DWORD dwROP = SRCCOPY ) const throw();
	BOOL BitBlt( HDC hDestDC, const RECT& rectDest, const POINT& pointSrc, 
		DWORD dwROP = SRCCOPY ) const throw();
	BOOL Create( int nWidth, int nHeight, int nBPP, DWORD dwFlags = 0 ) throw();
	void Destroy() throw();
	HBITMAP Detach() throw();
	BOOL Draw( HDC hDestDC, int xDest, int yDest, int nDestWidth, 
		int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight ) const throw();
	BOOL Draw( HDC hDestDC, const RECT& rectDest, const RECT& rectSrc ) const throw();
	BOOL Draw( HDC hDestDC, int xDest, int yDest ) const throw();
	BOOL Draw( HDC hDestDC, const POINT& pointDest ) const throw();
	BOOL Draw( HDC hDestDC, int xDest, int yDest, int nDestWidth, 
		int nDestHeight ) const throw();
	BOOL Draw( HDC hDestDC, const RECT& rectDest ) const throw();
	const void* GetBits() const throw();
	void* GetBits() throw();
	int GetBPP() const throw();
	void GetColorTable( UINT iFirstColor, UINT nColors, RGBQUAD* prgbColors ) const throw();
	HDC GetDC() const throw();
	HRESULT GetExporterFilterString( CSimpleString& strExporters, 
		CSimpleArray< GUID >& aguidFileTypes, TCHAR chSeparator = _T( '|' ) ) const;
	int GetHeight() const throw();
	int GetMaxColorTableEntries() const throw();
	int GetPitch() const throw();
	const void* GetPixelAddress( int x, int y ) const throw();
	void* GetPixelAddress( int x, int y ) throw();
	COLORREF GetPixel( int x, int y ) const throw();
	LONG GetTransparentColor() const throw();
	int GetWidth() const throw();
	bool IsDIBSection() const throw();
	bool IsIndexed() const throw();
	bool IsNull() const throw();
	HRESULT Load( LPCTSTR pszFileName, DWORD dwFormats = fmtAny, 
		DWORD dwPaletteOptions = palOptimal|palOverwriteSystemColors|
		palDitherErrorDiffusion ) throw();
	void LoadFromResource( HINSTANCE hInstance, LPCTSTR pszResourceName ) throw();
	void LoadFromResource( HINSTANCE hInstance, UINT nIDResource ) throw();
	BOOL MaskBlt( HDC hDestDC, int xDest, int yDest, int nDestWidth, 
		int nDestHeight, int xSrc, int ySrc, HBITMAP hbmMask, int xMask, 
		int yMask, DWORD dwROP = SRCCOPY ) const throw();
	BOOL MaskBlt( HDC hDestDC, const RECT& rectDest, const POINT& pointSrc, 
		HBITMAP hbmMask, const POINT& pointMask, DWORD dwROP = SRCCOPY ) const throw();
	BOOL MaskBlt( HDC hDestDC, int xDest, int yDest, HBITMAP hbmMask, 
		DWORD dwROP = SRCCOPY ) const throw();
	BOOL MaskBlt( HDC hDestDC, const POINT& pointDest, HBITMAP hbmMask, 
		DWORD dwROP = SRCCOPY ) const throw();
	BOOL PlgBlt( HDC hDestDC, const POINT* pPoints, HBITMAP hbmMask = NULL ) const throw();
	BOOL PlgBlt( HDC hDestDC, const POINT* pPoints, int xSrc, int ySrc, 
		int nSrcWidth, int nSrcHeight, HBITMAP hbmMask = NULL, int xMask = 0, 
		int yMask = 0 ) const throw();
	BOOL PlgBlt( HDC hDestDC, const POINT* pPoints, const RECT& rectSrc, 
		HBITMAP hbmMask = NULL, const POINT& pointMask = CPoint( 0, 0 ) ) const throw();
	HRESULT PrepareSaveAdvanced( REFGUID guidFileType, IUnknown** ppExport ) const throw();
	void ReleaseDC() const throw();
	HRESULT Save( ISequentialStream* pStream, REFGUID guidFileType ) const throw();
	HRESULT Save( LPCTSTR pszFileName, REFGUID guidFileType ) const throw();
	HRESULT SaveAdvanced( ISequentialStream* pStream, IUnknown* punkExport ) const throw();
	HRESULT SaveAdvanced( LPCTSTR pszFileName, IUnknown* punkExport ) const throw();
	void SetColorTable( UINT iFirstColor, UINT nColors, 
		const RGBQUAD* prgbColors ) throw();
	void SetPixel( int x, int y, COLORREF color ) throw();
	void SetPixelIndexed( int x, int y, int iIndex ) throw();
	void SetPixelRGB( int x, int y, BYTE r, BYTE g, BYTE b ) throw();
	LONG SetTransparentColor( LONG iTransparentColor ) throw();
	BOOL StretchBlt( HDC hDestDC, int xDest, int yDest, int nDestWidth, 
		int nDestHeight, DWORD dwROP = SRCCOPY ) const throw();
	BOOL StretchBlt( HDC hDestDC, const RECT& rectDest, DWORD dwROP = SRCCOPY ) const throw();
	BOOL StretchBlt( HDC hDestDC, int xDest, int yDest, int nDestWidth, 
		int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight,
		DWORD dwROP = SRCCOPY ) const throw();
	BOOL StretchBlt( HDC hDestDC, const RECT& rectDest, const RECT& rectSrc,
		DWORD dwROP = SRCCOPY ) const throw();
#if WINVER >= 0x0500
	BOOL TransparentBlt( HDC hDestDC, int xDest, int yDest, int nDestWidth, 
		int nDestHeight, UINT crTransparent = CLR_INVALID ) const throw();
	BOOL TransparentBlt( HDC hDestDC, const RECT& rectDest, 
		UINT crTransparent = CLR_INVALID ) const throw();
	BOOL TransparentBlt( HDC hDestDC, int xDest, int yDest, int nDestWidth,
		int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight,
		UINT crTransparent = CLR_INVALID ) const throw();
	BOOL TransparentBlt( HDC hDestDC, const RECT& rectDest, const RECT& rectSrc,
		UINT crTransparent = CLR_INVALID ) const throw();
#endif  // WINVER >= 0x0500

	static BOOL IsTransparencySupported() throw();

private:
	HBITMAP m_hBitmap;
	void* m_pBits;
	int m_nWidth;
	int m_nHeight;
	int m_nPitch;
	int m_nBPP;
	bool m_bIsDIBSection;
	bool m_bHasAlphaChannel;
	LONG m_iTransparentColor;

// Implementation
private:
	void UpdateBitmapInfo( DIBOrientation eOrientation );

	static int ComputePitch( int nWidth, int nBPP )
	{
		return( (((nWidth*nBPP)+31)/32)*4 );
	}
	static void GenerateHalftonePalette( LPRGBQUAD prgbPalette );
	COLORREF GetTransparentRGB() const;

private:
	mutable HDC m_hDC;
	mutable int m_nDCRefCount;
	mutable HBITMAP m_hOldBitmap;

	static CDCCache s_cache;
};

inline CImage::CDCCache::CDCCache()
{
	int iDC;

	for( iDC = 0; iDC < CIMAGE_DC_CACHE_SIZE; iDC++ )
	{
		m_ahDCs[iDC] = NULL;
	}
}

inline CImage::CDCCache::~CDCCache()
{
	int iDC;

	for( iDC = 0; iDC < CIMAGE_DC_CACHE_SIZE; iDC++ )
	{
		if( m_ahDCs[iDC] != NULL )
		{
			::DeleteDC( m_ahDCs[iDC] );
		}
	}
}

inline HDC CImage::CDCCache::GetDC()
{
	HDC hDC;

	for( int iDC = 0; iDC < CIMAGE_DC_CACHE_SIZE; iDC++ )
	{
		hDC = static_cast< HDC >( InterlockedExchangePointer( reinterpret_cast< void** >(&m_ahDCs[iDC]), NULL ) );
		if( hDC != NULL )
		{
			return( hDC );
		}
	}

	hDC = ::CreateCompatibleDC( NULL );

	return( hDC );
}

inline void CImage::CDCCache::ReleaseDC( HDC hDC )
{
	for( int iDC = 0; iDC < CIMAGE_DC_CACHE_SIZE; iDC++ )
	{
		HDC hOldDC;

		hOldDC = static_cast< HDC >( InterlockedExchangePointer( reinterpret_cast< void** >(&m_ahDCs[iDC]), hDC ) );
		if( hOldDC == NULL )
		{
			return;
		}
		else
		{
			hDC = hOldDC;
		}
	}
	if( hDC != NULL )
	{
		::DeleteDC( hDC );
	}
}

inline CImage::CImage() :
	m_hBitmap( NULL ),
	m_pBits( NULL ),
	m_hDC( NULL ),
	m_nDCRefCount( 0 ),
	m_hOldBitmap( NULL ),
	m_nWidth( 0 ),
	m_nHeight( 0 ),
	m_nPitch( 0 ),
	m_nBPP( 0 ),
	m_iTransparentColor( -1 ),
	m_bHasAlphaChannel( false ),
	m_bIsDIBSection( false )
{
}

inline CImage::~CImage()
{
	Destroy();
}

inline CImage::operator HBITMAP() const
{
	return( m_hBitmap );
}

#if WINVER >= 0x0500
inline BOOL CImage::AlphaBlend( HDC hDestDC, int xDest, int yDest, 
	BYTE bSrcAlpha, BYTE bBlendOp ) const
{
	return( AlphaBlend( hDestDC, xDest, yDest, m_nWidth, m_nHeight, 0, 0, 
		m_nWidth, m_nHeight, bSrcAlpha, bBlendOp ) );
}

inline BOOL CImage::AlphaBlend( HDC hDestDC, const POINT& pointDest, 
   BYTE bSrcAlpha, BYTE bBlendOp ) const
{
	return( AlphaBlend( hDestDC, pointDest.x, pointDest.y, m_nWidth, m_nHeight, 
		0, 0, m_nWidth, m_nHeight, bSrcAlpha, bBlendOp ) );
}

inline BOOL CImage::AlphaBlend( HDC hDestDC, int xDest, int yDest, 
	int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, 
	int nSrcHeight, BYTE bSrcAlpha, BYTE bBlendOp ) const
{
	BLENDFUNCTION blend;
	BOOL bResult;

	blend.SourceConstantAlpha = bSrcAlpha;
	blend.BlendOp = bBlendOp;
	blend.BlendFlags = 0;
	if( m_bHasAlphaChannel )
	{
		blend.AlphaFormat = AC_SRC_ALPHA;
	}
	else
	{
		blend.AlphaFormat = 0;
	}

	GetDC();

	bResult = ::AlphaBlend( hDestDC, xDest, yDest, nDestWidth, nDestHeight, m_hDC, 
		xSrc, ySrc, nSrcWidth, nSrcHeight, blend );

	ReleaseDC();

	return( bResult );
}

inline BOOL CImage::AlphaBlend( HDC hDestDC, const RECT& rectDest, 
	const RECT& rectSrc, BYTE bSrcAlpha, BYTE bBlendOp ) const
{
	return( AlphaBlend( hDestDC, rectDest.left, rectDest.top, rectDest.right-
		rectDest.left, rectDest.bottom-rectDest.top, rectSrc.left, rectSrc.top, 
		rectSrc.right-rectSrc.left, rectSrc.bottom-rectSrc.top, bSrcAlpha, 
		bBlendOp ) );
}
#endif  // WINVER >= 0x0500

inline void CImage::Attach( HBITMAP hBitmap, DIBOrientation eOrientation )
{
	ATLASSERT( m_hBitmap == NULL );
	ATLASSERT( hBitmap != NULL );

	m_hBitmap = hBitmap;

	UpdateBitmapInfo( eOrientation );
}

inline BOOL CImage::BitBlt( HDC hDestDC, int xDest, int yDest, DWORD dwROP ) const
{
	return( BitBlt( hDestDC, xDest, yDest, m_nWidth, m_nHeight, 0, 0, dwROP ) );
}

inline BOOL CImage::BitBlt( HDC hDestDC, const POINT& pointDest, DWORD dwROP ) const
{
	return( BitBlt( hDestDC, pointDest.x, pointDest.y, m_nWidth, m_nHeight,
		0, 0, dwROP ) );
}

inline BOOL CImage::BitBlt( HDC hDestDC, int xDest, int yDest, int nDestWidth, 
	int nDestHeight, int xSrc, int ySrc, DWORD dwROP ) const
{
	BOOL bResult;

	ATLASSERT( m_hBitmap != NULL );
	ATLASSERT( hDestDC != NULL );

	GetDC();

	bResult = ::BitBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight, m_hDC, 
		xSrc, ySrc, dwROP );

	ReleaseDC();

	return( bResult );
}

inline BOOL CImage::BitBlt( HDC hDestDC, const RECT& rectDest, 
	const POINT& pointSrc, DWORD dwROP ) const
{
	return( BitBlt( hDestDC, rectDest.left, rectDest.top, rectDest.right-
		rectDest.left, rectDest.bottom-rectDest.top, pointSrc.x, pointSrc.y, 
		dwROP ) );
}

inline BOOL CImage::Create( int nWidth, int nHeight, int nBPP, DWORD dwFlags )
{
	LPBITMAPINFO pbmi;
	HBITMAP hBitmap;

	ATLASSERT( (nBPP == 32) || !(dwFlags&createAlphaChannel) );

	pbmi = LPBITMAPINFO( _alloca( sizeof( BITMAPINFO )+256*sizeof( 
	  RGBQUAD ) ) );

	memset( &pbmi->bmiHeader, 0, sizeof( pbmi->bmiHeader ) );
	pbmi->bmiHeader.biSize = sizeof( pbmi->bmiHeader );
	pbmi->bmiHeader.biWidth = nWidth;
	pbmi->bmiHeader.biHeight = nHeight;
	pbmi->bmiHeader.biPlanes = 1;
	pbmi->bmiHeader.biBitCount = USHORT( nBPP );
	if( nBPP <= 8 )
	{
		memset( pbmi->bmiColors, 0, 256*sizeof( RGBQUAD ) );
	}

	hBitmap = ::CreateDIBSection( NULL, pbmi, DIB_RGB_COLORS, &m_pBits, NULL,
		0 );
	if( hBitmap == NULL )
	{
		return( FALSE );
	}

	Attach( hBitmap, (nHeight < 0) ? DIBOR_TOPDOWN : DIBOR_BOTTOMUP );

	if( dwFlags&createAlphaChannel )
	{
		m_bHasAlphaChannel = true;
	}

	return( TRUE );
}

inline void CImage::Destroy()
{
	HBITMAP hBitmap;

	if( m_hBitmap != NULL )
	{
		hBitmap = Detach();
		::DeleteObject( hBitmap );
	}
}

inline HBITMAP CImage::Detach()
{
	HBITMAP hBitmap;

	ATLASSERT( m_hBitmap != NULL );
	ATLASSERT( m_hDC == NULL );

	hBitmap = m_hBitmap;
	m_hBitmap = NULL;
	m_pBits = NULL;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nBPP = 0;
	m_nPitch = 0;
	m_iTransparentColor = -1;
	m_bHasAlphaChannel = false;
	m_bIsDIBSection = false;

	return( hBitmap );
}

inline BOOL CImage::Draw( HDC hDestDC, const RECT& rectDest ) const
{
	return( Draw( hDestDC, rectDest.left, rectDest.top, rectDest.right-
		rectDest.left, rectDest.bottom-rectDest.top, 0, 0, m_nWidth, 
		m_nHeight ) );
}

inline BOOL CImage::Draw( HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight ) const
{
	return( Draw( hDestDC, xDest, yDest, nDestWidth, nDestHeight, 0, 0, m_nWidth, m_nHeight ) );
}

inline BOOL CImage::Draw( HDC hDestDC, const POINT& pointDest ) const
{
	return( Draw( hDestDC, pointDest.x, pointDest.y, m_nWidth, m_nHeight, 0, 0, m_nWidth, m_nHeight ) );
}

inline BOOL CImage::Draw( HDC hDestDC, int xDest, int yDest ) const
{
	return( Draw( hDestDC, xDest, yDest, m_nWidth, m_nHeight, 0, 0, m_nWidth, m_nHeight ) );
}

inline BOOL CImage::Draw( HDC hDestDC, const RECT& rectDest, const RECT& rectSrc ) const
{
	return( Draw( hDestDC, rectDest.left, rectDest.top, rectDest.right-
		rectDest.left, rectDest.bottom-rectDest.top, rectSrc.left, rectSrc.top, 
		rectSrc.right-rectSrc.left, rectSrc.bottom-rectSrc.top ) );
}

inline BOOL CImage::Draw( HDC hDestDC, int xDest, int yDest, int nDestWidth,
	int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight ) const
{
	BOOL bResult;

	ATLASSERT( m_hBitmap != NULL );
	ATLASSERT( hDestDC != NULL );
	ATLASSERT( nDestWidth > 0 );
	ATLASSERT( nDestHeight > 0 );
	ATLASSERT( nSrcWidth > 0 );
	ATLASSERT( nSrcHeight > 0 );

	GetDC();

#if WINVER >= 0x0500
	if( (m_iTransparentColor != -1) && IsTransparencySupported() )
	{
		bResult = ::TransparentBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight,
			m_hDC, xSrc, ySrc, nSrcWidth, nSrcHeight, GetTransparentRGB() );
	}
	else if( m_bHasAlphaChannel && IsTransparencySupported() )
	{
		BLENDFUNCTION bf;

		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = 0xff;
		bf.AlphaFormat = AC_SRC_ALPHA;
		bResult = ::AlphaBlend( hDestDC, xDest, yDest, nDestWidth, nDestHeight, 
			m_hDC, xSrc, ySrc, nSrcWidth, nSrcHeight, bf );
	}
	else
#endif  // WINVER >= 0x0500
	{
		bResult = ::StretchBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight, 
			m_hDC, xSrc, ySrc, nSrcWidth, nSrcHeight, SRCCOPY );
	}

	ReleaseDC();

	return( bResult );
}

inline const void* CImage::GetBits() const
{
	ATLASSERT( m_hBitmap != NULL );
	ATLASSERT( IsDIBSection() );

	return( m_pBits );
}

inline void* CImage::GetBits()
{
	ATLASSERT( m_hBitmap != NULL );
	ATLASSERT( IsDIBSection() );

	return( m_pBits );
}

inline int CImage::GetBPP() const
{
	ATLASSERT( m_hBitmap != NULL );

	return( m_nBPP );
}

inline void CImage::GetColorTable( UINT iFirstColor, UINT nColors, 
	RGBQUAD* prgbColors ) const
{
	ATLASSERT( m_hBitmap != NULL );
	ATLASSERT( m_pBits != NULL );
	ATLASSERT( IsIndexed() );

	GetDC();

	::GetDIBColorTable( m_hDC, iFirstColor, nColors, prgbColors );

	ReleaseDC();
}

inline HDC CImage::GetDC() const
{
	ATLASSERT( m_hBitmap != NULL );

	m_nDCRefCount++;
	if( m_hDC == NULL )
	{
		m_hDC = s_cache.GetDC();
		m_hOldBitmap = HBITMAP( ::SelectObject( m_hDC, m_hBitmap ) );
	}

	return( m_hDC );
}

inline HRESULT CImage::GetExporterFilterString( CSimpleString& strExporters, 
	CSimpleArray< GUID >& aguidFileTypes, TCHAR chSeparator ) const
{
	USES_CONVERSION;
	CComQIPtr< IBMGraphManager > pManager;
	CComQIPtr< IEnumBMFileTypeInfo > pEnum;
	HRESULT hResult;
	int nExporters;
	ULONG nExtensions;

	aguidFileTypes.RemoveAll();

	hResult = pManager.CoCreateInstance( __uuidof( BMGraphManager ) );
	if( FAILED( hResult ) )
	{
		return( hResult );
	}

	hResult = pManager->EnumFileTypes( &pEnum );
	if( FAILED( hResult ) )
	{
		return( hResult );
	}

	nExporters = 0;
	do
	{
		CComQIPtr< IBMFileTypeInfo > pFileTypeInfo;

		hResult = pEnum->Next( 1, &pFileTypeInfo, NULL );
		if( hResult == S_OK )
		{
			GUID guidFileType;
			CComHeapPtr< OLECHAR > pszDescription;

			hResult = pFileTypeInfo->GetDescription( GetUserDefaultLCID(), 
				&pszDescription );
			if( FAILED( hResult ) )
			{
				return( hResult );
			}
			strExporters += OLE2CT( pszDescription );
			strExporters += chSeparator;

			pFileTypeInfo->GetNumExtensions( &nExtensions );
			for( ULONG iExtension = 0; iExtension < nExtensions; iExtension++ )
			{
				CComHeapPtr< OLECHAR > pszExtension;

				if( iExtension > 0 )
				{
					strExporters += _T( ";" );
				}

				hResult = pFileTypeInfo->GetExtension( iExtension, &pszExtension );
				if( FAILED( hResult ) )
				{
					return( hResult );
				}

				strExporters += _T( "*" );
				strExporters += OLE2CT( pszExtension );
			}

			pFileTypeInfo->GetGUID( &guidFileType );
			aguidFileTypes.Add( guidFileType );

			nExporters++;
		}
		strExporters += chSeparator;
	} while( hResult == S_OK );

	if( nExporters > 0 )
	{
		strExporters += chSeparator;
	}
	else
	{
		strExporters += chSeparator;
		strExporters += chSeparator;
	}

	return( S_OK );
}

inline int CImage::GetHeight() const
{
	ATLASSERT( m_hBitmap != NULL );

	return( m_nHeight );
}

inline int CImage::GetMaxColorTableEntries() const
{
	ATLASSERT( m_hBitmap != NULL );
	ATLASSERT( IsDIBSection() );

	if( IsIndexed() )
	{
		return( 1<<m_nBPP );
	}
	else
	{
		return( 0 );
	}
}

inline int CImage::GetPitch() const
{
	ATLASSERT( m_hBitmap != NULL );
	ATLASSERT( IsDIBSection() );

	return( m_nPitch );
}

inline COLORREF CImage::GetPixel( int x, int y ) const
{
	ATLASSERT( m_hBitmap != NULL );
	ATLASSERT( (x >= 0) && (x < m_nWidth) );
	ATLASSERT( (y >= 0) && (y < m_nHeight) );

	GetDC();

	COLORREF clr = ::GetPixel( m_hDC, x, y );

	ReleaseDC();

	return( clr );
}

inline const void* CImage::GetPixelAddress( int x, int y ) const
{
	ATLASSERT( m_hBitmap != NULL );
	ATLASSERT( IsDIBSection() );
	ATLASSERT( (x >= 0) && (x < m_nWidth) );
	ATLASSERT( (y >= 0) && (y < m_nHeight) );

	return( LPBYTE( m_pBits )+(y*m_nPitch)+((x*m_nBPP)/8) );
}

inline void* CImage::GetPixelAddress( int x, int y )
{
	ATLASSERT( m_hBitmap != NULL );
	ATLASSERT( IsDIBSection() );
	ATLASSERT( (x >= 0) && (x < m_nWidth) );
	ATLASSERT( (y >= 0) && (y < m_nHeight) );

	return( LPBYTE( m_pBits )+(y*m_nPitch)+((x*m_nBPP)/8) );
}

inline LONG CImage::GetTransparentColor() const
{
	ATLASSERT( m_hBitmap != NULL );
	ATLASSERT( (m_nBPP == 4) || (m_nBPP == 8) );

	return( m_iTransparentColor );
}

inline int CImage::GetWidth() const
{
	ATLASSERT( m_hBitmap != NULL );

	return( m_nWidth );
}

inline bool CImage::IsDIBSection() const
{
	return( m_bIsDIBSection );
}

inline bool CImage::IsIndexed() const
{
	ATLASSERT( m_hBitmap != NULL );
	ATLASSERT( IsDIBSection() );

	return( m_nBPP <= 8 );
}

inline bool CImage::IsNull() const throw()
{
	return( m_hBitmap == NULL );
}

inline HRESULT CImage::Load( LPCTSTR pszFileName, DWORD dwFormats, DWORD dwPaletteOptions )
{
	USES_CONVERSION;
	CComQIPtr< ISequentialStream > pStream;
	CComQIPtr< ISequentialStream > pSniffStream;
	CComQIPtr< IBitmapNotify > pNotify;
	CComQIPtr< IStdBitmapNotify > pStdNotify;
	HRESULT hResult;
	CComQIPtr< IBitmapImport > pImport;
	CComQIPtr< IBitmapSource > pSource;
	CComQIPtr< IBitmapTarget > pTarget;
	CComQIPtr< IDIBTarget > pDIBTarget;
	CComQIPtr< IBMGraphManager > pGraph;
	HBITMAP hBitmap;
	CStreamOnFILE* pStreamOnFILE;
	HANDLE hFile;
	LPCTSTR pszExtension;
	LPCOLESTR pszExtensionO;
	IRGBPALETTEUSAGE usage;
	RGBQUAD argbPalette[256];

	hResult = pGraph.CoCreateInstance( __uuidof( BMGraphManager ) );
	if( FAILED( hResult ) )
	{
		return( hResult );
	}

	hResult = pNotify.CoCreateInstance( __uuidof( StdBitmapNotify ), NULL, CLSCTX_INPROC_SERVER );
	if( FAILED( hResult ) )
	{
		return( hResult );
	}
	pStdNotify = pNotify;
	if( pStdNotify == NULL )
	{
		return( E_NOINTERFACE );
	}

	switch( dwPaletteOptions&palDitherOptions )
	{
	case palDitherNone:
		pStdNotify->SetDitherMode( BMDITHER_NONE );
		break;

	case palDitherErrorDiffusion:
		pStdNotify->SetDitherMode( BMDITHER_ERRORDIFFUSION );
		break;

	default:
		ATLASSERT( false );
		break;
	}

	pStdNotify->SetAlphaRemoveMode( BMALPHAREMOVE_BLEND );

	hFile = ::CreateFile( pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, 
		OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hFile == INVALID_HANDLE_VALUE )
	{
		return( E_FAIL );
	}

	pStreamOnFILE = NULL;
	ATLTRY(pStreamOnFILE = new CStreamOnFILE( hFile ));
	if( pStreamOnFILE == NULL )
	{
		::CloseHandle( hFile );
		return( E_OUTOFMEMORY );
	}
	hFile = NULL;

	pStreamOnFILE->QueryInterface( __uuidof(ISequentialStream), (void**)&pStream );
	ATLASSERT( pStream != NULL );
	pStreamOnFILE = NULL;

	pszExtension = ::PathFindExtension( pszFileName );
	if( *pszExtension == _T( '\0' ) )
	{
		pszExtensionO = NULL;
	}
	else
	{
		ATLASSERT( *pszExtension == _T( '.' ) );
		pszExtensionO = T2COLE( pszExtension );
	}
	hResult = pGraph->CreateImporter( NULL, pszExtensionO, pStream, &pImport, 
		&pSniffStream );
	if( FAILED( hResult ) )
	{
		return( hResult );
	}

	pSource = pImport;

	hResult = pTarget.CoCreateInstance( __uuidof( DIBTarget ) );
	if( FAILED( hResult ) )
	{
		return( hResult );
	}
	pDIBTarget = pTarget;
	if( pDIBTarget == NULL )
	{
		return( E_NOINTERFACE );
	}

	pDIBTarget->SetSupportedFormats( dwFormats );
	switch( dwPaletteOptions&palPalGenOptions )
	{
	case palOptimal:
		switch( dwPaletteOptions&palSystemColorOptions )
		{
		case palAvoidSystemColors:
			usage.iFirstAvailableColor = 10;
			usage.nAvailableColors = 236;
			usage.iFirstWritableColor = 10;
			usage.nWritableColors = 236;
			pDIBTarget->SetCustomPaletteUsage( DIBTARGET_8BPP, &usage );
			break;

		case palUseSystemColors:
			usage.iFirstAvailableColor = 0;
			usage.nAvailableColors = 256;
			usage.iFirstWritableColor = 10;
			usage.nWritableColors = 236;
			pDIBTarget->SetCustomPaletteUsage( DIBTARGET_8BPP, &usage );
			break;

		case palOverwriteSystemColors:
			usage.iFirstAvailableColor = 0;
			usage.nAvailableColors = 256;
			usage.iFirstWritableColor = 0;
			usage.nWritableColors = 256;
			pDIBTarget->SetCustomPaletteUsage( DIBTARGET_8BPP, &usage );
			break;

		default:
			ATLASSERT( false );
			break;
		}
		break;

	case palHalftone:
		GenerateHalftonePalette( argbPalette );
		usage.iFirstAvailableColor = 10;
		usage.nAvailableColors = 236;
		usage.iFirstWritableColor = 0;
		usage.nWritableColors = 0;
		pDIBTarget->SetCustomPaletteUsage( DIBTARGET_8BPP, &usage );
		pDIBTarget->SetCustomPalette( 10, 236, argbPalette );
		break;

	default:
		ATLASSERT( false );
		break;
	}

	pSource->JoinGraph( pGraph, pNotify );
	pSource->SetTarget( pTarget );
	hResult = pImport->Import( pSniffStream );
	if( FAILED( hResult ) )
	{
		return( hResult );
	}

	pDIBTarget->GetDIB( (void**)&hBitmap );
	pDIBTarget->ReleaseDIB();

	pStream.Release();
	pSniffStream.Release();

	Attach( hBitmap );

	pDIBTarget->GetTransparentColor( &m_iTransparentColor );
	if( pDIBTarget->HasAlphaChannel() == S_OK )
	{
		m_bHasAlphaChannel = true;
	}

	return( S_OK );
}

inline void CImage::LoadFromResource( HINSTANCE hInstance, LPCTSTR pszResourceName )
{
	HBITMAP hBitmap;

	hBitmap = HBITMAP( ::LoadImage( hInstance, pszResourceName, IMAGE_BITMAP, 0, 
		0, LR_CREATEDIBSECTION ) );

	Attach( hBitmap );
}

inline void CImage::LoadFromResource( HINSTANCE hInstance, UINT nIDResource )
{
	LoadFromResource( hInstance, MAKEINTRESOURCE( nIDResource ) );
}

inline BOOL CImage::MaskBlt( HDC hDestDC, int xDest, int yDest, int nWidth, 
	int nHeight, int xSrc, int ySrc, HBITMAP hbmMask, int xMask, int yMask,
	DWORD dwROP ) const
{
	BOOL bResult;

	ATLASSERT( m_hBitmap != NULL );
	ATLASSERT( hDestDC != NULL );

	GetDC();

	bResult = ::MaskBlt( hDestDC, xDest, yDest, nWidth, nHeight, m_hDC, xSrc, 
		ySrc, hbmMask, xMask, yMask, dwROP );

	ReleaseDC();

	return( bResult );
}

inline BOOL CImage::MaskBlt( HDC hDestDC, const RECT& rectDest, 
	const POINT& pointSrc, HBITMAP hbmMask, const POINT& pointMask, 
	DWORD dwROP ) const
{
	return( MaskBlt( hDestDC, rectDest.left, rectDest.top, rectDest.right-
		rectDest.left, rectDest.bottom-rectDest.top, pointSrc.x, pointSrc.y, 
		hbmMask, pointMask.x, pointMask.y, dwROP ) );
}

inline BOOL CImage::MaskBlt( HDC hDestDC, int xDest, int yDest, HBITMAP hbmMask, 
	DWORD dwROP ) const
{
	return( MaskBlt( hDestDC, xDest, yDest, m_nWidth, m_nHeight, 0, 0, hbmMask, 
		0, 0, dwROP ) );
}

inline BOOL CImage::MaskBlt( HDC hDestDC, const POINT& pointDest, HBITMAP hbmMask,
	DWORD dwROP ) const
{
	return( MaskBlt( hDestDC, pointDest.x, pointDest.y, m_nWidth, m_nHeight, 0, 
		0, hbmMask, 0, 0, dwROP ) );
}

inline BOOL CImage::PlgBlt( HDC hDestDC, const POINT* pPoints, int xSrc, 
	int ySrc, int nSrcWidth, int nSrcHeight, HBITMAP hbmMask, int xMask, 
	int yMask ) const
{
	BOOL bResult;

	ATLASSERT( m_hBitmap != NULL );
	ATLASSERT( hDestDC != NULL );

	GetDC();

	bResult = ::PlgBlt( hDestDC, pPoints, m_hDC, xSrc, ySrc, nSrcWidth, 
		nSrcHeight, hbmMask, xMask, yMask );

	ReleaseDC();

	return( bResult );
}

inline BOOL CImage::PlgBlt( HDC hDestDC, const POINT* pPoints, 
	const RECT& rectSrc, HBITMAP hbmMask, const POINT& pointMask ) const
{
	return( PlgBlt( hDestDC, pPoints, rectSrc.left, rectSrc.top, rectSrc.right-
		rectSrc.left, rectSrc.bottom-rectSrc.top, hbmMask, pointMask.x, 
		pointMask.y ) );
}

inline BOOL CImage::PlgBlt( HDC hDestDC, const POINT* pPoints, 
	HBITMAP hbmMask ) const
{
	return( PlgBlt( hDestDC, pPoints, 0, 0, m_nWidth, m_nHeight, hbmMask, 0, 
		0 ) );
}

inline HRESULT CImage::PrepareSaveAdvanced( REFGUID guidFileType, 
	IUnknown** ppunkExport ) const
{
	HRESULT hResult;
	CComQIPtr< IBMGraphManager > pGraph;
	CComQIPtr< IBitmapExport > pExport;

	ATLASSERT( ppunkExport != NULL );
	if (ppunkExport == NULL)
		return E_POINTER;
	*ppunkExport = NULL;

	hResult = pGraph.CoCreateInstance( __uuidof( BMGraphManager ) );
	if( FAILED( hResult ) )
	{
		return( hResult );
	}

	hResult = pGraph->CreateExporter( guidFileType, &pExport );
	if( FAILED( hResult ) )
	{
		return( hResult );
	}

	*ppunkExport = pExport.Detach();  // Transfer refcount

	return( S_OK );
}

inline void CImage::ReleaseDC() const
{
	HBITMAP hBitmap;

	ATLASSERT( m_hDC != NULL );

	m_nDCRefCount--;
	if( m_nDCRefCount == 0 )
	{
		hBitmap = HBITMAP( ::SelectObject( m_hDC, m_hOldBitmap ) );
		ATLASSERT( hBitmap == m_hBitmap );
		s_cache.ReleaseDC( m_hDC );
		m_hDC = NULL;
	}
}

inline HRESULT CImage::Save( ISequentialStream* pStream, REFGUID guidFileType ) const
{
	HRESULT hResult;
	CComQIPtr< IBitmapExport > pExport;
	CComQIPtr< IPersistStreamInit > pPSI;
	CComPtr< IUnknown > punkExport;

	hResult = PrepareSaveAdvanced( guidFileType, &punkExport );
	if( FAILED( hResult ) )
	{
		return( hResult );
	}
	pExport = punkExport;
	ATLASSERT( pExport != NULL );

	pPSI = pExport;
	if( pPSI != NULL )
	{
		hResult = pPSI->InitNew();
		if( FAILED( hResult ) )
		{
			return( hResult );
		}
	}

	hResult = SaveAdvanced( pStream, pExport );
	if( FAILED( hResult ) )
	{
		return( hResult );
	}

	return( S_OK );
}

inline HRESULT CImage::Save( LPCTSTR pszFileName, REFGUID guidFileType ) const
{
	HRESULT hResult;
	CComQIPtr< IBitmapExport > pExport;
	CComQIPtr< IPersistStreamInit > pPSI;
	CComPtr< IUnknown > punkExport;

	hResult = PrepareSaveAdvanced( guidFileType, &punkExport );
	if( FAILED( hResult ) )
	{
		return( hResult );
	}
	pExport = punkExport;
	ATLASSERT( pExport != NULL );

	pPSI = pExport;
	if( pPSI != NULL )
	{
		hResult = pPSI->InitNew();
		if( FAILED( hResult ) )
		{
			return( hResult );
		}
	}

	hResult = SaveAdvanced( pszFileName, pExport );
	if( FAILED( hResult ) )
	{
		return( hResult );
	}

	return( S_OK );
}

inline HRESULT CImage::SaveAdvanced( LPCTSTR pszFileName, IUnknown* punkExport ) const
{
	CComQIPtr< ISequentialStream > pStream;
	HANDLE hFile;
	CStreamOnFILE* pStreamOnFILE;

	hFile = ::CreateFile( pszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( hFile == INVALID_HANDLE_VALUE )
	{
		return( E_FAIL );
	}

	pStreamOnFILE = NULL;
	ATLTRY(pStreamOnFILE = new CStreamOnFILE( hFile ));
	if( pStreamOnFILE == NULL )
	{
		::CloseHandle( hFile );
		return( E_OUTOFMEMORY );
	}
	hFile = NULL;

	pStreamOnFILE->QueryInterface( __uuidof(ISequentialStream), (void**)&pStream );
	ATLASSERT( pStream != NULL );
	pStreamOnFILE = NULL;

	return( SaveAdvanced( pStream, punkExport ) );
}

inline HRESULT CImage::SaveAdvanced( ISequentialStream* pStream, IUnknown* punkExport ) const
{
	USES_CONVERSION;
	CComQIPtr< IBitmapExport > pExport;
	CComQIPtr< IBitmapNotify > pNotify;
	CComQIPtr< IStdBitmapNotify > pStdNotify;
	HRESULT hResult;
	CComQIPtr< IBitmapSource > pSource;
	CComQIPtr< IBitmapTarget > pTarget;
	CComQIPtr< IDIBSource > pDIBSource;
	CComQIPtr< IBMGraphManager > pGraph;
	LPBITMAPINFO pBitmapInfo;
	void* pBits;
	DWORD dwFlags;

	pExport = punkExport;
	if( pExport == NULL )
	{
		return( E_NOINTERFACE );
	}

	hResult = pGraph.CoCreateInstance( __uuidof( BMGraphManager ) );
	if( FAILED( hResult ) )
	{
		return( hResult );
	}

	hResult = pNotify.CoCreateInstance( __uuidof( StdBitmapNotify ) );
	if( FAILED( hResult ) )
	{
		return( hResult );
	}
	pStdNotify = pNotify;
	if( pStdNotify == NULL )
	{
		return( E_NOINTERFACE );
	}

	DWORD dwPaletteOptions = palDitherNone;

	switch( dwPaletteOptions&palDitherOptions )
	{
	case palDitherNone:
		pStdNotify->SetDitherMode( BMDITHER_NONE );
		break;

	case palDitherErrorDiffusion:
		pStdNotify->SetDitherMode( BMDITHER_ERRORDIFFUSION );
		break;

	default:
		ATLASSERT( false );
		break;
	}

	pStdNotify->SetAlphaRemoveMode( BMALPHAREMOVE_BLEND );

	hResult = pSource.CoCreateInstance( __uuidof( DIBSource ) );
	if( FAILED( hResult ) )
	{
		return( hResult );
	}
	pDIBSource = pSource;
	if( pDIBSource == NULL )
	{
		return( E_NOINTERFACE );
	}

	pTarget = pExport;
	if( pTarget == NULL )
	{
		return( E_NOINTERFACE );
	}

	pSource->JoinGraph( pGraph, pNotify );
	pSource->SetTarget( pTarget );
	pExport->SetDestination( pStream );

	pBitmapInfo = LPBITMAPINFO( _alloca( sizeof( BITMAPINFO )+256*sizeof( RGBQUAD ) ) );
	memset( &pBitmapInfo->bmiHeader, 0, sizeof( pBitmapInfo->bmiHeader ) );
	pBitmapInfo->bmiHeader.biSize = sizeof( pBitmapInfo->bmiHeader );
	if( !m_bIsDIBSection )
	{
		::GetDIBits( NULL, m_hBitmap, 0, 0, NULL, pBitmapInfo, DIB_RGB_COLORS );
		pBitmapInfo->bmiHeader.biHeight = abs( pBitmapInfo->bmiHeader.biHeight );
		ATLASSERT( pBitmapInfo->bmiHeader.biWidth == m_nWidth );
		ATLASSERT( pBitmapInfo->bmiHeader.biHeight == m_nHeight );
		pBits = NULL;
		ATLTRY(pBits = malloc( m_nHeight*ComputePitch( m_nWidth, pBitmapInfo->bmiHeader.biBitCount ) ));
		ATLASSERT(pBits != NULL);
		::GetDIBits( NULL, m_hBitmap, 0, m_nHeight, pBits, pBitmapInfo, DIB_RGB_COLORS );
	}
	else
	{
		pBitmapInfo->bmiHeader.biCompression = BI_RGB;
		pBitmapInfo->bmiHeader.biWidth = m_nWidth;
		if( m_nPitch < 0 )
		{
			pBitmapInfo->bmiHeader.biHeight = m_nHeight;
			pBits = LPBYTE( m_pBits )+(m_nPitch*(m_nHeight-1));
		}
		else
		{
			pBitmapInfo->bmiHeader.biHeight = -m_nHeight;
			pBits = m_pBits;
		}
		pBitmapInfo->bmiHeader.biBitCount = USHORT( m_nBPP );
		pBitmapInfo->bmiHeader.biPlanes = 1;
		pBitmapInfo->bmiHeader.biClrImportant = 0;

		if( IsIndexed() )
		{
			GetDC();

			pBitmapInfo->bmiHeader.biClrUsed = ::GetDIBColorTable( m_hDC, 0, 256, pBitmapInfo->bmiColors );

			ReleaseDC();
		}
		else
		{
			pBitmapInfo->bmiHeader.biClrUsed = 0;
		}
	}

	dwFlags = 0;
	if( m_bHasAlphaChannel )
	{
		dwFlags |= DIBSOURCE_ALPHA;
	}
	hResult = pDIBSource->Go( pBitmapInfo, dwFlags, pBits );
	if( !m_bIsDIBSection )
	{
		free( pBits );
	}
	if( FAILED( hResult ) )
	{
		return( hResult );
	}

	return( S_OK );
}

inline void CImage::SetColorTable( UINT iFirstColor, UINT nColors, 
	const RGBQUAD* prgbColors )
{
	ATLASSERT( m_hBitmap != NULL );
	ATLASSERT( IsDIBSection() );
	ATLASSERT( IsIndexed() );

	GetDC();

	::SetDIBColorTable( m_hDC, iFirstColor, nColors, prgbColors );

	ReleaseDC();
}

inline void CImage::SetPixel( int x, int y, COLORREF color )
{
	ATLASSERT( m_hBitmap != NULL );
	ATLASSERT( (x >= 0) && (x < m_nWidth) );
	ATLASSERT( (y >= 0) && (y < m_nHeight) );

	GetDC();

	::SetPixel( m_hDC, x, y, color );

	ReleaseDC();
}

inline void CImage::SetPixelIndexed( int x, int y, int iIndex )
{
	SetPixel( x, y, PALETTEINDEX( iIndex ) );
}

inline void CImage::SetPixelRGB( int x, int y, BYTE r, BYTE g, BYTE b )
{
	SetPixel( x, y, RGB( r, g, b ) );
}

inline LONG CImage::SetTransparentColor( LONG iTransparentColor )
{
	LONG iOldTransparentColor;

	ATLASSERT( m_hBitmap != NULL );
	ATLASSERT( (m_nBPP == 4) || (m_nBPP == 8) );
	ATLASSERT( iTransparentColor < GetMaxColorTableEntries() );
	ATLASSERT( iTransparentColor >= -1 );

	iOldTransparentColor = m_iTransparentColor;
	m_iTransparentColor = iTransparentColor;

	return( iOldTransparentColor );
}

inline BOOL CImage::StretchBlt( HDC hDestDC, int xDest, int yDest, 
	int nDestWidth, int nDestHeight, DWORD dwROP ) const
{
	return( StretchBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight, 0, 0, 
		m_nWidth, m_nHeight, dwROP ) );
}

inline BOOL CImage::StretchBlt( HDC hDestDC, const RECT& rectDest, 
	DWORD dwROP ) const
{
	return( StretchBlt( hDestDC, rectDest.left, rectDest.top, rectDest.right-
		rectDest.left, rectDest.bottom-rectDest.top, 0, 0, m_nWidth, m_nHeight, 
		dwROP ) );
}

inline BOOL CImage::StretchBlt( HDC hDestDC, int xDest, int yDest, 
	int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, 
	int nSrcHeight, DWORD dwROP ) const
{
	BOOL bResult;

	ATLASSERT( m_hBitmap != NULL );
	ATLASSERT( hDestDC != NULL );

	GetDC();

	bResult = ::StretchBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight, m_hDC,
		xSrc, ySrc, nSrcWidth, nSrcHeight, dwROP );

	ReleaseDC();

	return( bResult );
}

inline BOOL CImage::StretchBlt( HDC hDestDC, const RECT& rectDest, 
	const RECT& rectSrc, DWORD dwROP ) const
{
	return( StretchBlt( hDestDC, rectDest.left, rectDest.top, rectDest.right-
		rectDest.left, rectDest.bottom-rectDest.top, rectSrc.left, rectSrc.top, 
		rectSrc.right-rectSrc.left, rectSrc.bottom-rectSrc.top, dwROP ) );
}

#if WINVER >= 0x0500
inline BOOL CImage::TransparentBlt( HDC hDestDC, int xDest, int yDest, 
	int nDestWidth, int nDestHeight, UINT crTransparent ) const
{
	return( TransparentBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight, 0, 
		0, m_nWidth, m_nHeight, crTransparent ) );
}

inline BOOL CImage::TransparentBlt( HDC hDestDC, const RECT& rectDest, 
	UINT crTransparent ) const
{
	return( TransparentBlt( hDestDC, rectDest.left, rectDest.top, 
		rectDest.right-rectDest.left, rectDest.bottom-rectDest.top, 
		crTransparent ) );
}

inline BOOL CImage::TransparentBlt( HDC hDestDC, int xDest, int yDest, 
	int nDestWidth, int nDestHeight, int xSrc, int ySrc, int nSrcWidth, 
	int nSrcHeight, UINT crTransparent ) const
{
	BOOL bResult;

	ATLASSERT( m_hBitmap != NULL );
	ATLASSERT( hDestDC != NULL );

	GetDC();

	if( crTransparent == CLR_INVALID )
	{
		crTransparent = GetTransparentRGB();
	}

	bResult = ::TransparentBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight,
		m_hDC, xSrc, ySrc, nSrcWidth, nSrcHeight, crTransparent );

	ReleaseDC();

	return( bResult );
}

inline BOOL CImage::TransparentBlt( HDC hDestDC, const RECT& rectDest, 
	const RECT& rectSrc, UINT crTransparent ) const
{
	return( TransparentBlt( hDestDC, rectDest.left, rectDest.top, 
		rectDest.right-rectDest.left, rectDest.bottom-rectDest.top, rectSrc.left, 
		rectSrc.top, rectSrc.right-rectSrc.left, rectSrc.bottom-rectSrc.top, 
		crTransparent ) );
}
#endif  // WINVER >= 0x0500

inline BOOL CImage::IsTransparencySupported()
{
#if WINVER >= 0x0500
	return( _AtlBaseModule.m_bNT5orWin98 );
#else  // WINVER < 0x0500
	return( FALSE );
#endif  // WINVER >= 0x0500
}

inline void CImage::UpdateBitmapInfo( DIBOrientation eOrientation )
{
	DIBSECTION dibsection;
	int nBytes;

	nBytes = ::GetObject( m_hBitmap, sizeof( DIBSECTION ), &dibsection );
	if( nBytes == sizeof( DIBSECTION ) )
	{
		m_bIsDIBSection = true;
		m_nWidth = dibsection.dsBmih.biWidth;
		m_nHeight = abs( dibsection.dsBmih.biHeight );
		m_nBPP = dibsection.dsBmih.biBitCount;
		m_nPitch = ComputePitch( m_nWidth, m_nBPP );
		m_pBits = dibsection.dsBm.bmBits;
		if( eOrientation == DIBOR_DEFAULT )
		{
			eOrientation = (dibsection.dsBmih.biHeight > 0) ? DIBOR_BOTTOMUP : DIBOR_TOPDOWN;
		}
		if( eOrientation == DIBOR_BOTTOMUP )
		{
			m_pBits = LPBYTE( m_pBits )+((m_nHeight-1)*m_nPitch);
			m_nPitch = -m_nPitch;
		}
	}
	else
	{
		// Non-DIBSection
		ATLASSERT( nBytes == sizeof( BITMAP ) );
		m_bIsDIBSection = false;
		m_nWidth = dibsection.dsBm.bmWidth;
		m_nHeight = dibsection.dsBm.bmHeight;
		m_nBPP = dibsection.dsBm.bmBitsPixel;
		m_nPitch = 0;
		m_pBits = 0;
	}
	m_iTransparentColor = -1;
	m_bHasAlphaChannel = false;
}

inline void CImage::GenerateHalftonePalette( LPRGBQUAD prgbPalette )
{
	int r;
	int g;
	int b;
	int gray;
	LPRGBQUAD prgbEntry;

	prgbEntry = prgbPalette;
	for( r = 0; r < 6; r++ )
	{
		for( g = 0; g < 6; g++ )
		{
			for( b = 0; b < 6; b++ )
			{
				prgbEntry->rgbBlue = BYTE( b*255/5 );
				prgbEntry->rgbGreen = BYTE( g*255/5 );
				prgbEntry->rgbRed = BYTE( r*255/5 );
				prgbEntry->rgbReserved = 0;

				prgbEntry++;
			}
		}
	}

	for( gray = 0; gray < 20; gray++ )
	{
		prgbEntry->rgbBlue = BYTE( gray*255/20 );
		prgbEntry->rgbGreen = BYTE( gray*255/20 );
		prgbEntry->rgbRed = BYTE( gray*255/20 );
		prgbEntry->rgbReserved = 0;

		prgbEntry++;
	}
}

inline COLORREF CImage::GetTransparentRGB() const
{
	RGBQUAD rgb;

	ATLASSERT( m_hDC != NULL );  // Must have a DC
	ATLASSERT( m_iTransparentColor != -1 );

	::GetDIBColorTable( m_hDC, m_iTransparentColor, 1, &rgb );

	return( RGB( rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue ) );
}

};  // namespace ATL

#pragma pack(pop)

#endif  // __ATLIMAGE_H__
