// This is a part of the Active Template Library.
// Copyright (C) 1996-2000 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLPATH_H__
#define __ATLPATH_H__

#pragma once

#ifdef _AFX
#include <afxstr.h>
#else
#include <atlstr.h>
#endif

#ifndef _ATL_NO_DEFAULT_LIBS
#pragma comment(lib, "shlwapi.lib")
#endif  // !_ATL_NO_DEFAULT_LIBS

namespace ATL
{

namespace ATLPath
{

inline char* PathAddBackslash( char* pszPath )
{
	return ::PathAddBackslashA( pszPath );
}

inline wchar_t* PathAddBackslash( wchar_t* pszPath )
{
	return ::PathAddBackslashW( pszPath );
}

inline BOOL PathAddExtension( char* pszPath, const char* pszExtension )
{
	return ::PathAddExtensionA( pszPath, pszExtension );
}

inline BOOL PathAddExtension( wchar_t* pszPath, const wchar_t* pszExtension )
{
	return ::PathAddExtensionW( pszPath, pszExtension );
}

inline BOOL PathAppend( char* pszPath, const char* pszMore )
{
	return ::PathAppendA( pszPath, pszMore );
}

inline BOOL PathAppend( wchar_t* pszPath, const wchar_t* pszMore )
{
	return ::PathAppendW( pszPath, pszMore );
}

inline char* PathBuildRoot( char* pszPath, int iDrive )
{
	return ::PathBuildRootA( pszPath, iDrive );
}

inline wchar_t* PathBuildRoot( wchar_t* pszPath, int iDrive )
{
	return ::PathBuildRootW( pszPath, iDrive );
}

inline BOOL PathCanonicalize( char* pszDest, const char* pszSrc )
{
	return ::PathCanonicalizeA( pszDest, pszSrc );
}

inline BOOL PathCanonicalize( wchar_t* pszDest, const wchar_t* pszSrc )
{
	return ::PathCanonicalizeW( pszDest, pszSrc );
}

inline char* PathCombine( char* pszDest, const char* pszDir,
	const char* pszFile )
{
	return ::PathCombineA( pszDest, pszDir, pszFile );
}

inline wchar_t* PathCombine( wchar_t* pszDest, const wchar_t* pszDir,
	const wchar_t* pszFile )
{
	return ::PathCombineW( pszDest, pszDir, pszFile );
}

inline int PathCommonPrefix( const char* pszFile1, const char* pszFile2,
	char* pszDest )
{
	return ::PathCommonPrefixA( pszFile1, pszFile2, pszDest );
}

inline int PathCommonPrefix( const wchar_t* pszFile1, const wchar_t* pszFile2,
	wchar_t* pszDest )
{
	return ::PathCommonPrefixW( pszFile1, pszFile2, pszDest );
}

inline BOOL PathFileExists( const char* pszPath )
{
	return ::PathFileExistsA( pszPath );
}

inline BOOL PathFileExists( const wchar_t* pszPath )
{
	return ::PathFileExistsW( pszPath );
}

inline char* PathFindExtension( const char* pszPath )
{
	return ::PathFindExtensionA( pszPath );
}

inline wchar_t* PathFindExtension( const wchar_t* pszPath )
{
	return ::PathFindExtensionW( pszPath );
}

inline char* PathFindFileName( const char* pszPath )
{
	return ::PathFindFileNameA( pszPath );
}

inline wchar_t* PathFindFileName( const wchar_t* pszPath )
{
	return ::PathFindFileNameW( pszPath );
}

inline int PathGetDriveNumber( const char* pszPath )
{
	return ::PathGetDriveNumberA( pszPath );
}

inline int PathGetDriveNumber( const wchar_t* pszPath )
{
	return ::PathGetDriveNumberW( pszPath );
}

inline BOOL PathIsDirectory( const char* pszPath )
{
	return ::PathIsDirectoryA( pszPath );
}

inline BOOL PathIsDirectory( const wchar_t* pszPath )
{
	return ::PathIsDirectoryW( pszPath );
}

/*
inline BOOL PathIsDirectoryEmpty( const char* pszPath )
{
	return ::PathIsDirectoryEmptyA( pszPath );
}
*/

/*
inline BOOL PathIsDirectoryEmpty( const wchar_t* pszPath )
{
	return ::PathIsDirectoryEmptyW( pszPath );
}
*/

inline BOOL PathIsFileSpec( const char* pszPath )
{
	return ::PathIsFileSpecA( pszPath );
}

inline BOOL PathIsFileSpec( const wchar_t* pszPath )
{
	return ::PathIsFileSpecW( pszPath );
}

inline BOOL PathIsPrefix( const char* pszPrefix, const char* pszPath )
{
	return ::PathIsPrefixA( pszPrefix, pszPath );
}

inline BOOL PathIsPrefix( const wchar_t* pszPrefix, const wchar_t* pszPath )
{
	return ::PathIsPrefixW( pszPrefix, pszPath );
}

inline BOOL PathIsRelative( const char* pszPath )
{
	return ::PathIsRelativeA( pszPath );
}

inline BOOL PathIsRelative( const wchar_t* pszPath )
{
	return ::PathIsRelativeW( pszPath );
}

inline BOOL PathIsRoot( const char* pszPath )
{
	return ::PathIsRootA( pszPath );
}

inline BOOL PathIsRoot( const wchar_t* pszPath )
{
	return ::PathIsRootW( pszPath );
}

inline BOOL PathIsSameRoot( const char* pszPath1, const char* pszPath2 )
{
	return ::PathIsSameRootA( pszPath1, pszPath2 );
}

inline BOOL PathIsSameRoot( const wchar_t* pszPath1, const wchar_t* pszPath2 )
{
	return ::PathIsSameRootW( pszPath1, pszPath2 );
}

inline BOOL PathIsUNC( const char* pszPath )
{
	return ::PathIsUNCA( pszPath );
}

inline BOOL PathIsUNC( const wchar_t* pszPath )
{
	return ::PathIsUNCW( pszPath );
}

inline BOOL PathIsUNCServer( const char* pszPath )
{
	return ::PathIsUNCServerA( pszPath );
}

inline BOOL PathIsUNCServer( const wchar_t* pszPath )
{
	return ::PathIsUNCServerW( pszPath );
}

inline BOOL PathIsUNCServerShare( const char* pszPath )
{
	return ::PathIsUNCServerShareA( pszPath );
}

inline BOOL PathIsUNCServerShare( const wchar_t* pszPath )
{
	return ::PathIsUNCServerShareW( pszPath );
}

inline BOOL PathMakePretty( char* pszPath )
{
	return ::PathMakePrettyA( pszPath );
}

inline BOOL PathMakePretty( wchar_t* pszPath )
{
	return ::PathMakePrettyW( pszPath );
}

inline BOOL PathMatchSpec( const char* pszPath, const char* pszSpec )
{
	return ::PathMatchSpecA( pszPath, pszSpec );
}

inline BOOL PathMatchSpec( const wchar_t* pszPath, const wchar_t* pszSpec )
{
	return ::PathMatchSpecW( pszPath, pszSpec );
}

inline void PathQuoteSpaces( char* pszPath )
{
	::PathQuoteSpacesA( pszPath );
}

inline void PathQuoteSpaces( wchar_t* pszPath )
{
	::PathQuoteSpacesW( pszPath );
}

inline BOOL PathRelativePathTo( char* pszPath, const char* pszFrom,
	DWORD dwAttrFrom, const char* pszTo, DWORD dwAttrTo )
{
	return ::PathRelativePathToA( pszPath, pszFrom, dwAttrFrom, pszTo, dwAttrTo );
}

inline BOOL PathRelativePathTo( wchar_t* pszPath, const wchar_t* pszFrom,
	DWORD dwAttrFrom, const wchar_t* pszTo, DWORD dwAttrTo )
{
	return ::PathRelativePathToW( pszPath, pszFrom, dwAttrFrom, pszTo, dwAttrTo );
}

inline void PathRemoveArgs( char* pszPath )
{
	::PathRemoveArgsA( pszPath );
}

inline void PathRemoveArgs( wchar_t* pszPath )
{
	::PathRemoveArgsW( pszPath );
}

inline char* PathRemoveBackslash( char* pszPath )
{
	return ::PathRemoveBackslashA( pszPath );
}

inline wchar_t* PathRemoveBackslash( wchar_t* pszPath )
{
	return ::PathRemoveBackslashW( pszPath );
}

inline void PathRemoveBlanks( char* pszPath )
{
	::PathRemoveBlanksA( pszPath );
}

inline void PathRemoveBlanks( wchar_t* pszPath )
{
	::PathRemoveBlanksW( pszPath );
}

inline void PathRemoveExtension( char* pszPath )
{
	::PathRemoveExtensionA( pszPath );
}

inline void PathRemoveExtension( wchar_t* pszPath )
{
	::PathRemoveExtensionW( pszPath );
}

inline BOOL PathRemoveFileSpec( char* pszPath )
{
	return ::PathRemoveFileSpecA( pszPath );
}

inline BOOL PathRemoveFileSpec( wchar_t* pszPath )
{
	return ::PathRemoveFileSpecW( pszPath );
}

inline BOOL PathRenameExtension( char* pszPath, const char* pszExt )
{
	return ::PathRenameExtensionA( pszPath, pszExt );
}

inline BOOL PathRenameExtension( wchar_t* pszPath, const wchar_t* pszExt )
{
	return ::PathRenameExtensionW( pszPath, pszExt );
}

inline char* PathSkipRoot( const char* pszPath )
{
	return ::PathSkipRootA( pszPath );
}

inline wchar_t* PathSkipRoot( const wchar_t* pszPath )
{
	return ::PathSkipRootW( pszPath );
}

inline void PathStripPath( char* pszPath )
{
	::PathStripPathA( pszPath );
}

inline void PathStripPath( wchar_t* pszPath )
{
	::PathStripPathW( pszPath );
}

inline BOOL PathStripToRoot( char* pszPath )
{
	return ::PathStripToRootA( pszPath );
}

inline BOOL PathStripToRoot( wchar_t* pszPath )
{
	return ::PathStripToRootW( pszPath );
}

inline void PathUnquoteSpaces( char* pszPath )
{
	::PathUnquoteSpacesA( pszPath );
}

inline void PathUnquoteSpaces( wchar_t* pszPath )
{
	::PathUnquoteSpacesW( pszPath );
}

inline BOOL PathCompactPath( HDC hDC, char* pszPath, UINT dx )
{
	return ::PathCompactPathA( hDC, pszPath, dx );
}

inline BOOL PathCompactPath( HDC hDC, wchar_t* pszPath, UINT dx )
{
	return ::PathCompactPathW( hDC, pszPath, dx );
}

inline BOOL PathCompactPathEx( char* pszDest, const char* pszSrc,
	UINT nMaxChars, DWORD dwFlags )
{
	return ::PathCompactPathExA( pszDest, pszSrc, nMaxChars, dwFlags );
}

inline BOOL PathCompactPathEx( wchar_t* pszDest, const wchar_t* pszSrc,
	UINT nMaxChars, DWORD dwFlags )
{
	return ::PathCompactPathExW( pszDest, pszSrc, nMaxChars, dwFlags );
}

};  // namespace ATLPath

template< typename StringType >
class CPathT
{
public:
	typedef StringType::XCHAR XCHAR;
	typedef StringType::PCXSTR PCXSTR;
	typedef StringType::PXSTR PXSTR;

public:
	CPathT()
	{
	}
	CPathT( const CPathT< StringType >& path ) :
		m_strPath( path.m_strPath )
	{
	}
	CPathT( PCXSTR pszPath ) :
		m_strPath( pszPath )
	{
	}

