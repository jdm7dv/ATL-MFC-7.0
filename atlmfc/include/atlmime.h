// This is a part of the Active Template Library.
// Copyright (C) 1996-2000 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

// atlmime.h
#pragma once

#include <tchar.h>
#include <time.h>
#include <atlbase.h>
#include <atlfile.h>
#include <atlcoll.h>
#include <atlstr.h>
#include <atlsmtputil.h>
#include <atlenc.h>

#define ATL_MIME_DATE_LEN 64

namespace ATL {

// This function is used to create an CSMTPConnection-compatible recipient string 
// from a recipient string that is in a CMimeMessage object.
inline BOOL AtlMimeMakeRecipientsString(LPCSTR lpszNames, LPSTR lpszRecipients, LPDWORD pdwLen = NULL) throw()
{
	ATLASSERT(lpszNames != NULL);
	ATLASSERT(lpszRecipients != NULL);

	char ch;
	DWORD dwLen = 0;
	while ((ch = *lpszNames++) != '\0')
	{
		// Skip everything that is in double quotes
		if (ch == '"')
		{
			while (*lpszNames++ != '"');
		}
		if (ch == '<')
		{
			// Extract the address from within the <>
			while (*lpszNames != '>')
			{
				*lpszRecipients++ = *lpszNames++;
				dwLen++;
			}
			// End it with a comma
			*lpszRecipients++ = ',';
			dwLen++;
		}
		if (ch == '=')
		{
			// Skip any BEncoded or QEncoded parts
			while (*lpszNames)
			{
				if (*lpszNames == '?' && *(lpszNames+1) == '=')
				{
					lpszNames+=2;
					break;
				}
				lpszNames++;
			}
		}
		lpszNames++;
	}
	if (dwLen != 0)
	{
		lpszRecipients--;
		dwLen--;
	}
	*lpszRecipients = '\0';

	if (pdwLen)
		*pdwLen = dwLen;

	return TRUE;
}

// AtlMimeCharsetFromCodePage, AtlMimeConvertString
// are MIME multilanguage support functions.

// Get the MIME character set of the of the code page.  The character set is copied
// into szCharset.
inline BOOL AtlMimeCharsetFromCodePage(LPSTR szCharset, UINT uiCodePage, IMultiLanguage* pMultiLanguage) throw()
{
	ATLASSERT(szCharset);

	if (!pMultiLanguage)
	{
		if (uiCodePage == 28591)
		{
			strcpy(szCharset, ATLSMTP_DEFAULT_CSET);
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		if (uiCodePage == 0)
			uiCodePage = GetACP();

		HRESULT hr;
		MIMECPINFO cpInfo;
		memset(&cpInfo, 0x00, sizeof(cpInfo));

#ifdef __IMultiLanguage2_INTERFACE_DEFINED__

		CComPtr<IMultiLanguage2> spMultiLanguage2;
		hr = pMultiLanguage->QueryInterface(__uuidof(IMultiLanguage2), (void **)&spMultiLanguage2);
		if (FAILED(hr) || !spMultiLanguage2.p)
			hr = pMultiLanguage->GetCodePageInfo(uiCodePage, &cpInfo);
		else
			hr = spMultiLanguage2->GetCodePageInfo(uiCodePage, 
				LANGIDFROMLCID(GetThreadLocale()), &cpInfo);

#else // __IMultiLanguage2_INTERFACE_DEFINED__

		hr = pMultiLanguage->GetCodePageInfo(uiCodePage, &cpInfo);

#endif // __IMultiLanguage2_INTERFACE_DEFINED__

		if (hr != S_OK)
			return FALSE;

		strcpy(szCharset, CW2A(cpInfo.wszWebCharset));
	}

	return TRUE;
}

inline BOOL AtlMimeConvertStringW(
	IMultiLanguage *pMultiLanguage,
	UINT uiCodePage,
	LPCWSTR wszIn, 
	LPSTR *ppszOut, 
	UINT *pnLen) throw()
{
	ATLASSERT( pMultiLanguage != NULL );
	ATLASSERT( wszIn != NULL );
	ATLASSERT( ppszOut != NULL );
	ATLASSERT( pnLen != NULL );

	if (uiCodePage == 0)
	{
		uiCodePage = GetACP();
	}

	DWORD dwMode = 0;
	CHeapPtr<char> pszOut;

	// get the length
	HRESULT hr = pMultiLanguage->ConvertStringFromUnicode(&dwMode, (DWORD) uiCodePage, const_cast<LPWSTR>(wszIn), NULL, NULL, pnLen);
	if (SUCCEEDED(hr))
	{
		// allocate the buffer
		if (pszOut.Allocate((size_t) (*pnLen)))
		{
			dwMode = 0;
			// do the conversion
			hr = pMultiLanguage->ConvertStringFromUnicode(&dwMode, (DWORD) uiCodePage, const_cast<LPWSTR>(wszIn), NULL, pszOut, pnLen);
			if (SUCCEEDED(hr))
			{
				*ppszOut = pszOut.Detach();
				return TRUE;
			}
		}
	}

	return FALSE;
}

inline BOOL AtlMimeConvertStringA(
	IMultiLanguage *pMultiLanguage,
	UINT uiCodePage,
	LPCSTR szIn, 
	LPSTR *ppszOut, 
	UINT *pnLen) throw()
{
	return AtlMimeConvertStringW(pMultiLanguage, uiCodePage, CA2W(szIn), ppszOut, pnLen);
}


#ifdef _UNICODE
	#define AtlMimeConvertString AtlMimeConvertStringW
#else
	#define AtlMimeConvertString AtlMimeConvertStringA
#endif


// CMimeBodyPart is an abstract base class for the body parts
// CMimeAttachment, CMimeText, CMimeHeader.
class CMimeBodyPart
{
public:

	virtual ~CMimeBodyPart() = 0 {}

	// DumpData - pure virtual method to dump the data for a body part.
	virtual BOOL DumpData(HANDLE hFile, LPOVERLAPPED pOverlapped, LPCSTR pszBoundary, DWORD dwFlags = 0) = 0;

	// GetContentType - pure virtual method to get the content of a body part
	virtual BOOL GetContentType(LPSTR lpszContentType) = 0;

	// GetCharset - virtual method to get the character set of a body part
	// (defaults to ATLSMTP_DEFAULT_CSET).
	virtual BOOL GetCharset(LPSTR lpszCharacterSet)
	{
		strcpy(lpszCharacterSet, ATLSMTP_DEFAULT_CSET);
		return TRUE;
	}

	virtual CMimeBodyPart* Copy() = 0;

protected:

	// MakeMimeHeader - pure virutal method to create a MIME header for a 
	// body part.
	virtual BOOL MakeMimeHeader(CStringA& header, LPCSTR pszBoundary) = 0;
}; // class CMimeBodyPart


// This enum is used with the X-Priority part of the message header
typedef enum {
	MIME_HIGH_PRIORITY   = 1, 
	MIME_NORMAL_PRIORITY = 3, 
	MIME_LOW_PRIORITY    = 5, 
	MIME_PRIORITY_ERROR  = 0
} MIME_PRIORITY;


// CMimeHeader describes the basic RFC 822 message header.
// It also serves as the base class for the CMimeMessage object.
class CMimeHeader : public CMimeBodyPart
{
protected:

	// Pointer to MLANG's IMultiLanguage interface.
	// This is used in doing conversion from code pages
	// to MIME-compatible character sets.
	CComPtr<IMultiLanguage> m_spMultiLanguage;

	//Basic Header Parts
	CStringA        m_From;
	CStringA        m_To;
	CStringA        m_Cc;
	CStringA        m_Bcc;
	CStringA        m_Subject;

	//Extended Header Parts
	MIME_PRIORITY   m_nPriority;
	CStringA        m_XHeader;

	//Display Names
	CStringA        m_SenderName;

	//MIME Character Sets
	char            m_szSubjectCharset[ATL_MAX_ENC_CHARSET_LENGTH];
	char            m_szSenderCharset[ATL_MAX_ENC_CHARSET_LENGTH];

	//Recipient and CC charsets are encoded in the Add methods

public:

	CMimeHeader() throw()
		:m_nPriority(MIME_NORMAL_PRIORITY)
	{
	}

	~CMimeHeader() throw()
	{
	}

	// Initialize MLang for multilanguage support
	inline BOOL Initialize(IMultiLanguage* pMultiLanguage = NULL) throw()
	{
		if (pMultiLanguage != NULL)
		{
			m_spMultiLanguage = pMultiLanguage;
		}
		else
		{
			HRESULT hr = CoCreateInstance(__uuidof(CMultiLanguage), NULL, CLSCTX_INPROC_SERVER, 
				                          __uuidof(IMultiLanguage), (void**)&m_spMultiLanguage);
			if (hr != S_OK)
				return FALSE;
		}
		return TRUE;
	}

	// Get the content type
	inline BOOL GetContentType(LPSTR lpszContentType) throw()
	{
		memcpy(lpszContentType, "text/plain", sizeof("text/plain"));
		return TRUE;
	}

	// Get the character set
	inline BOOL GetCharset(LPSTR lpszCharset) throw()
	{
		memcpy(lpszCharset, "iso-8859-1", sizeof("iso-8859-1"));
		return TRUE;
	}

	ATL_NOINLINE CMimeBodyPart* Copy()
	{
		CMimeHeader* pNewHeader = NULL;
		ATLTRY(pNewHeader = new CMimeHeader);
		if (pNewHeader)
			*pNewHeader = *this;

		return pNewHeader;
	}

	const CMimeHeader& operator=(const CMimeHeader& that)
	{
		if (this != &that)
		{
			m_spMultiLanguage = that.m_spMultiLanguage;
			m_From = that.m_From;
			m_To = that.m_To;
			m_Cc = that.m_Cc;
			m_Subject = that.m_Subject;

			m_nPriority = that.m_nPriority;
			m_XHeader = that.m_XHeader;

			m_SenderName = that.m_SenderName;

			strcpy(m_szSubjectCharset, that.m_szSubjectCharset);
			strcpy(m_szSenderCharset, that.m_szSenderCharset);
		}

		return *this;
	}

	// Set the priority of the message
	inline BOOL SetPriority(MIME_PRIORITY nPriority) throw()
	{
		if (nPriority < 0)
			return FALSE;
		m_nPriority = nPriority;
		return TRUE;
	}

	// Get the priority of the message
	inline MIME_PRIORITY GetPriority() throw()
	{
		return m_nPriority;
	}

	// Set the display (friendly) name for the header
	inline BOOL SetSenderName(LPCTSTR lpszName, UINT uiCodePage = 0) throw()
	{
		if (!lpszName)
			return FALSE;

		CHeapPtr<char> szName;
		UINT nLen(0);

		BOOL bRet = AtlMimeConvertString(m_spMultiLanguage, uiCodePage, lpszName, &szName, &nLen);
		if (bRet)
		{
			m_SenderName = "";
			m_SenderName.Append(szName, (int) nLen);
			bRet = AtlMimeCharsetFromCodePage(m_szSenderCharset, uiCodePage, m_spMultiLanguage);
		}

		return bRet;
	}

	// Get the display (friendly) name for the sender
	inline LPCSTR GetSenderName() throw()
	{
		return m_SenderName;
	}

	// Append a user defined header (should not contain CRLF)
	inline BOOL AppendUserDefinedHeader(LPCTSTR lpszHeaderName, LPCTSTR lpszHeader, UINT uiCodePage = 0) throw()
	{
		if (!lpszHeader)
			return FALSE;

		CHeapPtr<char> szName;
		UINT nLen(0);

		BOOL bRet = AtlMimeConvertString(m_spMultiLanguage, uiCodePage, lpszHeader, &szName, &nLen);
		if (bRet)
		{
			// get the charset
			char szCharset[ATL_MAX_ENC_CHARSET_LENGTH];			
			bRet = AtlMimeCharsetFromCodePage(szCharset, uiCodePage, m_spMultiLanguage);

			if (bRet)
			{
				CStringA str;
				str.Append(szName, (int)nLen);

				// encode the string
				char szBuf[1000];
				DWORD dwLength(0);
				if (!GetEncodedString(str, szCharset, szBuf, 1000, dwLength))
					return FALSE;

				// add to m_XHeader
				m_XHeader += CT2CA(lpszHeaderName);
				m_XHeader.Append(": ", 2);
				m_XHeader.Append(szBuf, dwLength);
				m_XHeader.Append("\r\n", 2);
			}
		}

		return bRet;
	}

	// Add a recipient ("To:" line)
	inline BOOL AddRecipient(LPCTSTR lpszAddress, LPCTSTR lpszName = NULL, UINT uiCodePage = 0)
	{
		return AddRecipientHelper(m_To, lpszAddress, lpszName, uiCodePage);
	}

	// Get the recipients string ("To:" line)
	inline LPCSTR GetRecipients() throw()
	{
		return m_To;
	}

	// Clear all recipients ("To:" line)
	inline BOOL ClearRecipients() throw()
	{
		m_To = "";
		return TRUE;
	}

	// Add a recipient ("CC:" line)
	inline BOOL AddCc(LPCTSTR lpszAddress, LPCTSTR lpszName = NULL, UINT uiCodePage = 0)
	{
		return AddRecipientHelper(m_Cc, lpszAddress, lpszName, uiCodePage);
	}

	// Get the recipients string ("CC:" line)
	inline LPCSTR GetCc() throw()
	{
		return m_Cc;
	}

	// Clear the recipients string ("CC:" line)
	inline BOOL ClearCc() throw()
	{
		m_Cc = "";
		return TRUE;
	}

	// Add a Bcc recipient (not output as part of message)
	inline BOOL AddBcc(LPCTSTR lpszAddress)
	{
		if (m_Bcc.GetLength() > 0)
			m_Bcc += ',';

		m_Bcc += CT2CA(lpszAddress);

		return TRUE;
	}

	// Get the recipients string (Bcc part)
	inline LPCSTR GetBcc() throw()
	{
		return m_Bcc;
	}

	// Clear the recipients string (Bcc part)
	inline BOOL ClearBcc() throw()
	{
		m_Bcc = "";
		return TRUE;
	}


	inline DWORD GetRequiredRecipientsStringLength()
	{
		DWORD dwRet = m_To.GetLength();
		if (m_Cc.GetLength())
		{
			dwRet += dwRet ? 1 : 0;
			dwRet += m_Cc.GetLength();
		}
		if (m_Bcc.GetLength())
		{
			dwRet += dwRet ? 1 : 0;
			dwRet += m_Bcc.GetLength();
		}
		dwRet++;
		return dwRet;
	}

	// returns the recipients string to be (addresses only, in comma separated format)
	ATL_NOINLINE BOOL GetRecipientsString(LPSTR szRecip, LPDWORD pdwLen)
	{
		ATLASSERT(szRecip != NULL);
		ATLASSERT(pdwLen != NULL);

		if (*pdwLen < GetRequiredRecipientsStringLength())
		{
			*pdwLen = GetRequiredRecipientsStringLength();
			return FALSE;
		}

		DWORD dwLen = 0;
		DWORD dwTotalLen = 0;
		if (m_To.GetLength() > 0)
		{
			if (AtlMimeMakeRecipientsString(m_To, szRecip, &dwLen) != TRUE)
			{
				return FALSE;
			}
			szRecip+= dwLen;
			dwTotalLen = dwLen;
		}

		if (m_Cc.GetLength() > 0)
		{
			if (dwTotalLen)
			{
				*szRecip++ = ',';
				dwTotalLen++;
			}
			if (AtlMimeMakeRecipientsString(m_Cc, szRecip, &dwLen) != TRUE)
			{
				return FALSE;
			}
			szRecip+= dwLen;
			dwTotalLen+= dwLen;
		}

		if (m_Bcc.GetLength() > 0)
		{
			dwLen = m_Bcc.GetLength();
			if (dwTotalLen)
			{
				*szRecip++ = ',';
				dwTotalLen++;
			}
			memcpy(szRecip, m_Bcc, dwLen);
			szRecip+= dwLen;
			dwTotalLen+= dwLen;
		}

		*szRecip = '\0';
		*pdwLen = dwTotalLen;

		return TRUE;
	}


	// Get the sender
	inline LPCSTR GetSender() throw()
	{
		return m_From;
	}

	// Set the sender
	inline BOOL SetSender(LPCTSTR lpszSender) throw()
	{
		if (!lpszSender)
			return FALSE;

		m_From = CT2CA(lpszSender);
		return TRUE;
	}

	// Set the subject
	inline BOOL SetSubject(LPCTSTR lpszSubject, UINT uiCodePage = 0) throw()
	{
		if (!lpszSubject)
			return FALSE;

		CHeapPtr<char> szName;
		UINT nLen(0);

		BOOL bRet = AtlMimeConvertString(m_spMultiLanguage, uiCodePage, lpszSubject, &szName, &nLen);
		if (bRet)
		{
			m_Subject = "";
			m_Subject.Append(szName, (int)nLen);
			bRet = AtlMimeCharsetFromCodePage(m_szSubjectCharset, uiCodePage, m_spMultiLanguage);
		}

		return bRet;
	}

	// Get the subject
	inline LPCSTR GetSubject()
	{
		return (LPCSTR)m_Subject;
	}

	// Dump the header to hFile
	inline BOOL DumpData(HANDLE hFile, LPOVERLAPPED pOverlapped, LPCSTR /*pszBoundary*/, DWORD dwFlags = 0) throw()
	{
		// Allocate the buffers
		LPSTR pSendBuffer = (LPSTR)malloc(GetRequiredBufferSize(ATLSMTP_MAX_LINE_LENGTH-4)*sizeof(char));
		if (!pSendBuffer)
			return FALSE;

		CHeapPtr<char> spSendBuffer;
		spSendBuffer.Attach(pSendBuffer);

		// choose QEncode here, because the max QEncodeGetRequiredLength will always
		// return a value greater than BEncodeGetRequiredLength
		int nBufLen = max(QEncodeGetRequiredLength(m_Subject.GetLength(), 
								ATL_MAX_ENC_CHARSET_LENGTH),
		                  QEncodeGetRequiredLength(m_SenderName.GetLength(), 
								ATL_MAX_ENC_CHARSET_LENGTH)+m_From.GetLength()+2);

		char* pszBuf = (LPSTR)malloc(nBufLen*sizeof(char));
		if (!pszBuf)
		{
			return FALSE;
		}
		CHeapPtr<char> spBuf;
		spBuf.Attach(pszBuf);

		DWORD dwOffset = 0;

		char szDate[ATL_MIME_DATE_LEN];

		SetTime(szDate);

		DWORD dwLength = (DWORD) strlen(szDate);
		memcpy(pSendBuffer+dwOffset, szDate, dwLength);
		dwOffset += dwLength;
		*(pSendBuffer+dwOffset++) = '\r';
		*(pSendBuffer+dwOffset++) = '\n';

		DWORD dwHeaderPartLength = 0;
		*pszBuf = '\0';

		// Get the sender name
		BOOL bRet = TRUE;
		if (m_SenderName.GetLength() > 0)
		{
			bRet = GetEncodedString(m_SenderName, m_szSenderCharset, pszBuf, nBufLen, dwLength);
			dwHeaderPartLength += dwLength;
		}

		// Get the sender email address
		if (bRet && m_From.GetLength() > 0)
		{
			if (dwHeaderPartLength != 0)
			{
				*(pszBuf+dwHeaderPartLength++) = ' ';
			}
			*(pszBuf+dwHeaderPartLength++) = '<';
			memcpy(pszBuf+dwHeaderPartLength, (LPCSTR)m_From, m_From.GetLength());
			dwHeaderPartLength+= m_From.GetLength();
			*(pszBuf+dwHeaderPartLength++) = '>';
		}

		// Output the "From: " line
		if (bRet && dwHeaderPartLength != 0)
		{
			memcpy(pSendBuffer+dwOffset, "From: ", sizeof("From: ")-1);
			dwOffset+= 6;
			DWORD dwWritten = 0;
			bRet = FormatField((LPBYTE)pszBuf, dwHeaderPartLength, (LPBYTE)(pSendBuffer+dwOffset), &dwWritten, dwFlags);
			dwOffset += dwWritten;
			*(pSendBuffer+dwOffset++) = '\r';
			*(pSendBuffer+dwOffset++) = '\n';
		}

		// Output the subject
		if (bRet && m_Subject.GetLength() > 0)
		{
			dwLength = 0;
			bRet = GetEncodedString(m_Subject, m_szSubjectCharset, pszBuf, nBufLen, dwLength);
			if (bRet && dwLength != 0)
			{
				memcpy(pSendBuffer+dwOffset, "Subject: ", sizeof("Subject: ")-1);
				dwOffset+= 9;
				DWORD dwWritten = 0;
				bRet = FormatField((LPBYTE)pszBuf, dwLength, (LPBYTE)(pSendBuffer+dwOffset), &dwWritten, dwFlags);
				dwOffset += dwWritten;
				*(pSendBuffer+dwOffset++) = '\r';
				*(pSendBuffer+dwOffset++) = '\n';
			}
		}

		// Output the "To:" line
		if (bRet && m_To.GetLength() > 0)
		{
			memcpy(pSendBuffer+dwOffset, "To: ", sizeof("To: ")-1);
			dwOffset+= 4;
			DWORD dwWritten = 0;
			FormatRecipients((LPBYTE)((LPCSTR)m_To), m_To.GetLength(), (LPBYTE)(pSendBuffer+dwOffset), &dwWritten);
			dwOffset+= dwWritten;
			*(pSendBuffer+dwOffset++) = '\r';
			*(pSendBuffer+dwOffset++) = '\n';
		}

		// Output the "CC:" line
		if (bRet && m_Cc.GetLength() > 0)
		{
			memcpy(pSendBuffer+dwOffset, "CC: ", sizeof("CC: ")-1);
			dwOffset+= 4;
			DWORD dwWritten = 0;
			bRet = FormatRecipients((LPBYTE)((LPCSTR)m_Cc), m_Cc.GetLength(), (LPBYTE)(pSendBuffer+dwOffset), &dwWritten);
			dwOffset+= dwWritten;
			*(pSendBuffer+dwOffset++) = '\r';
			*(pSendBuffer+dwOffset++) = '\n';
		}

		// Send the header
		if (bRet && dwOffset)
			bRet = AtlSmtpSendAndWait(hFile, pSendBuffer, dwOffset, pOverlapped);

		return bRet;
	}

protected:

	// Called when message is sent - sets the "Date:" field
	inline void SetTime(LPSTR szDate) throw()
	{
		static const LPCSTR s_months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                                   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

		static const LPCSTR s_days[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

		SYSTEMTIME st;
		DWORD      dwTimeZoneId=TIME_ZONE_ID_UNKNOWN;
		CHAR       cDiff;
		LONG       ltzBias=0;
		LONG       ltzHour;
		LONG       ltzMinute;
		TIME_ZONE_INFORMATION tzi;

		GetLocalTime(&st);

		// Gets TIME_ZONE_INFORMATION
		dwTimeZoneId = GetTimeZoneInformation(&tzi);
		switch (dwTimeZoneId)
		{
		case TIME_ZONE_ID_STANDARD:
			ltzBias = tzi.Bias + tzi.StandardBias;
			break;

		case TIME_ZONE_ID_DAYLIGHT:
			ltzBias = tzi.Bias + tzi.DaylightBias;
			break;

		case TIME_ZONE_ID_UNKNOWN:
		default:
			ltzBias = tzi.Bias;
			break;
		}

		// Set Hour Minutes and time zone dif
		ltzHour = ltzBias / 60;
		ltzMinute = ltzBias % 60;
		cDiff = (ltzHour < 0) ? '+' : '-';

		int nDay = (st.wDayOfWeek > 6) ? 0 : st.wDayOfWeek;
		int nMonth = st.wMonth = (st.wMonth < 1 || st.wMonth > 12) ? 0 : st.wMonth - 1;


		// Constructs RFC 822 format: "ddd, dd mmm yyyy hh:mm:ss +/- hhmm\0"
		wsprintfA(szDate, "Date: %3s, %d %3s %4d %02d:%02d:%02d %c%02d%02d",
						  s_days[nDay],                            // "ddd"
						  st.wDay,                                 // "dd"
						  s_months[nMonth],                        // "mmm"
						  st.wYear,                                // "yyyy"
						  st.wHour,                                // "hh"
						  st.wMinute,                              // "mm"
						  st.wSecond,                              // "ss"
						  cDiff,                                   // "+" / "-"
						  abs (ltzHour),                           // "hh"
						  abs (ltzMinute));                        // "mm"
	}

	// Make the mime header
	inline BOOL MakeMimeHeader(CStringA& /*header*/, LPCSTR /*pszBoundary*/) throw()
	{
		// The message header does not have its own MIME header
		ATLASSERT(FALSE);
		return TRUE;
	}

	// Get an encoded string for a header field
	inline BOOL GetEncodedString(CStringA& headerString, LPCSTR szCharset, LPSTR szBuf, int nBufLen, DWORD& dwLength) throw()
	{
		BOOL bEncoded = FALSE;
		if (m_spMultiLanguage.p)
		{
			// only encode if there are 8bit characters
			int nExtendedChars = GetExtendedChars(headerString, headerString.GetLength());
			if (nExtendedChars)
			{
				// choose smallest encoding
				if (((nExtendedChars*100)/headerString.GetLength()) < 17)
				{
					int nEncCnt = 0;
					if (!QEncode((LPBYTE)((LPCSTR)headerString), headerString.GetLength(), szBuf, &nBufLen, szCharset, &nEncCnt))
					{
						return FALSE;
					}

					//if no unsafe characters were encountered, just output it
					if (nEncCnt != 0)
					{
						bEncoded = TRUE;
					}
				}
				else
				{
					if (!BEncode((LPBYTE)((LPCSTR)headerString), headerString.GetLength(), szBuf, &nBufLen, szCharset))
					{
						return FALSE;
					}

					bEncoded = TRUE;
				}
			}
		}

		if (!bEncoded)
		{
			// there was no encoding
			dwLength = sprintf(szBuf, headerString);
		}
		else
		{
			dwLength = nBufLen;
		}
		return TRUE;
	}


	// Helper function for adding recipients
	inline BOOL AddRecipientHelper(CStringA& str, LPCTSTR lpszAddress, LPCTSTR lpszName = NULL, UINT uiCodePage = 0) throw()
	{
		if (!lpszAddress && !lpszName)
		{
			return FALSE;
		}

		if (str.GetLength() != 0)
			str += ", ";

		if (lpszName)
		{
			CHeapPtr<char> szName;
			UINT nLen(0);

			BOOL bRet = AtlMimeConvertString(m_spMultiLanguage, uiCodePage, lpszName, &szName, &nLen);
			if (bRet)
			{
				CStringA Name = "\"";
				Name.Append(szName, (int)nLen);
				Name.Append("\"", 1);

				char szCharset[ATL_MAX_ENC_CHARSET_LENGTH];

				if (!AtlMimeCharsetFromCodePage(szCharset, uiCodePage, m_spMultiLanguage))
					return FALSE;

				char szBuf[256];
				int nBufLen = 256;
				DWORD dwLength = 0;
				if (!GetEncodedString(Name, szCharset, szBuf, nBufLen, dwLength))
					return FALSE;
				str += szBuf;
			}
		}

		if (lpszAddress)
		{
			if (lpszName)
				str += ' ';
			str += '<';
			str += CT2CA(lpszAddress);
			str += '>';
		}
		return TRUE;
	}

	// Get the formatted header information
	inline BOOL FormatField(LPBYTE pbSrcData, int nSrcLen, LPBYTE pbDest, 
		DWORD* pdwWritten = NULL, DWORD dwFlags = 0) throw()
	{
		int nRead = 0;

		// 9 is the length of the maximum field name : "Subject :"
		// we set that here for simplicity
		int nLineLen = 9;
		int nWritten = 0;

		//subtract 2 from these because it's easier for when we have
		//to break lines with a CRLF (and tab if necessary)
		int nMaxLineLength = ATLSMTP_MAX_LINE_LENGTH-3;
		while (nRead < nSrcLen)
		{
			//if we're at the end of the line, break it
			if (nLineLen == nMaxLineLength)
			{
				*pbDest++ = '\r';
				*pbDest++ = '\n';
				nWritten+= 2;
				nLineLen = -1;

				if ((dwFlags & ATLSMTP_FORMAT_SMTP))
				{
					*pbDest++ = '\t';
					nWritten++;
					nLineLen++;
				}
			}

			//if we hit a CRLF, reset nLineLen
			if (*pbSrcData == '\n' && nRead > 0 && *(pbSrcData-1) == '\r')
			{
				nLineLen = -1;
			}

			*pbDest++ = *pbSrcData++;
			nRead++;
			nWritten++;
			nLineLen++;
		}

		if (pdwWritten)
			*pdwWritten = (DWORD)nWritten;

		return TRUE;
	}


	// Get the formatted recipient information
	inline BOOL FormatRecipients(LPBYTE pbSrcData, int nSrcLen, LPBYTE pbDest, 
		DWORD* pdwWritten = NULL) throw()
	{
		int nRead    = 0;
		int nWritten = 0;

		while (nRead < nSrcLen)
		{
			if (*pbSrcData == ',')
			{
				*pbDest++ = *pbSrcData++;
				nRead++;
				if (nRead+1 <= nSrcLen && *pbSrcData == ' ')
				{
					pbSrcData++;
					nRead++;
				}
				*pbDest++ = '\r';
				*pbDest++ = '\n';
				*pbDest++ = '\t';
				nWritten+= 4;

				continue;
			}

			*pbDest++ = *pbSrcData++;
			nRead++;
			nWritten++;
		}

		if (pdwWritten)
			*pdwWritten = (DWORD)nWritten;

		return TRUE;
	}

	// Get the required buffer size for the header
	inline int GetRequiredBufferSize(int nMaxLineLength) throw()
	{
		//data lengths (QEncoding potentially takes up more space than BEncoding,
		//so default to it)
		int nRequiredLength = QEncodeGetRequiredLength(m_SenderName.GetLength(), ATL_MAX_ENC_CHARSET_LENGTH)
			+QEncodeGetRequiredLength(m_Subject.GetLength(), ATL_MAX_ENC_CHARSET_LENGTH);
		nRequiredLength += m_From.GetLength()+m_To.GetLength()+m_Cc.GetLength();

		//Add space for date
		nRequiredLength += 27;

		//Add space for From: line
		nRequiredLength += 10;
		
		//Add space for To: line
		nRequiredLength += 6;

		//Add space for Cc: line
		nRequiredLength += 6;

		//Add space for Subject: line
		nRequiredLength += 11;

		//Add space for line breaks and tabs
		nRequiredLength += 3*(nRequiredLength/nMaxLineLength);

		//Trailing CRLF
		nRequiredLength += 2;

		return nRequiredLength;
	}

}; // class CMimeHeader


// CMimeAttachment is an abstract base class for MIME message attachments.
// It serves as a base class for CMimeFileAttachment and CMimeRawAttachment
class CMimeAttachment : public CMimeBodyPart
{
protected:

	// the encoding scheme (ATLSMTP_BASE64_ENCODE, ATLSMTP_UUENCODE, ATLSMTP_QP_ENCODE)
	int      m_nEncodingScheme;

	// the content type of the attachment
	CStringA m_ContentType;

	// the character set
	char     m_szCharset[ATL_MAX_ENC_CHARSET_LENGTH];

	// the encode string ("base64", "qpencode", "uuencode")
	char     m_szEncodeString[20];
	
	// the display name of the attachment
	TCHAR    m_szDisplayName[_MAX_FNAME];

public:
	CMimeAttachment() throw()
		:m_nEncodingScheme(ATLSMTP_BASE64_ENCODE)
	{
	}

	virtual ~CMimeAttachment() throw()
	{
	}

	// CMimeFileAttachment and CMimeRawAttachment have to handle their own dumping
	inline BOOL DumpData(HANDLE hFile, LPOVERLAPPED pOverlapped, LPCSTR lpszBoundary, DWORD dwFlags = 0) = 0;

	// Set the encoding scheme of the attachment
	inline BOOL SetEncodingScheme(int nScheme) throw()
	{
		if (nScheme != ATLSMTP_BASE64_ENCODE && nScheme != ATLSMTP_UUENCODE && nScheme != ATLSMTP_QP_ENCODE)
		{
			return FALSE;
		}

		m_nEncodingScheme = nScheme;
		return TRUE;
	}
	
	// Set the Content-Type of the attachment
	inline BOOL SetContentType(LPCTSTR lpszContent) throw()
	{
		m_ContentType = CT2CA(lpszContent);
		return TRUE;
	}

	// Get the content type of the attachment
	inline BOOL GetContentType(LPSTR lpszContentType) throw()
	{
		strcpy(lpszContentType, m_ContentType);
		return TRUE;
	}

	// Get the character set of the attachment
	inline BOOL GetCharset(LPSTR lpszCharset) throw()
	{
		strcpy(lpszCharset, m_szCharset);
		return TRUE;
	}

	ATL_NOINLINE CMimeBodyPart* Copy() = 0;

	const CMimeAttachment& operator=(const CMimeAttachment& that)
	{
		if (this != &that)
		{
			m_nEncodingScheme = that.m_nEncodingScheme;
			m_ContentType = that.m_ContentType;
			strcpy(m_szCharset, that.m_szCharset);
			strcpy(m_szEncodeString, that.m_szEncodeString);
			_tcscpy(m_szDisplayName, that.m_szDisplayName);
		}

		return *this;
	}

protected:

	// Make the MIME header for the attachment
	inline BOOL MakeMimeHeader(CStringA& header, LPCSTR pszBoundary) throw()
	{
		// if no display name is specified, default to "rawdata"
		return MakeMimeHeader(header, pszBoundary, _T("rawdata"));
	}

	// Make the MIME header with the specified filename
	inline BOOL MakeMimeHeader(CStringA& header, LPCSTR pszBoundary, LPCTSTR lpszFileName) throw()
	{
		char szBegin[256];
		if (*pszBoundary)
		{
			// this is not the only body part
			memcpy(szBegin, "\r\n\r\n--", 6);
			memcpy(szBegin+6, pszBoundary, 32);
			*(szBegin+38) = '\0';
		}
		else
		{
			// this is the only body part, so output the MIME header
			memcpy(szBegin, "MIME-Version: 1.0", sizeof("MIME-Version: 1.0"));
		}

		// Get file name with the path stripped out
		LPTSTR pszTmpFile;
		if (*m_szDisplayName)
			pszTmpFile = m_szDisplayName;
		else 
		{
			pszTmpFile = _tcsrchr(lpszFileName, '\\');
			if (pszTmpFile)
			{
				pszTmpFile++;
			}
			else
			{
				pszTmpFile = const_cast<LPTSTR>(lpszFileName);
			}
		}

		CT2CAEX<MAX_PATH+1> lpszFileNameA(pszTmpFile);

		header.Format("%s\r\nContent-Type: %s;\r\n\tcharset=\"%s\"\r\n\tname=\"%s\"\r\n"
                      "Content-Transfer-Encoding: %s\r\nContent-Disposition: attachment;\r\n\tfilename=\"%s\"\r\n\r\n",
			szBegin, (LPCSTR) m_ContentType, m_szCharset, (LPCSTR) lpszFileNameA, m_szEncodeString, (LPCSTR) lpszFileNameA);
		return TRUE;
	}

	// Get encoding information
	inline BOOL GetEncodingInformation(LPSTR szEnc, int* pnRequiredLength, int* pnLineLength) throw()
	{
		ATLASSERT(szEnc && pnRequiredLength && pnLineLength);
		switch(m_nEncodingScheme)
		{
			case ATLSMTP_BASE64_ENCODE:
				memcpy(szEnc, "base64", sizeof("base64"));
				*pnLineLength = ATLSMTP_MAX_BASE64_LINE_LENGTH;
				*pnRequiredLength = Base64EncodeGetRequiredLength(ATLSMTP_MAX_BASE64_LINE_LENGTH);
				break;
			case ATLSMTP_UUENCODE:
				memcpy(szEnc, "uuencode", sizeof("uuencode"));
				*pnLineLength = ATLSMTP_MAX_UUENCODE_LINE_LENGTH;
				*pnRequiredLength = UUEncodeGetRequiredLength(ATLSMTP_MAX_UUENCODE_LINE_LENGTH);
				break;
			case ATLSMTP_QP_ENCODE:
				memcpy(szEnc, "quoted-printable", sizeof("quoted-printable"));
				*pnLineLength = ATLSMTP_MAX_QP_LINE_LENGTH;
				*pnRequiredLength = QPEncodeGetRequiredLength(ATLSMTP_MAX_QP_LINE_LENGTH);
				break;
			default:
				return FALSE;
		}
		return TRUE;
	}

}; // class CMimeAttachment


// CMimeFileAttachment represents a MIME file attachment body part
class CMimeFileAttachment : public CMimeAttachment
{
	
protected:
	// The filename
	TCHAR m_szFileName[_MAX_PATH];

public:
	CMimeFileAttachment() throw()
	{
	}

	ATL_NOINLINE CMimeBodyPart* Copy()
	{
		CMimeFileAttachment* pNewAttachment = NULL;
		ATLTRY(pNewAttachment = new CMimeFileAttachment);
		if (pNewAttachment)
			*pNewAttachment = *this;

		return pNewAttachment;
	}

	const CMimeFileAttachment& operator=(const CMimeFileAttachment& that)
	{
		if (this != &that)
		{
			CMimeAttachment::operator=(that);
			_tcscpy(m_szFileName, that.m_szFileName);
		}

		return *this;
	}
		

	// Initialize the file attachment
	// lpszFileName - the actual file name
	// lpszDisplayName - the display name for the file (optional)
	// pMultiLanguage - the IMulitLanguage pointer for codepage to charset conversion (optional)
	// uiCodePage - the code page (optional)
	inline BOOL Initialize(LPCTSTR lpszFileName, LPCTSTR lpszDisplayName = NULL, IMultiLanguage* pMultiLanguage = NULL, UINT uiCodePage = 0) throw()
	{
		if (!AtlMimeCharsetFromCodePage(m_szCharset, uiCodePage, pMultiLanguage))
			return FALSE;

		_tcscpy(m_szFileName, lpszFileName);

		if (lpszDisplayName)
		{
			// use the user-specified display name
			_tcscpy(m_szDisplayName, lpszDisplayName);
		}
		else
		{
			// otherwise there is no display name
			*m_szDisplayName = '\0';
		}
		return TRUE;
	}
 
	// Dump the data for the file attachment
	inline BOOL DumpData(HANDLE hFile, LPOVERLAPPED pOverlapped, LPCSTR pszBoundary, DWORD dwFlags = 0) throw()
	{
		int nLineLength = 0;
		int nRequiredLength = 0;

		if (!GetEncodingInformation(m_szEncodeString, &nRequiredLength, &nLineLength))
			return FALSE;

		//Try to open the file that is being attached
		CAtlFile readFile;
		if (FAILED(readFile.Create(m_szFileName, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING)))
			return FALSE;

		//Make the mime header
		CStringA header;

		MakeMimeHeader(header, pszBoundary, m_szFileName);

		//Try to send the mime header
		if (!AtlSmtpSendAndWait(hFile, ((LPCSTR)header), header.GetLength(), pOverlapped))
		{
			return FALSE;
		}

		int nGetLines = ATLSMTP_GET_LINES;

		nRequiredLength *= nGetLines;

		//dwToGet is the total number of characters to attempt to get
		DWORD dwToGet = (DWORD)nGetLines*nLineLength;

		//allocate the data array
		CHeapPtr<BYTE> spData;
		if (!spData.Allocate(dwToGet+1))
			return FALSE;

// if double buffering is defined, create two buffers
#ifdef ATLSMTP_DOUBLE_BUFFERED
		CHeapPtr<char> buffer1;
		if (!buffer1.Allocate(nRequiredLength+3))
			return FALSE;
		
		CHeapPtr<char> buffer2;
		if (!buffer2.Allocate(nRequiredLength+3))
			return FALSE;

		char* currBuffer = buffer1;
		char* prevBuffer = NULL;
		int nCurrBuffer = 0;
		DWORD dwPrevLength = 0;
#else
		CHeapPtr<char> currBuffer;
		if (!currBuffer.Allocate(nRequiredLength+3))
			return FALSE;

#endif // ATLSMTP_DOUBLE_BUFFERED

		int nEncodedLength = nRequiredLength;
		BOOL bRet = FALSE;
		DWORD dwRead = 0;
		DWORD dwTotalRead = 0;
		DWORD dwCurrRead = 0;

		do
		{
			do 
			{
				//Read a chunk of data from the file increment buffer offsets and amount to read
				//based on what's already been read in this iteration of the loop
				HRESULT hr = readFile.Read(((LPBYTE)spData)+dwCurrRead, dwToGet-dwCurrRead, dwRead);
				if (FAILED(hr))
				{
					if (hr != AtlHresultFromWin32(ERROR_MORE_DATA))
					{
						return FALSE;
					}
				}
				dwCurrRead += dwRead;

			} while (dwRead != 0 && dwCurrRead < dwToGet);

			//reset nEncodedLength
			nEncodedLength = nRequiredLength;
			switch (m_nEncodingScheme)
			{
				case ATLSMTP_BASE64_ENCODE:
					//if we are at the end of input (dwCurrRead < dwToGet), output the trailing padding if necessary
					//(ATL_FLAG_NONE)
					bRet = Base64Encode(spData, dwCurrRead, currBuffer, &nEncodedLength, 
						(dwCurrRead < dwToGet ? ATL_BASE64_FLAG_NONE: ATL_BASE64_FLAG_NOPAD));
					//Base64Encoding needs explicit CRLF added
					currBuffer[nEncodedLength++] = '\r';
					currBuffer[nEncodedLength++] = '\n';
					break;
				case ATLSMTP_UUENCODE:
					//if we are at the beginning of the input, output the header (ATL_UUENCODE_HEADER)
					//if we are the end of input (dwCurrRead < dwToGet), output the 'end'
					//we are encoding for purposes of sending mail, so stuff dots (ATL_UUENCODE_DOT)
					bRet = UUEncode(spData, dwCurrRead, currBuffer, &nEncodedLength, m_szFileName,
						            (dwTotalRead > 0 ? 0 : ATLSMTP_UUENCODE_HEADER) | 
						            (dwCurrRead < dwToGet ? ATLSMTP_UUENCODE_END : 0) | 
						            ((dwFlags & ATLSMTP_FORMAT_SMTP) ? ATLSMTP_UUENCODE_DOT : 0));
					break;
				case ATLSMTP_QP_ENCODE:
					//we are encoding for purposes of sending mail, so stuff dots
					bRet = QPEncode(spData, dwCurrRead, currBuffer, &nEncodedLength, 
						            ((dwFlags & ATLSMTP_FORMAT_SMTP) ? ATLSMTP_QPENCODE_DOT : 0) |
						            (dwCurrRead < dwToGet ? 0 : ATLSMTP_QPENCODE_TRAILING_SOFT));
					break;
			}
			//try to send the encoded data
#ifdef ATLSMTP_DOUBLE_BUFFERED
			if (bRet)
			{
				bRet = AtlSmtpSendOverlapped(hFile, currBuffer, nEncodedLength, 
					prevBuffer, dwPrevLength, pOverlapped);
			}

			//swap the buffers
			dwPrevLength = nEncodedLength;
			prevBuffer = currBuffer;
			currBuffer = (nCurrBuffer == 0 ? buffer2 : buffer1);
			nCurrBuffer = (nCurrBuffer == 0 ? 1 : 0);
#else
			if (bRet)
			{
				bRet = AtlSmtpSendAndWait(hFile, currBuffer, nEncodedLength, pOverlapped);
			}
#endif // ATLSMTP_DOUBLE_BUFFERED

			dwTotalRead += dwCurrRead;
			if (dwRead != 0)
				dwCurrRead = 0;

			nEncodedLength = nRequiredLength;

		} while (dwRead != 0 && bRet);

		//ensure that the last Send sent all the data
#ifdef ATLSMTP_DOUBLE_BUFFERED
		DWORD dwWritten = 0, dwErr = 0;
		if (!GetOverlappedResult(hFile, pOverlapped, &dwWritten, TRUE))
		{
			if ((dwErr = GetLastError()) != ERROR_IO_PENDING && dwErr != ERROR_IO_INCOMPLETE)
			{
				bRet = FALSE;
			}
			else if (dwWritten < dwPrevLength)
			{
				bRet = AtlSmtpSendAndWait(hFile, prevBuffer+dwWritten, 
					dwPrevLength-dwWritten, pOverlapped);
			}
		}
#endif // ATLSMTP_DOUBLE_BUFFERED

		//for uuencoding, if the last chunk read was of size dwToGet, but it was also the end of the file,
		//the "end" keyword will not get encoded, so a check is necessary
		if (m_nEncodingScheme == ATLSMTP_UUENCODE && dwCurrRead == dwToGet)
		{
			bRet = UUEncode(spData, 0, currBuffer, &nEncodedLength, m_szFileName, 
				            (dwFlags & ATLSMTP_FORMAT_SMTP ? ATLSMTP_UUENCODE_DOT : 0) |
				            ATLSMTP_UUENCODE_END);
			if (bRet)
			{
				bRet = AtlSmtpSendAndWait(hFile, currBuffer, nEncodedLength, pOverlapped);
			}
		}

		return bRet;
	}
}; // class CMimeFileAttachment


// CMimeRawAttachment represents a file attachment MIME body part.
// The data provided is not a file, but a blob of raw data.
class CMimeRawAttachment : public CMimeAttachment
{
protected:
	//the raw data
	void* m_pvRaw;
	
	//the length
	DWORD m_dwLength;

	//whether or not we own it
	BOOL  m_bShared;

public:
	CMimeRawAttachment() throw()
		:m_bShared(FALSE), m_pvRaw(NULL), m_dwLength(0)
	{
	}

	~CMimeRawAttachment() throw()
	{
		//If we own the raw data, free it
		if (!m_bShared && m_pvRaw)
			free(m_pvRaw);
	}

	ATL_NOINLINE CMimeBodyPart* Copy()
	{
		CMimeRawAttachment* pNewAttachment = NULL;
		ATLTRY(pNewAttachment = new CMimeRawAttachment);
		if (pNewAttachment)
			*pNewAttachment = *this;

		return pNewAttachment;
	}

	const CMimeRawAttachment& operator=(const CMimeRawAttachment& that)
	{
		if (this != &that)
		{
			CMimeAttachment::operator=(that);
			if (!m_bShared && m_pvRaw)
				free(m_pvRaw);

			m_bShared = that.m_bShared;
			m_dwLength = that.m_dwLength;

			if (m_bShared)
			{
				m_pvRaw = that.m_pvRaw;
			}
			else
			{
				m_pvRaw = malloc(m_dwLength);
				if (m_pvRaw)
					memcpy(m_pvRaw, that.m_pvRaw, m_dwLength);
			}
		}

		return *this;
	}

	// Initialize the attachment
	// pData - the data
	// nDataLength - the size of pData in BYTEs
	// bCopyData - flag specifying whether CMimeRawAttachment should make a copy of the data (optional)
	// pMultiLanguage - the IMultiLanguage pointer for codepage to character set conversion (optional)
	// uiCodePage - the codepage (optional)
	inline BOOL Initialize(void* pData, DWORD nDataLength, BOOL bCopyData = TRUE, LPCTSTR lpszDisplayName = NULL, 
		IMultiLanguage* pMultiLanguage = NULL, UINT uiCodePage = 0) throw()
	{
		// if we're already attached to some data, and it's not shared, free it
		if (m_pvRaw && !m_bShared)
			free(m_pvRaw);

		m_dwLength = nDataLength;
		if (bCopyData)
		{
			if (m_pvRaw != NULL && !m_bShared)
				free(m_pvRaw);

			m_pvRaw = malloc(sizeof(BYTE)*m_dwLength);
			if (!m_pvRaw)
			{
				return FALSE;
			}
			memcpy(m_pvRaw, pData, m_dwLength);
			m_bShared = FALSE;
		}
		else
		{
			m_pvRaw = pData;
			m_bShared = TRUE;
		}

		if (!AtlMimeCharsetFromCodePage(m_szCharset, uiCodePage, pMultiLanguage))
			return FALSE;
		
		if (lpszDisplayName)
		{
			// use the user-specified display name
			_tcscpy(m_szDisplayName, lpszDisplayName);
		}
		else
		{
			// no display name
			*m_szDisplayName = '\0';
		}
		return TRUE;
	}

	// Output the data--similar to CFileAttachment::DumpData
	// See CFileAttachment::DumpData for comments
	inline BOOL DumpData(HANDLE hFile, LPOVERLAPPED pOverlapped, LPCSTR lpszBoundary, DWORD dwFlags = 0) throw()
	{
		if (!m_pvRaw)
			return FALSE;

		int nLineLength = 0, nRequiredLength = 0;
		if (!GetEncodingInformation(m_szEncodeString, &nRequiredLength, &nLineLength))
			return FALSE;

		CStringA header;
		MakeMimeHeader(header, lpszBoundary);
		if (!AtlSmtpSendAndWait(hFile, ((LPCSTR)header), header.GetLength(), pOverlapped))
		{
			return FALSE;
		}

		int nGetLines = ATLSMTP_GET_LINES;
		DWORD dwCurrChunk = 0;
		nRequiredLength *= nGetLines;
		DWORD dwToGet = (DWORD)nGetLines*nLineLength;
		int nDestLen = nRequiredLength;
		BOOL bRet = FALSE;
		DWORD dwRead = 0;
#ifdef ATLSMTP_DOUBLE_BUFFERED
		CHeapPtr<char> buffer1;
		if (!buffer1.Allocate(nRequiredLength+3))
			return FALSE;

		CHeapPtr<char> buffer2;
		if (!buffer2.Allocate(nRequiredLength+3))
			return FALSE;

		char* currBuffer = buffer1;
		char* prevBuffer = NULL;
		int nCurrBuffer = 0;
		DWORD dwPrevLength = 0;
#else
		CHeapPtr<char> currBuffer;
		if (!currBuffer.Allocate(nRequiredLength+3))
			return FALSE;
#endif // ATLSMTP_DOUBLE_BUFFERED

		do 
		{
			if ((m_dwLength-dwRead) <= dwToGet)
				dwCurrChunk = m_dwLength-dwRead;
			else
				dwCurrChunk = dwToGet;
			switch(m_nEncodingScheme)
			{
				case ATLSMTP_BASE64_ENCODE:
					bRet = Base64Encode(((LPBYTE)(m_pvRaw))+dwRead, dwCurrChunk, currBuffer, &nDestLen, 
						(dwRead < m_dwLength) ? ATL_BASE64_FLAG_NONE : ATL_BASE64_FLAG_NOPAD);
					currBuffer[nDestLen++] = '\r';
					currBuffer[nDestLen++] = '\n';
					break;
				case ATLSMTP_UUENCODE:
					bRet = UUEncode(((LPBYTE)(m_pvRaw))+dwRead, dwCurrChunk, currBuffer, &nDestLen, _T("rawdata"), 
						            (dwRead > 0 ? 0 : ATLSMTP_UUENCODE_HEADER) | 
						            (dwRead+dwCurrChunk == m_dwLength ? ATLSMTP_UUENCODE_END : 0) | 
						            ((dwFlags & ATLSMTP_FORMAT_SMTP) ? ATLSMTP_UUENCODE_DOT : 0));
					break;
				case ATLSMTP_QP_ENCODE:
					bRet = QPEncode(((LPBYTE)(m_pvRaw))+dwRead, dwCurrChunk, currBuffer, &nDestLen, 
						            ((dwFlags & ATLSMTP_FORMAT_SMTP) ? ATLSMTP_QPENCODE_DOT : 0) | 
						            (dwRead+dwCurrChunk == m_dwLength ? 0 : ATLSMTP_QPENCODE_TRAILING_SOFT));
					break;
			}
			if (!bRet)
				break;
#ifdef ATLSMTP_DOUBLE_BUFFERED
			bRet = AtlSmtpSendOverlapped(hFile, currBuffer, nDestLen, prevBuffer, dwPrevLength, pOverlapped);
			dwPrevLength = (DWORD)nDestLen;
			prevBuffer = currBuffer;
			currBuffer = (nCurrBuffer == 0 ? buffer2 : buffer1);
			nCurrBuffer = (nCurrBuffer == 0 ? 1 : 0);
#else
			bRet = AtlSmtpSendAndWait(hFile, currBuffer, nDestLen, pOverlapped);
#endif // ATLSMTP_DOUBLE_BUFFERED

			nDestLen = nRequiredLength;
			dwRead += dwCurrChunk;
		} while (bRet && (dwRead < m_dwLength));

		//ensure all data is sent from prevBuffer
#ifdef ATLSMTP_DOUBLE_BUFFERED
		DWORD dwWritten = 0, dwErr = 0;
		if (!GetOverlappedResult(hFile, pOverlapped, &dwWritten, TRUE))
		{
			if ((dwErr = GetLastError()) != ERROR_IO_PENDING && dwErr != ERROR_IO_INCOMPLETE)
				bRet = FALSE;
			else if (dwWritten < dwPrevLength)
				bRet = AtlSmtpSendAndWait(hFile, prevBuffer+dwWritten, dwPrevLength-dwWritten, pOverlapped);
		}
#endif // ATLSMTP_DOUBLE_BUFFERED

		//for uuencoding, if the last chunk read was of size dwToGet, but it was also the end of the file,
		//the "end" keyword will not get encoded, so a check is necessary
		if (m_nEncodingScheme == ATLSMTP_UUENCODE && dwCurrChunk == dwToGet)
		{
			bRet = UUEncode((LPBYTE)m_pvRaw, 0, currBuffer, &nDestLen, _T("rawdata"), (dwFlags & ATLSMTP_FORMAT_SMTP ? ATLSMTP_UUENCODE_DOT : 0) |
				ATLSMTP_UUENCODE_END);
			if (bRet)
				bRet = AtlSmtpSendAndWait(hFile, currBuffer, nDestLen, pOverlapped);
		}
		return bRet;
	}
}; // class CMimeRawAttachment


// CMimeText - represents a text body part in MIME body
class CMimeText : public CMimeBodyPart
{
protected:

	// the text
	LPSTR   m_szText;

	// the character set
	char     m_szCharset[ATL_MAX_ENC_CHARSET_LENGTH];

	// the text length
	int      m_nTextLen;

public:
	CMimeText() throw()
		:m_nTextLen(0), m_szText(NULL)
	{
		strcpy(m_szCharset, ATLSMTP_DEFAULT_CSET);
	}

	virtual ~CMimeText() throw()
	{
		if (m_szText)
			free(m_szText);
	}

	// Get the content type
	inline BOOL GetContentType(LPSTR lpszContentType) throw()
	{
		memcpy(lpszContentType, "text/plain", sizeof("text/plain"));
		return TRUE;
	}

	// Get the character set
	inline BOOL GetCharset(LPSTR lpszCharacterSet) throw()
	{
		strcpy(lpszCharacterSet, m_szCharset);
		return TRUE;
	}

	ATL_NOINLINE CMimeBodyPart* Copy()
	{
		CMimeText* pNewText = NULL;
		ATLTRY(pNewText = new CMimeText);
		if (pNewText)
			*pNewText = *this;

		return pNewText;
	}

	const CMimeText& operator=(const CMimeText& that)
	{
		if (this != &that)
		{
			m_nTextLen = that.m_nTextLen;
			strcpy(m_szCharset, that.m_szCharset);
			if (m_szText)
				free(m_szText);
			m_szText = (CHAR*)malloc(m_nTextLen);
			if (m_szText)
				memcpy(m_szText, that.m_szText, m_nTextLen);
		}

		return *this;
	}

	// Initialize the body part
	// lpszText - the text (required)
	// nTextLen - the text length in bytes (optional--if not specified a _tcslen will be done)
	// pMultiLanguage - the IMultiLanguagte pointer for converting codepages to MIME character sets (optional)
	// uiCodePage - the codepage
	inline BOOL Initialize(LPCTSTR lpszText, int nTextLen = -1, IMultiLanguage* pMultiLanguage = NULL, UINT uiCodePage = 0) throw()
	{
		BOOL bRet = TRUE;

		// if IMultiLanguage is there, respect the codepage
		if (pMultiLanguage)
		{
			CHeapPtr<char> pszText;
			UINT nLen(0);

			BOOL bRet = AtlMimeConvertString(pMultiLanguage, uiCodePage, lpszText, &pszText, &nLen);
			if (bRet)
			{
				if (m_szText != NULL)
					free(m_szText);

				m_szText = pszText.Detach();
				m_nTextLen = nLen;
			}
		}
		else // no multilanguage support
		{
			if (nTextLen < 0)
			{
				nTextLen = (int) _tcslen(lpszText);
				nTextLen*= sizeof(TCHAR);
			}
			
			if (m_szText != NULL)
				free(m_szText);

			m_szText = (LPSTR)malloc(nTextLen);
			if (!m_szText)
				return FALSE;
			memcpy((void*)m_szText, (void*)lpszText, nTextLen);
			m_nTextLen = nTextLen;
		}

		if (bRet)
		{
			bRet = AtlMimeCharsetFromCodePage(m_szCharset, uiCodePage, pMultiLanguage);
		}

		return bRet;
	}

	// Dump the data to hFile
	inline BOOL DumpData(HANDLE hFile, LPOVERLAPPED pOverlapped, LPCSTR lpszBoundary, DWORD dwFlags = 0) throw()
	{
		CStringA header;
		char sendBuffer[ATLSMTP_READBUFFER_SIZE];
		LPSTR pSendBuffer = sendBuffer;
		LPSTR lpszText = m_szText;

		MakeMimeHeader(header, lpszBoundary);

		//copy the header into the sendbuffer
		int nWritten = header.GetLength();
		memcpy(pSendBuffer, (LPCSTR)header, nWritten);
		pSendBuffer+= nWritten;
		int nRead = 0;
		int nLineLen = 0;

		//subtract 2 from these because it's easier for when we have
		//to break lines with a CRLF
		int nMaxLineLength = ATLSMTP_MAX_LINE_LENGTH-2;
		int nMaxBufferSize = ATLSMTP_READBUFFER_SIZE-2;
		while (nRead <= m_nTextLen)
		{
			//if the buffer is full or we've reached the end of the text, 
			//send it
			if (nWritten >= nMaxBufferSize || nRead == m_nTextLen)
			{
				if (!AtlSmtpSendAndWait(hFile, sendBuffer, nWritten, pOverlapped))
					return FALSE;
				nWritten = 0;
				pSendBuffer = sendBuffer;
			}

			//if we're at the end of the line, break it
			if (nLineLen == nMaxLineLength)
			{
				*pSendBuffer++ = '\r';
				*pSendBuffer++ = '\n';
				nWritten+= 2;
				nLineLen = -1;
				continue;
			}

			//stuff dots at the start of the line
			if (nLineLen == 0 && (dwFlags & ATLSMTP_FORMAT_SMTP) && *lpszText == '.')
			{
				*pSendBuffer++ = '.';
				nWritten++;
				nLineLen++;
				continue;
			}

			//if we hit a CRLF, reset nLineLen
			if (*lpszText == '\n' && nRead > 0 && *(lpszText-1) == '\r')
				nLineLen = -1;

			*pSendBuffer++ = (BYTE)(*lpszText++);
			nRead++;
			nWritten++;
			nLineLen++;
		}

		return TRUE;
	}

protected:

	// Make the MIME header
	inline BOOL MakeMimeHeader(CStringA& header, LPCSTR pszBoundary) throw()
	{
		char szBegin[256];
		if (*pszBoundary)
		{
			// this is not the only body part
			memcpy(szBegin, "\r\n\r\n--", 6);
			memcpy(szBegin+6, pszBoundary, 32);
			*(szBegin+38) = '\0';
		}
		else
		{
			// this is the only body part, so output the full MIME header
			memcpy(szBegin, "MIME-Version: 1.0", sizeof("MIME-Version: 1.0"));
		}

		header.Format("%s\r\nContent-Type: text/plain;\r\n\tcharset=\"%s\"\r\nContent-Transfer-Encoding: 8bit\r\n\r\n", 
			szBegin, m_szCharset);
		return TRUE;
	}
}; // class CMimeText


// CMimeMessage - the MIME message class.  Represents a full MIME message
class CMimeMessage : public CMimeHeader
{
protected:

	// The list of the MIME body parts
	CAtlList<CMimeBodyPart*> m_BodyParts;

	// The display name of the message
	char m_szDisplayName[MAX_PATH+1];

public:
	CMimeMessage(IMultiLanguage *pMultiLanguage = NULL) throw()
	{
		Initialize(pMultiLanguage);
		memcpy(m_szDisplayName, "email", sizeof("email"));
	}

	virtual ~CMimeMessage() throw()
	{
		RemoveParts();
	}

	void RemoveParts() throw()
	{
		POSITION currPos = m_BodyParts.GetHeadPosition();

		// free the body parts
		while (currPos != NULL)
		{
			delete m_BodyParts.GetNext(currPos);
		}
	}


	ATL_NOINLINE CMimeBodyPart* Copy()
	{
		CMimeMessage* pNewMessage = NULL;
		ATLTRY(pNewMessage = new CMimeMessage);
		if (pNewMessage)
			*pNewMessage = *this;

		return pNewMessage;
	}


	const CMimeMessage& operator=(const CMimeMessage& that)
	{
		if (this != &that)
		{
			CMimeHeader::operator=(that);
			strcpy(m_szDisplayName, that.m_szDisplayName);

			RemoveParts();
			POSITION pos = that.m_BodyParts.GetHeadPosition();
			while (pos != NULL)
			{
				CMimeBodyPart* pCopy = that.m_BodyParts.GetNext(pos)->Copy();
				if (pCopy)
					m_BodyParts.AddTail(pCopy);
			}
		}

		return *this;
	}

	// Set the display name of the message
	inline BOOL SetDisplayName(LPCTSTR lpszDisplayName)
	{
		if (!lpszDisplayName || _tcslen(lpszDisplayName) > MAX_PATH)
		{
			return FALSE;
		}

		strcpy(m_szDisplayName, CT2CA(lpszDisplayName));
		return TRUE;
	}

	// Add some text to the message at position nPos in the body parts list
	// lpszText - the text
	// nTextLen - the size of the text in bytes (optional - if not specified a _tcslen will be done)
	// nPos - the position in the message at which to insert the text (optional)
	// uiCodePage - the codepage (optional)
	inline BOOL AddText(LPCTSTR lpszText, int nTextLen = -1, int nPos = 1, UINT uiCodePage = 0)
	{
		if (!lpszText)
			return FALSE;

		if (nPos < 1)
		{
			nPos = 1;
		}

		CMimeText* pNewText = NULL;
		ATLTRY(pNewText = new CMimeText());
		if (!pNewText)
			return FALSE;

		BOOL bRet = pNewText->Initialize(lpszText, nTextLen, m_spMultiLanguage, uiCodePage);

		POSITION currPos = m_BodyParts.FindIndex(nPos-1);
		if (bRet)
		{
			if (!currPos)
			{
				if (!m_BodyParts.AddTail(pNewText))
					bRet = FALSE;
			}
			else
			{
				if (!m_BodyParts.InsertBefore(currPos, pNewText))
					bRet = FALSE;
			}
		}

		if (!bRet)
			delete pNewText;
		return bRet;
	}

	// Dump the data
	virtual BOOL DumpData(HANDLE hFile, LPOVERLAPPED pOverlapped, LPCSTR pszBoundary=NULL, DWORD dwFlags = 0) throw()
	{	
		// Make the MIME boundary for this message
		char szBoundary[35];
		MakeBoundary(szBoundary);

		// if the passed boundary is valid, this is an attached message
		if (pszBoundary && *pszBoundary != '\0')
		{
			// output the MIME header for a message attachment
			CStringA strHeader;
			strHeader.Format("\r\n\r\n--%s\r\nContent-Type: message/rfc822\r\n\tname=\"%s\"\r\nContent-Transfer-Encoding: 8bit\r\n"
			    "Content-Disposition: attachment;\r\n\tfilename=\"%s\"\r\n\r\n", 
				pszBoundary, m_szDisplayName, m_szDisplayName);

			if (!AtlSmtpSendAndWait(hFile, ((LPCSTR)strHeader), strHeader.GetLength(), pOverlapped))
			{
				return FALSE;
			}
		}

		if (!CMimeHeader::DumpData(hFile, pOverlapped, szBoundary, dwFlags))
			return FALSE;

		// Create and output the header
		CStringA strHeader;
		MakeMimeHeader(strHeader, szBoundary);
		if (!AtlSmtpSendAndWait(hFile, ((LPCSTR)strHeader), strHeader.GetLength(), pOverlapped))
		{
			return FALSE;
		}

		CMimeBodyPart* pCurrPart;
		POSITION currPos = m_BodyParts.GetHeadPosition();

		//Dump the body parts
		while (currPos != NULL)
		{
			pCurrPart = m_BodyParts.GetAt(currPos);
			if (!pCurrPart->DumpData(hFile, pOverlapped, szBoundary, dwFlags))
			{
				return FALSE;
			}
			m_BodyParts.GetNext(currPos);
		}

		char szBuf[50];
		//output a trailing boundary
		if (*szBoundary)
		{
			int nBufLen = sprintf(szBuf, "\r\n\r\n--%s--\r\n", szBoundary);
			if (!AtlSmtpSendAndWait(hFile, szBuf, nBufLen, pOverlapped))
			{
				return FALSE;
			}
		}

		return TRUE;
	}

	// Attach a file.
	// lpszFileName - the filename
	// lpszDisplayName - the display name (optional)
	// lpszContentType - the content type (optional - defaults to NULL -- lookup will be attempted, otherwise default to application/octet-stream)
	// nEncodingScheme - the encoding scheme to use for the attachment (optional - defaults to base64
	// uiCodePage - the codepage (optional)
	inline BOOL AttachFile(LPCTSTR lpszFileName, LPCTSTR lpszDisplayName = NULL, LPCTSTR lpszContentType = NULL, 
		int nEncodingScheme = ATLSMTP_BASE64_ENCODE, UINT uiCodepage = 0)
	{
		if (!lpszFileName)
			return FALSE;

		CMimeFileAttachment* pFileAttach = NULL;
		ATLTRY(pFileAttach = new CMimeFileAttachment());
		if (!pFileAttach)
			return FALSE;

		BOOL bRet = pFileAttach->Initialize(lpszFileName, lpszDisplayName, m_spMultiLanguage, uiCodepage);

		if (bRet)
			bRet = pFileAttach->SetEncodingScheme(nEncodingScheme);

		if (bRet)
		{
			DWORD dwErr = ERROR_PATH_NOT_FOUND;
			CString strContentType;
			// if content-type is not specified
			if (!lpszContentType)
			{
				// get the file extension
				TCHAR szExt[_MAX_EXT];
				_tsplitpath(lpszFileName, NULL, NULL, NULL, szExt);
				if (*szExt)
				{
					// Query the content type from the registry
					CRegKey rkContentType;
					dwErr = rkContentType.Open(HKEY_CLASSES_ROOT, szExt, KEY_READ);
					if (dwErr == ERROR_SUCCESS)
					{
						ULONG nChars=0;
						dwErr = rkContentType.QueryStringValue(_T("Content Type"), NULL, &nChars);
						if (dwErr == ERROR_SUCCESS)
						{
							LPTSTR pszBuf = strContentType.GetBuffer(nChars);
							dwErr = rkContentType.QueryStringValue(_T("Content Type"), pszBuf, &nChars);
							if (dwErr == ERROR_SUCCESS)
							{
								lpszContentType = pszBuf;
							}
							strContentType.ReleaseBuffer(nChars);
						}
					}
				}
			}
			
			if (dwErr != ERROR_SUCCESS)
			{
				// default to application/octet-stream
				lpszContentType = _T("application/octet-stream");
			}

			bRet = pFileAttach->SetContentType(lpszContentType);
		}
		if (bRet)
			if (!m_BodyParts.AddTail(pFileAttach))
				bRet = FALSE;
		if (!bRet)
			delete pFileAttach;
		return bRet;
	}

	// Attach some raw data
	// pRawData - the data
	// nDataLength - the size of the data in bytes
	// nEncodingScheme - the encoding scheme to use for the attachment (optional - defaults to base64
	// uiCodePage - the codepage (optional)
	inline BOOL AttachRaw(void* pRawData, DWORD dwDataLength, int nEncodingScheme = ATLSMTP_BASE64_ENCODE, BOOL bCopyData = TRUE, 
		LPCTSTR lpszDisplayName = NULL, LPCTSTR lpszContentType = _T("application/octet-stream"), UINT uiCodepage = 0)
	{
		if (!pRawData)
			return FALSE;

		CMimeRawAttachment* pRawAttach = NULL;
		ATLTRY(pRawAttach = new CMimeRawAttachment());
		if (!pRawAttach)
		{
			return FALSE;
		}

		BOOL bRet = pRawAttach->Initialize(pRawData, dwDataLength, bCopyData, lpszDisplayName, m_spMultiLanguage, uiCodepage);

		if (bRet)
			bRet = pRawAttach->SetEncodingScheme(nEncodingScheme);
		if (bRet)
			bRet = pRawAttach->SetContentType(lpszContentType);
		if (bRet)
			if(!m_BodyParts.AddTail(pRawAttach))
				bRet = FALSE;
		if (!bRet)
			delete pRawAttach;
		return bRet;
	}

	// Attach a CMimeMessage
	// pMsg - pointer to the Msg object
	inline BOOL AttachMessage(CMimeMessage* pMsg)
	{
		if (!pMsg)
			return FALSE;

		if (!m_BodyParts.AddTail(pMsg->Copy()))
			return FALSE;

		return TRUE;
	}

protected:
	// Make the MIME header
	inline BOOL MakeMimeHeader(CStringA& header, LPCSTR pszBoundary) throw()
	{
		if (!*pszBoundary)
		{
			header.Format("X-Priority: %d\r\n%s", m_nPriority, (LPCSTR) m_XHeader);
		}
		else if (m_BodyParts.GetCount() > 1)
		{
			header.Format("X-Priority: %d\r\n%sMIME-Version: 1.0\r\nContent-Type: multipart/mixed;\r\n\tboundary=\"%s\"\r\n", 
				m_nPriority, (LPCSTR) m_XHeader, pszBoundary);
		}
		return TRUE;
	}

	// Make the MIME boundary
	inline BOOL MakeBoundary(LPSTR pszBoundary) throw()
	{
		if (m_BodyParts.GetCount() < 2)
		{
			*pszBoundary = '\0';
		}
		else 
		{
			sprintf(pszBoundary, "------=_Next_Part_%.10u.%.3u", GetTickCount(), rand()%1000);
		}
		return TRUE;
	}

}; // class CMimeMessage

} // namespace ATL