	operator const StringType& () const
	{
		return m_strPath;
	}
	operator StringType& ()
	{
		return m_strPath;
	}
	operator PCXSTR() const
	{
		return m_strPath;
	}
	CPathT< StringType >& operator+=( PCXSTR pszMore )
	{
		Append( pszMore );

		return *this;
	}

	void AddBackslash()
	{
		PXSTR pszBuffer;

		pszBuffer = m_strPath.GetBuffer( m_strPath.GetLength()+1 );
		ATLPath::PathAddBackslash( pszBuffer );
		m_strPath.ReleaseBuffer();
	}
	BOOL AddExtension( PCXSTR pszExtension )
	{
		PXSTR pszBuffer;
		BOOL bResult;

		pszBuffer = m_strPath.GetBuffer( m_strPath.GetLength()+StringType::StringLength( pszExtension ) );
		bResult = ATLPath::PathAddExtension( pszBuffer, pszExtension );
		m_strPath.ReleaseBuffer();

		return bResult;
	}
	BOOL Append( PCXSTR pszMore )
	{
		PXSTR pszBuffer;
		BOOL bResult;

		pszBuffer = m_strPath.GetBuffer( m_strPath.GetLength()+StringType::StringLength( pszMore )+1 );
		bResult = ATLPath::PathAppend( pszBuffer, pszMore );
		m_strPath.ReleaseBuffer();

		return bResult;
	}
	void BuildRoot( int iDrive )
	{
		PXSTR pszBuffer;

		ATLASSERT( iDrive >= 0 );
		ATLASSERT( iDrive <= 25 );

		pszBuffer = m_strPath.GetBuffer( 3 );
		ATLPath::PathBuildRoot( pszBuffer, iDrive );
		m_strPath.ReleaseBuffer();
	}
	void Canonicalize()
	{
		PXSTR pszBuffer;
		StringType strResult;

		pszBuffer = strResult.GetBuffer( m_strPath.GetLength() );
		ATLPath::PathCanonicalize( pszBuffer, m_strPath );
		strResult.ReleaseBuffer();

		m_strPath = strResult;
	}
	void Combine( PCXSTR pszDir, PCXSTR pszFile )
	{
		PXSTR pszBuffer;

		pszBuffer = m_strPath.GetBuffer( MAX_PATH );
		ATLPath::PathCombine( pszBuffer, pszDir, pszFile );
		m_strPath.ReleaseBuffer();
	}
	CPathT< StringType > CommonPrefix( PCXSTR pszOther )
	{
		PXSTR pszBuffer;
		int nLength;
		CPathT< StringType > pathResult;

		pszBuffer = pathResult.m_strPath.GetBuffer( max( m_strPath.GetLength(), StringType::StringLength( pszOther ) ) );
		nLength = ATLPath::PathCommonPrefix( m_strPath, pszOther, pszBuffer );
		pathResult.m_strPath.ReleaseBuffer( nLength );

		return pathResult;
	}
	BOOL CompactPath( HDC hDC, UINT nWidth )
	{
		PXSTR pszBuffer;
		BOOL bResult;

		// PathCompactPath can actually _increase_ the length of the path
		pszBuffer = m_strPath.GetBuffer( MAX_PATH );
		bResult = ATLPath::PathCompactPath( hDC, pszBuffer, nWidth );
		m_strPath.ReleaseBuffer();

		return bResult;
	}
	BOOL CompactPathEx( UINT nMaxChars, DWORD dwFlags = 0 )
	{
		StringType strResult;
		BOOL bResult;
		PXSTR pszBuffer;

		pszBuffer = strResult.GetBuffer( nMaxChars );
		bResult = ATLPath::PathCompactPathEx( pszBuffer, m_strPath, nMaxChars,
			dwFlags );
		strResult.ReleaseBuffer();

		m_strPath = strResult;

		return bResult;
	}
	BOOL FileExists() const
	{
		return ATLPath::PathFileExists( m_strPath );
	}
	int FindExtension() const
	{
		PCXSTR pszBuffer;
		PCXSTR pszExtension;

		pszBuffer = m_strPath;
		pszExtension = ATLPath::PathFindExtension( pszBuffer );
		if( *pszExtension == 0 )
			return -1;
		else
			return pszExtension-pszBuffer;
	}
	int FindFileName() const
	{
		PCXSTR pszBuffer;
		PCXSTR pszFileName;

		pszBuffer = m_strPath;
		pszFileName = ATLPath::PathFindFileName( pszBuffer );
		if( *pszFileName == 0 )
			return -1;
		else
			return pszFileName-pszBuffer;
	}
	int GetDriveNumber() const
	{
		return ATLPath::PathGetDriveNumber( m_strPath );
	}
	StringType GetExtension() const
	{
		int iExtension;
		StringType strExtension;

		iExtension = FindExtension();
		if( iExtension != -1 )
			strExtension = m_strPath.Mid( iExtension );

		return strExtension;
	}
	BOOL IsDirectory() const
	{
		return ATLPath::PathIsDirectory( m_strPath );
	}
	/*
	BOOL IsDirectoryEmpty() const
	{
		return Traits::PathIsDirectoryEmpty( m_strPath );
	}
	*/
	BOOL IsFileSpec() const
	{
		return ATLPath::PathIsFileSpec( m_strPath );
	}
	BOOL IsPrefix( PCXSTR pszPrefix ) const
	{
		return ATLPath::PathIsPrefix( pszPrefix, m_strPath );
	}
	BOOL IsRelative() const
	{
		return ATLPath::PathIsRelative( m_strPath );
	}
	BOOL IsRoot() const
	{
		return ATLPath::PathIsRoot( m_strPath );
	}
	BOOL IsSameRoot( PCXSTR pszOther ) const
	{
		return ATLPath::PathIsSameRoot( m_strPath, pszOther );
	}
	BOOL IsUNC() const
	{
		return ATLPath::PathIsUNC( m_strPath );
	}
	BOOL IsUNCServer() const
	{
		return ATLPath::PathIsUNCServer( m_strPath );
	}
	BOOL IsUNCServerShare() const
	{
		return ATLPath::PathIsUNCServerShare( m_strPath );
	}
	BOOL MakePretty()
	{
		PXSTR pszBuffer;
		BOOL bResult;

		pszBuffer = m_strPath.GetBuffer( m_strPath.GetLength() );
		bResult = ATLPath::PathMakePretty( pszBuffer );
		m_strPath.ReleaseBuffer();

		return bResult;
	}
	BOOL MatchSpec( PCXSTR pszSpec ) const
	{
		return ATLPath::PathMatchSpec( m_strPath, pszSpec );
	}
	void QuoteSpaces()
	{
		PXSTR pszBuffer;

		pszBuffer = m_strPath.GetBuffer( m_strPath.GetLength()+2 );
		ATLPath::PathQuoteSpaces( pszBuffer );
		m_strPath.ReleaseBuffer();
	}
	BOOL RelativePathTo( PCXSTR pszFrom, DWORD dwAttrFrom, 
		PCXSTR pszTo, DWORD dwAttrTo )
	{
		PXSTR pszBuffer;
		BOOL bResult;

		pszBuffer = m_strPath.GetBuffer( MAX_PATH );
		bResult = ATLPath::PathRelativePathTo( pszBuffer, pszFrom, dwAttrFrom,
			pszTo, dwAttrTo );
		m_strPath.ReleaseBuffer();

		return bResult;
	}
	void RemoveArgs()
	{
		PXSTR pszBuffer;

		pszBuffer = m_strPath.GetBuffer( m_strPath.GetLength() );
		ATLPath::PathRemoveArgs( pszBuffer );
		m_strPath.ReleaseBuffer();
	}
	void RemoveBackslash()
	{
		PXSTR pszBuffer;

		pszBuffer = m_strPath.GetBuffer( m_strPath.GetLength() );
		ATLPath::PathRemoveBackslash( pszBuffer );
		m_strPath.ReleaseBuffer();
	}
	void RemoveBlanks()
	{
		PXSTR pszBuffer;

		pszBuffer = m_strPath.GetBuffer( m_strPath.GetLength() );
		ATLPath::PathRemoveBlanks( pszBuffer );
		m_strPath.ReleaseBuffer();
	}
	void RemoveExtension()
	{
		PXSTR pszBuffer;

		pszBuffer = m_strPath.GetBuffer( m_strPath.GetLength() );
		ATLPath::PathRemoveExtension( pszBuffer );
		m_strPath.ReleaseBuffer();
	}
	BOOL RemoveFileSpec()
	{
		PXSTR pszBuffer;

		pszBuffer = m_strPath.GetBuffer( m_strPath.GetLength() );
		BOOL bResult = ATLPath::PathRemoveFileSpec( pszBuffer );
		m_strPath.ReleaseBuffer();

		return bResult;
	}
	BOOL RenameExtension( PCXSTR pszExtension )
	{
		PXSTR pszBuffer;
		BOOL bResult;

		pszBuffer = m_strPath.GetBuffer( MAX_PATH );
		bResult = ATLPath::PathRenameExtension( pszBuffer, pszExtension );
		m_strPath.ReleaseBuffer();

		return bResult;
	}
	int SkipRoot() const
	{
		PCXSTR pszBuffer;
		PXSTR pszResult;

		pszBuffer = m_strPath;
		pszResult = ATLPath::PathSkipRoot( pszBuffer );

		return pszResult-pszBuffer;
	}
	void StripPath()
	{
		PXSTR pszBuffer;

		pszBuffer = m_strPath.GetBuffer( m_strPath.GetLength() );
		ATLPath::PathStripPath( pszBuffer );
		m_strPath.ReleaseBuffer();
	}
	BOOL StripToRoot()
	{
		PXSTR pszBuffer;
		BOOL bResult;

		pszBuffer = m_strPath.GetBuffer( m_strPath.GetLength() );
		bResult = ATLPath::PathStripToRoot( pszBuffer );
		m_strPath.ReleaseBuffer();

		return bResult;
	}
	void UnquoteSpaces()
	{
		PXSTR pszBuffer;
		
		pszBuffer = m_strPath.GetBuffer( m_strPath.GetLength() );
		ATLPath::PathUnquoteSpaces( pszBuffer );
		m_strPath.ReleaseBuffer();
	}

public:
	StringType m_strPath;
};

typedef CPathT< CString > CPath;
typedef CPathT< CStringA > CPathA;
typedef CPathT< CStringW > CPathW;

};  // namespace ATL

#endif  //__ATLPATH_H__
