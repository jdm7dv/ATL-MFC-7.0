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

#include <atlstr.h>
#include <atlsiface.h>


namespace ATL {

#define TAGF_NONE 0
#define TAGF_HASEND 1
#define TAGF_BLOCK  2


struct ATL_HTML_TAG
{
	LPCTSTR szTagName;
	UINT uFlags;
};

enum ATL_HTML_TAGS {
	ATL_HTML_TAG_BODY,
	ATL_HTML_TAG_A,
	ATL_HTML_TAG_B,
	ATL_HTML_TAG_I,
	ATL_HTML_TAG_U,
	ATL_HTML_TAG_FONT,
	ATL_HTML_TAG_IMG,
	ATL_HTML_TAG_HR,
	ATL_HTML_TAG_BR,
	ATL_HTML_TAG_DIV,
	ATL_HTML_TAG_BLOCKQUOTE,
	ATL_HTML_TAG_ADDRESS,
	ATL_HTML_TAG_P,
	ATL_HTML_TAG_H1,
	ATL_HTML_TAG_H2,
	ATL_HTML_TAG_H3,
	ATL_HTML_TAG_H4,
	ATL_HTML_TAG_H5,
	ATL_HTML_TAG_H6,
	ATL_HTML_TAG_PRE,
	ATL_HTML_TAG_Q,
	ATL_HTML_TAG_SUB,
	ATL_HTML_TAG_SUP,
	ATL_HTML_TAG_INS,
	ATL_HTML_TAG_DEL,
	ATL_HTML_TAG_EM,
	ATL_HTML_TAG_STRONG,
	ATL_HTML_TAG_DFN,
	ATL_HTML_TAG_CODE,
	ATL_HTML_TAG_SAMP,
	ATL_HTML_TAG_KBD,
	ATL_HTML_TAG_VAR,
	ATL_HTML_TAG_CITE,
	ATL_HTML_TAG_ABBR,
	ATL_HTML_TAG_ACRONYM,
	ATL_HTML_TAG_OL,
	ATL_HTML_TAG_UL,
	ATL_HTML_TAG_LI,
	ATL_HTML_TAG_DL,
	ATL_HTML_TAG_DT,
	ATL_HTML_TAG_DD,
	ATL_HTML_TAG_TABLE,
	ATL_HTML_TAG_TR,
	ATL_HTML_TAG_TD,
	ATL_HTML_TAG_FORM,
	ATL_HTML_TAG_INPUT,
	ATL_HTML_TAG_SELECT,
	ATL_HTML_TAG_OPTION,
	ATL_HTML_TAG_HEAD,
	ATL_HTML_TAG_HTML,
	ATL_HTML_TAG_MAP,
	ATL_HTML_TAG_AREA,
	ATL_HTML_TAG_BASE,
	ATL_HTML_TAG_BDO,
	ATL_HTML_TAG_BIG,
	ATL_HTML_TAG_BUTTON,
	ATL_HTML_TAG_IFRAME,
	ATL_HTML_TAG_LABEL,
	ATL_HTML_TAG_LINK,
	ATL_HTML_TAG_META,
	ATL_HTML_TAG_NOFRAMES,
	ATL_HTML_TAG_NOSCRIPT,
	ATL_HTML_TAG_COL,
	ATL_HTML_TAG_COLGROUP,
	ATL_HTML_TAG_FIELDSET,
	ATL_HTML_TAG_LEGEND,
	ATL_HTML_TAG_TBODY,
	ATL_HTML_TAG_TEXTAREA,
	ATL_HTML_TAG_TFOOT,
	ATL_HTML_TAG_TH,
	ATL_HTML_TAG_TITLE,
	ATL_HTML_TAG_TT,
	ATL_HTML_TAG_SMALL,
	ATL_HTML_TAG_SPAN,
	ATL_HTML_TAG_LAST };

// todo: add th and the rest of the table stuff
__declspec(selectany) ATL_HTML_TAG s_tags[] = 
{
	{ "body",  TAGF_HASEND | TAGF_BLOCK },
	{ "a", TAGF_HASEND },
	{ "b", TAGF_HASEND },
	{ "i", TAGF_HASEND },
	{ "u", TAGF_HASEND },
	{ "font", TAGF_HASEND },
	{ "img", TAGF_NONE },
	{ "hr", TAGF_NONE },
	{ "br", TAGF_NONE },
	{ "div", TAGF_HASEND | TAGF_BLOCK },
	{ "blockquote", TAGF_HASEND | TAGF_BLOCK },
	{ "adress", TAGF_HASEND },
	{ "p", TAGF_HASEND | TAGF_BLOCK },
	{ "h1", TAGF_HASEND | TAGF_BLOCK},
	{ "h2", TAGF_HASEND  | TAGF_BLOCK},
	{ "h3", TAGF_HASEND | TAGF_BLOCK },
	{ "h4", TAGF_HASEND | TAGF_BLOCK },
	{ "h5", TAGF_HASEND | TAGF_BLOCK },
	{ "h6", TAGF_HASEND | TAGF_BLOCK },
	{ "pre", TAGF_HASEND | TAGF_BLOCK },
	{ "q", TAGF_HASEND },
	{ "sub", TAGF_HASEND },
	{ "sup", TAGF_HASEND },
	{ "ins", TAGF_HASEND },
	{ "del", TAGF_HASEND },
	{ "em", TAGF_HASEND },
	{ "strong", TAGF_HASEND },
	{ "dfn", TAGF_HASEND },
	{ "code", TAGF_HASEND },
	{ "samp", TAGF_HASEND },
	{ "kbd", TAGF_HASEND },
	{ "var", TAGF_HASEND },
	{ "cite", TAGF_HASEND },
	{ "abbr", TAGF_HASEND },
	{ "acronym", TAGF_HASEND },
	{ "ol", TAGF_HASEND | TAGF_BLOCK },
	{ "ul", TAGF_HASEND | TAGF_BLOCK },
	{ "li", TAGF_HASEND },
	{ "dl", TAGF_HASEND | TAGF_BLOCK },
	{ "dt", TAGF_HASEND },
	{ "dd", TAGF_HASEND },
	{ "table", TAGF_HASEND },
	{ "tr", TAGF_HASEND },
	{ "td", TAGF_HASEND },
	{ "form", TAGF_HASEND },
	{ "input", TAGF_HASEND },
	{ "select", TAGF_HASEND },
	{ "option", TAGF_HASEND },
	{ "head", TAGF_HASEND | TAGF_BLOCK },
	{ "html", TAGF_HASEND | TAGF_BLOCK },
	{ "map", TAGF_HASEND | TAGF_BLOCK },
	{ "area", TAGF_BLOCK },
	{ "base", TAGF_BLOCK },
	{ "bdo", TAGF_HASEND },
	{ "big", TAGF_HASEND },
	{ "button", TAGF_HASEND },
	{ "iframe", TAGF_HASEND },
	{ "label", TAGF_HASEND },
	{ "link", TAGF_NONE },
	{ "meta", TAGF_BLOCK },
	{ "noframes", TAGF_BLOCK },
	{ "noscript", TAGF_BLOCK },
	{ "col", TAGF_BLOCK },
	{ "colgroup", TAGF_HASEND | TAGF_BLOCK },
	{ "fieldset", TAGF_HASEND | TAGF_BLOCK },
	{ "legend", TAGF_HASEND | TAGF_BLOCK },
	{ "tbody", TAGF_HASEND | TAGF_BLOCK },
	{ "textarea", TAGF_HASEND | TAGF_BLOCK },
	{ "tfoot", TAGF_HASEND | TAGF_BLOCK },
	{ "th", TAGF_HASEND | TAGF_BLOCK },
	{ "title", TAGF_HASEND | TAGF_BLOCK },
	{ "tt", TAGF_HASEND },
	{ "small", TAGF_HASEND },
	{ "span", TAGF_HASEND },
};

class AtlHtmlAttrs
{
public:
	CString m_strAttrs;

	AtlHtmlAttrs()
	{

	}

	AtlHtmlAttrs(int nCount, ...)
	{
		va_list args;

		va_start(args, nCount);
		for (int i=0; i<nCount; i++)
		{
			LPCTSTR szName = va_arg(args, LPCTSTR);
			LPCTSTR szVal = va_arg(args, LPCTSTR);
			Add(szName, szVal);
		}
		va_end(args);
	}

	AtlHtmlAttrs(LPCTSTR szFormat, ...)
	{
		if (!szFormat || !*szFormat)
			return;

		va_list args;

		va_start(args, szFormat);

		CString strTmp;
		strTmp.FormatV(szFormat, args);
		va_end(args);

		m_strAttrs += " ";
		m_strAttrs += strTmp;
	}

	BOOL Add(LPCTSTR szName, LPCTSTR szValue)
	{
		if (szValue)
			m_strAttrs.AppendFormat(" %s=\"%s\"", szName, szValue);
		else
			m_strAttrs.AppendFormat(" %s", szName);
		return TRUE;
	}

	void AddFormat(LPCTSTR szFormat, ...)
	{
		va_list args;

		va_start(args, szFormat);

		CString strTmp;
		strTmp.FormatV(szFormat, args);
		va_end(args);

		m_strAttrs += " ";
		m_strAttrs += strTmp;
	}

	void Set(LPCTSTR szAttrs)
	{
		if (szAttrs)
		{
			m_strAttrs.Empty();
			if (!isspace(szAttrs[0]))
				m_strAttrs = " ";
			m_strAttrs += szAttrs;
		}
	}

	operator LPCTSTR()
	{
		return m_strAttrs;
	}

};


class CStreamFormatter
{
protected:
	IStream *m_pStream;

public:
	BOOL m_bAddCRLF;

	CStreamFormatter()
	{
		m_pStream = NULL;
		m_bAddCRLF = TRUE;
	}

	void Initialize(IStream *pStream, BOOL bAddCRLF=TRUE)
	{
		m_pStream = pStream;
		m_bAddCRLF = bAddCRLF;
	}

	HRESULT WriteRaw(LPCTSTR szString, int nCount=-1)
	{
		ATLASSERT(szString != NULL);
		if (!m_pStream)
			return E_FAIL;

		if (nCount == -1)
			nCount = (int) _tcslen(szString);
		DWORD dwWritten;
		return m_pStream->Write(szString, (DWORD) nCount, &dwWritten);
	}

	HRESULT StartTag(int nTagIndex, LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		if (nTagIndex < 0 || nTagIndex >= ATL_HTML_TAG_LAST)
			return E_INVALIDARG;
		if (m_bAddCRLF && (s_tags[nTagIndex].uFlags & TAGF_BLOCK))
			WriteRaw("\r\n");
		HRESULT hr = StartTag(s_tags[nTagIndex].szTagName, szContent, szAttrs);
		if (FAILED(hr))
			return hr;
		if (m_bAddCRLF && (s_tags[nTagIndex].uFlags & TAGF_BLOCK))
			WriteRaw("\r\n");
		return S_OK;
	}

	HRESULT StartTag(LPCTSTR szTag, LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		HRESULT hr;
		hr = WriteRaw("<");
		if (FAILED(hr))
			return hr;
		hr = WriteRaw(szTag);
		if (FAILED(hr))
			return hr;
		hr = WriteAttributes(szAttrs);
		if (FAILED(hr))
			return hr;
		hr = WriteRaw(">");
		if (FAILED(hr))
			return hr;
		if (szContent && *szContent)
		{
			WriteRaw(szContent);
			EndTag(szTag);
		}
		return S_OK;
	}

	HRESULT EndTag(int nTagIndex)
	{
		if (nTagIndex < 0 || nTagIndex >= ATL_HTML_TAG_LAST)
			return E_INVALIDARG;
		if (m_bAddCRLF && (s_tags[nTagIndex].uFlags & TAGF_BLOCK))
			WriteRaw("\r\n");
		HRESULT hr = EndTag(s_tags[nTagIndex].szTagName);
		if (FAILED(hr))
			return hr;
		if (m_bAddCRLF && (s_tags[nTagIndex].uFlags & TAGF_BLOCK))
			WriteRaw("\r\n");
		return S_OK;
	}

	HRESULT EndTag(LPCTSTR szTag)
	{
		HRESULT hr = WriteRaw("</");
		if (FAILED(hr))
			return hr;
		hr = WriteRaw(szTag);
		if (FAILED(hr))
			return hr;
		return WriteRaw(">");
	}


	HRESULT WriteAttributes(LPCTSTR szAttrs)
	{
		if (szAttrs && szAttrs[0])
			return WriteRaw(szAttrs);

		return S_OK;
	}

	HRESULT Write(LPCTSTR szString, int nCount=-1)
	{
		ATLASSERT(szString != NULL);
		if (!m_pStream)
			return E_FAIL;

		if (nCount == -1)
			nCount = (int) _tcslen(szString);
		DWORD dwWritten;
		return m_pStream->Write(szString, (DWORD) nCount, &dwWritten);
	}

	HRESULT Write(int i)
	{
		char tmp[21];
		return Write(tmp, sprintf(tmp, _T("%d"), i));
	}

	HRESULT WriteFormatted(LPCTSTR szFormat, ...)
	{
		ATLASSERT(szFormat != NULL);
		if (!m_pStream)
			return E_FAIL;

		va_list args;
		va_start(args, szFormat);

		TCHAR szTmp[1024];
		int nCount = vsprintf(szTmp, szFormat, args);
		va_end(args);

		DWORD dwWritten;
		return m_pStream->Write(szTmp, (DWORD) nCount, &dwWritten);
	}
};

template <typename TData, int nMax=64>
class CSimpleStack
{
public:
	int m_nTop;
	TData m_Data[nMax];

	CSimpleStack()
	{
		m_nTop = -1;
	}

	bool IsEmpty()
	{
		return (m_nTop == -1);
	}

	bool Push(const TData *pData)
	{
		if (m_nTop >= nMax)
			return false;

		m_nTop++;

		m_Data[m_nTop] = *pData;
		return true;
	}

	bool Pop(TData *pData)
	{
		if (m_nTop < 0)
			return false;

		*pData = m_Data[m_nTop];
		m_nTop--;
		return true;
	}
};


struct HTML_SCHEME
{
	CString strBgColor;
	CString strLinkColor;
	CString strVLinkColor;
	CString strALinkColor;
	CString strBackground;
	int nTopMargin;
	int nLeftMargin;
	
	CString strTdBgColor;
	CString strTableBgColor;
	CString strTrBgColor;

	HTML_SCHEME()
	{
		nTopMargin = -1;
		nLeftMargin = -1;
	}
};

template <class T>
class CHtmlGenBase : public CStreamFormatter
{
public:
	CString m_strState;
	HTML_SCHEME *m_pScheme;

	T* GetOuter()
	{
		return static_cast<T*>(this);
	}

	struct TableState
	{
		TableState() : m_bRowOpen(false), m_bDataOpen(false)
		{

		}

		void Clear()
		{
			m_bRowOpen = false;
			m_bDataOpen = false;
		}

		bool m_bRowOpen;
		bool m_bDataOpen;
	};

	int m_nWidthPercent;
	int m_nHeightPercent;

	enum ATL_HTML_FORM_METHOD { ATL_HTML_FORM_METHOD_NONE=-1, ATL_HTML_FORM_METHOD_GET, ATL_HTML_FORM_METHOD_POST, ATL_HTML_FORM_METHOD_MULTIPART };
	ATL_HTML_FORM_METHOD m_nFormMethod;

	TableState m_tableState;
	CSimpleStack<TableState> m_RowStack;


	CHtmlGenBase()
	{
		m_nWidthPercent = -1;
		m_nHeightPercent = -1;
		m_nFormMethod = ATL_HTML_FORM_METHOD_NONE;
		m_pScheme = NULL;
	}

	void SetScheme(HTML_SCHEME *pScheme)
	{
		m_pScheme = pScheme;
	}
	HRESULT body(LPCTSTR szBgColor=NULL, LPCTSTR szBackground=NULL, LPCTSTR szTopMargin=NULL, LPCTSTR szLeftMargin=NULL,
		LPCTSTR szAttrs=NULL)
	{
		AtlHtmlAttrs Attrs;
		Attrs.Set(szAttrs);

		if (szBgColor && *szBgColor)
			Attrs.Add("bgColor", szBgColor);
		else if (m_pScheme && m_pScheme->strBgColor.GetLength())
			Attrs.Add("bgColor", m_pScheme->strBgColor);

		if (szBackground && *szBackground)
			Attrs.Add("background", szBackground);
		else if (m_pScheme && m_pScheme->strBackground.GetLength())
			Attrs.Add("background", m_pScheme->strBackground);

		if (m_pScheme && m_pScheme->strLinkColor.GetLength())
			Attrs.Add("link", m_pScheme->strLinkColor);

		if (m_pScheme && m_pScheme->strALinkColor.GetLength())
			Attrs.Add("alink", m_pScheme->strALinkColor);

		if (m_pScheme && m_pScheme->strVLinkColor.GetLength())
			Attrs.Add("vlink", m_pScheme->strVLinkColor);

		if (szTopMargin && *szTopMargin)
			Attrs.Add("topmargin", szTopMargin);
		else if (m_pScheme && m_pScheme->nTopMargin != -1)
			Attrs.AddFormat("topmargin=\"%d\"", m_pScheme->nTopMargin);

		if (szLeftMargin && *szLeftMargin)
			Attrs.Add("leftmargin", szLeftMargin);
		else if (m_pScheme && m_pScheme->nLeftMargin != -1)
			Attrs.AddFormat("leftmargin=\"%d\"", m_pScheme->nLeftMargin);

		return GetOuter()->StartTag(ATL_HTML_TAG_BODY, NULL, Attrs);

	}

	HRESULT bodyEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_BODY);
	}

	HRESULT a(LPCTSTR szHref, LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		AtlHtmlAttrs Attrs;
		Attrs.Set(szAttrs);

		if (m_strState.GetLength()==0)
		{
			Attrs.Add("href", szHref);
			return GetOuter()->StartTag(ATL_HTML_TAG_A, szContent, Attrs);
		}

		const TCHAR *szQuestion = _tcschr(szHref, '?');
		CString strHref = szHref;
		if (!szQuestion)
			strHref.Append("?");
		else
			strHref.Append("&");

		strHref += m_strState;

		Attrs.Add("href", strHref);

		return GetOuter()->StartTag(ATL_HTML_TAG_A, szContent, Attrs);
	}

	HRESULT aEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_A);
	}

	HRESULT b(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_B, szContent, szAttrs);
	}

	HRESULT bEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_B);
	}

	HRESULT i(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_I, szContent, szAttrs);
	}

	HRESULT iEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_I);
	}

	HRESULT u(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_U, szContent, szAttrs);
	}

	HRESULT uEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_U);
	}

	HRESULT font(LPCTSTR szFace, LPCTSTR szSize=NULL, LPCTSTR szColor=NULL, LPCTSTR szAttrs=NULL)
	{
		AtlHtmlAttrs Attrs;
		Attrs.Set(szAttrs);
		if (szFace && *szFace)
			Attrs.Add("face", szFace);
		if (szSize && *szSize)
			Attrs.Add("size", szSize);
		if (szColor && *szColor)
			Attrs.Add("color", szColor);
		return GetOuter()->StartTag(ATL_HTML_TAG_FONT, NULL, Attrs);
	}

	HRESULT font(COLORREF clrColor, LPCTSTR szAttrs=NULL)
	{
		TCHAR szColor[8];
		sprintf(szColor, "#%02x%02x%02x", GetRValue(clrColor), GetGValue(clrColor),
			GetBValue(clrColor));
		return GetOuter()->font(NULL, NULL, szColor, szAttrs);
	}

	HRESULT fontEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_FONT);
	}

	HRESULT img(LPCTSTR szSrc, LPCTSTR szAttrs=NULL)
	{
		AtlHtmlAttrs Attrs;
		Attrs.Set(szAttrs);

		Attrs.Add("src", szSrc);

		return GetOuter()->StartTag(ATL_HTML_TAG_IMG, NULL, Attrs);
	}

	HRESULT br(LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_BR, NULL, szAttrs);
	}

	HRESULT hr(LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_HR, NULL, szAttrs);
	}

	HRESULT div(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_DIV, szContent, szAttrs);
	}

	HRESULT divEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_DIV);
	}

	HRESULT blockquote(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_BLOCKQUOTE, szContent, szAttrs);
	}

	HRESULT blockquoteEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_BLOCKQUOTE);
	}

	HRESULT address(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_ADDRESS, szContent, szAttrs);
	}

	HRESULT addressEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_ADDRESS);
	}

	HRESULT p(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_P, szContent, szAttrs);
	}

	HRESULT pEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_P);
	}

	HRESULT h(int nLevel=1, LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		if (nLevel < 1 || nLevel > 6)
			return E_INVALIDARG;
		return GetOuter()->StartTag(ATL_HTML_TAG_H1+nLevel-1, szContent, szAttrs);
	}

	HRESULT hEnd(int nLevel=1)
	{
		if (nLevel < 1 || nLevel > 6)
			return E_INVALIDARG;
		return GetOuter()->EndTag(ATL_HTML_TAG_H1+nLevel-1);
	}

	HRESULT pre(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_PRE, szContent, szAttrs);
	}

	HRESULT preEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_PRE);
	}

	HRESULT q(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_Q, szContent, szAttrs);
	}

	HRESULT qEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_Q);
	}

	HRESULT sub(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_SUB, szContent, szAttrs);
	}

	HRESULT subEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_SUB);
	}

	HRESULT sup(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_SUP, szContent, szAttrs);
	}

	HRESULT supEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_SUP);
	}

	//TODO: should handle the time and date part of this
	HRESULT ins(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_INS, szContent, szAttrs);
	}

	HRESULT insEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_INS);
	}

	HRESULT del(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_DEL, szContent, szAttrs);
	}

	HRESULT delEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_DEL);
	}


	HRESULT em(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_EM, szContent, szAttrs);
	}

	HRESULT emEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_EM);
	}

	HRESULT strong(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_STRONG, szContent, szAttrs);
	}

	HRESULT strongEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_STRONG);
	}

	HRESULT dfn(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_DFN, szContent, szAttrs);
	}

	HRESULT dfnEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_DFN);
	}

	HRESULT code(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_CODE, szContent, szAttrs);
	}

	HRESULT codeEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_CODE);
	}

	HRESULT samp(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_SAMP, szContent, szAttrs);
	}

	HRESULT sampEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_SAMP);
	}

	HRESULT kbd(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_KBD, szContent, szAttrs);
	}

	HRESULT kbdEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_KBD);
	}

	HRESULT var(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_VAR, szContent, szAttrs);
	}

	HRESULT varEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_VAR);
	}

	HRESULT cite(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_CITE, szContent, szAttrs);
	}

	HRESULT citeEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_CITE);
	}

	HRESULT abbr(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_ABBR, szContent, szAttrs);
	}

	HRESULT abbrEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_ABBR);
	}

	HRESULT acronym(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_ACRONYM, szContent, szAttrs);
	}

	HRESULT acronymEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_ACRONYM);
	}


	HRESULT ol(LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_OL, NULL, szAttrs);
	}

	HRESULT ul(LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_UL, NULL, szAttrs);
	}

	HRESULT olEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_OL);
	}
	
	HRESULT ulEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_UL);
	}

	HRESULT li(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_LI, szContent, szAttrs);
	}

	HRESULT liEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_LI);
	}

	HRESULT dl(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_DL, szContent, szAttrs);
	}

	HRESULT dlEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_DL);
	}

	HRESULT dt(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_DT, szContent, szAttrs);
	}

	HRESULT dtEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_DT);
	}

	HRESULT dd(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_DD, szContent, szAttrs);
	}

	HRESULT ddEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_DD);
	}

	void SetSizePercent(int nWidth, int nHeight)
	{
		m_nWidthPercent = nWidth;
		m_nHeightPercent = nHeight;
	}

	HRESULT table(int nBorderWidth=0, LPCTSTR szAttrs=NULL)
	{
		AtlHtmlAttrs Attrs;
		Attrs.Set(szAttrs);

		m_RowStack.Push(&m_tableState);
		m_tableState.Clear();

		Attrs.AddFormat("border=\"%d\"", nBorderWidth);
		
		if (m_nWidthPercent != -1)
			Attrs.AddFormat("width=\"%d%%\"", m_nWidthPercent);
		if (m_nHeightPercent != -1)
			Attrs.AddFormat("height=\"%d%%\"", m_nHeightPercent);

		if (m_pScheme && m_pScheme->strTableBgColor.GetLength())
			Attrs.Add("bgcolor", m_pScheme->strTableBgColor);

		m_nWidthPercent = -1;
		m_nHeightPercent = -1;
		return GetOuter()->StartTag(ATL_HTML_TAG_TABLE, NULL, Attrs);
	}

	HRESULT tableEnd()
	{
		if (m_tableState.m_bRowOpen)
			GetOuter()->trEnd();
		m_RowStack.Pop(&m_tableState);
		return GetOuter()->EndTag(ATL_HTML_TAG_TABLE);
	}

	HRESULT tr(LPCTSTR szAttrs=NULL)
	{
		AtlHtmlAttrs Attrs;
		Attrs.Set(szAttrs);

		if (m_RowStack.IsEmpty())
			GetOuter()->table();
		if (m_tableState.m_bRowOpen)
			GetOuter()->trEnd();
		m_tableState.m_bRowOpen = true;

		if (m_pScheme && m_pScheme->strTrBgColor.GetLength())
			Attrs.Add("bgcolor", m_pScheme->strTrBgColor);
		return GetOuter()->StartTag(ATL_HTML_TAG_TR, NULL, Attrs);
	}

	HRESULT td(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		AtlHtmlAttrs Attrs;
		Attrs.Set(szAttrs);

		if (!m_tableState.m_bRowOpen)
			GetOuter()->tr();
		m_tableState.m_bDataOpen = true;
		if (m_pScheme && m_pScheme->strTdBgColor.GetLength())
			Attrs.Add("bgColor", m_pScheme->strTdBgColor);

		HRESULT hr = GetOuter()->StartTag(ATL_HTML_TAG_TD, szContent, Attrs);
		if (FAILED(hr))
			return hr;
		if (szContent)
			m_tableState.m_bDataOpen = false;
		return S_OK;
	}

	HRESULT tdEnd()
	{
		if (!m_tableState.m_bDataOpen)
			return S_OK;
		m_tableState.m_bDataOpen = false;
		return GetOuter()->EndTag(ATL_HTML_TAG_TD);
	}

	HRESULT trEnd()
	{
		if (!m_tableState.m_bRowOpen)
			return S_OK;
		if (m_tableState.m_bDataOpen)
			GetOuter()->tdEnd();
		m_tableState.m_bRowOpen = false;
		return GetOuter()->EndTag(ATL_HTML_TAG_TR);
	}

	// TODO: should do this the other way around, so we don't have to compare strings
	// aka, the one that takes the enum calls the one that takes a string
	HRESULT form(LPCTSTR szAction, ATL_HTML_FORM_METHOD nMethod=ATL_HTML_FORM_METHOD_GET, LPCTSTR szAttrs=NULL)
	{
		static LPCTSTR s_szFormMethods[] = { "get", "post", "multipart-www-url-encoded" };

		AtlHtmlAttrs Attrs;
		Attrs.Set(szAttrs);

		Attrs.Add("action", szAction);
		Attrs.Add("method", s_szFormMethods[nMethod]);

		return GetOuter()->StartTag(ATL_HTML_TAG_FORM, NULL, Attrs);
	}

	HRESULT form(LPCTSTR szAction, LPCTSTR szMethod)
	{
		ATL_HTML_FORM_METHOD nMethod;
		if (!_tcsicmp(szMethod, "GET"))
			nMethod = ATL_HTML_FORM_METHOD_GET;
		else if (!_tcsicmp(szMethod, "POST"))
			nMethod = ATL_HTML_FORM_METHOD_POST;
		else if (!_tcsicmp(szMethod, "MULTIPART"))
			nMethod = ATL_HTML_FORM_METHOD_MULTIPART;
		else
			return E_INVALIDARG;
		return GetOuter()->form(szAction, nMethod);
	}

	HRESULT input(LPCTSTR szType, LPCTSTR szName, LPCTSTR szValue, LPCTSTR szAttrs=NULL)
	{
		AtlHtmlAttrs Attrs;
		Attrs.Set(szAttrs);

		Attrs.Add("type", szType);
		if (szName)
			Attrs.Add("name", szName);
		if (szValue)
			Attrs.Add("value", szValue);
		return GetOuter()->StartTag(ATL_HTML_TAG_INPUT, NULL, Attrs);
	}

	HRESULT submit(LPCTSTR szValue=NULL, LPCTSTR szName=NULL, LPCTSTR szAttrs=NULL)
	{
		return input("submit", szName, szValue, szAttrs);
	}

	HRESULT formEnd()
	{
//		if (GetOuter()->m_strState.GetLength())
//		{
//			GetOuter()->WriteFormatted("<input type=\"hidden\" name=\"state\" value=\"%s\">\r\n",
//				(LPCTSTR) GetOuter()->m_strState);
//		}
		return GetOuter()->EndTag(ATL_HTML_TAG_FORM);
	}


	HRESULT select(LPCTSTR szName, BOOL bMultiple=FALSE, LPCTSTR szAttrs=NULL)
	{
		AtlHtmlAttrs Attrs;
		Attrs.Set(szAttrs);

		Attrs.Add("name", szName);
		if (bMultiple)
			Attrs.Add("multiple", NULL);
		return GetOuter()->StartTag(ATL_HTML_TAG_SELECT, NULL, Attrs);
	}

	HRESULT option(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_OPTION, szContent, szAttrs);
	}

	HRESULT optionEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_OPTION);
	}

	HRESULT selectEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_SELECT);
	}


	HRESULT head(LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_HEAD, NULL, szAttrs);
	}

	HRESULT headEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_HEAD);
	}

	HRESULT html(LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_HTML, NULL, szAttrs);
	}

	HRESULT htmlEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_HTML);
	}

	HRESULT map(LPCTSTR szName, LPCTSTR szAttrs=NULL)
	{
		AtlHtmlAttrs Attrs;
		Attrs.Set(szAttrs);

		Attrs.Add("name", szName);
		return GetOuter()->StartTag(ATL_HTML_TAG_MAP, NULL, Attrs);
	}

	HRESULT mapEnd(LPCTSTR szAttrs=NULL)
	{
		szAttrs; // unused
		return GetOuter()->EndTag(ATL_HTML_TAG_MAP);
	}

	HRESULT area(LPCTSTR szAlt, LPCTSTR szHref=NULL, LPCTSTR szAttrs=NULL)
	{
		AtlHtmlAttrs Attrs;
		Attrs.Set(szAttrs);

		Attrs.Add("alt", szAlt);
		if (szHref && *szHref)
			Attrs.Add("href", szHref);
		return GetOuter()->StartTag(ATL_HTML_TAG_AREA, NULL, Attrs);
	}

	HRESULT base(LPCTSTR szHref, LPCTSTR szAttrs=NULL)
	{
		AtlHtmlAttrs Attrs;
		Attrs.Set(szAttrs);

		Attrs.Add("href", szHref);
		return GetOuter()->StartTag(ATL_HTML_TAG_BASE, NULL, Attrs);
	}

	HRESULT bdo(LPCTSTR szDir, LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		AtlHtmlAttrs Attrs;
		Attrs.Set(szAttrs);

		Attrs.Add("dir", szDir);
		return GetOuter()->StartTag(ATL_HTML_TAG_BDO, szContent, Attrs);
	}

	HRESULT bdoEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_BDO);
	}

	HRESULT big(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_BIG, szContent, szAttrs);
	}

	HRESULT bigEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_BIG);
	}

	HRESULT button(LPCTSTR szName=NULL, LPCTSTR szValue=NULL, LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		AtlHtmlAttrs Attrs;
		Attrs.Set(szAttrs);

		if (szName && *szName)
			Attrs.Add("name", szName);
		if (szValue && *szValue)
			Attrs.Add("value", szValue);
		return GetOuter()->StartTag(ATL_HTML_TAG_BUTTON, szContent, Attrs);
	}

	HRESULT buttonEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_BUTTON);
	}

	HRESULT iframe(LPCTSTR szSrc=NULL, LPCTSTR szWidth=NULL, LPCTSTR szHeight=NULL, LPCTSTR szAttrs=NULL)
	{
		AtlHtmlAttrs Attrs;
		Attrs.Set(szAttrs);

		if (szSrc && *szSrc)
			Attrs.Add("src", szSrc);
		if (szWidth && *szWidth)
			Attrs.Add("width", szWidth);
		if (szHeight && *szHeight)
			Attrs.Add("height", szHeight);
		return GetOuter()->StartTag(ATL_HTML_TAG_IFRAME, NULL, Attrs);
	}

	HRESULT iframeEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_IFRAME);
	}

	HRESULT label(LPCTSTR szFor=NULL, LPCTSTR szAccessKey=NULL, LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		AtlHtmlAttrs Attrs;
		Attrs.Set(szAttrs);

		if (szFor && *szFor)
			Attrs.Add("for", szFor);
		if (szAccessKey && *szAccessKey)
			Attrs.Add("accesskey", szAccessKey);
		return GetOuter()->StartTag(ATL_HTML_TAG_LABEL, szContent, Attrs);
	}

	HRESULT labelEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_LABEL);
	}

	HRESULT link(LPCTSTR szRel=NULL, LPCTSTR szHref=NULL, LPCTSTR szAttrs=NULL)
	{
		AtlHtmlAttrs Attrs;
		Attrs.Set(szAttrs);

		if (szRel && *szRel)
			Attrs.Add("rel", szRel);
		if (szHref && *szHref)
			Attrs.Add("href", szHref);
		return GetOuter()->StartTag(ATL_HTML_TAG_LINK, NULL, Attrs);
	}

	HRESULT linkEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_LINK);
	}

	HRESULT meta(LPCTSTR szName=NULL, LPCTSTR szContent=NULL, LPCTSTR szHttpEquiv=NULL, LPCTSTR szAttrs=NULL)
	{
		AtlHtmlAttrs Attrs;
		Attrs.Set(szAttrs);

		if (szName && *szName)
			Attrs.Add("name", szName);
		if (szContent && *szContent)
			Attrs.Add("content", szContent);
		if (szHttpEquiv && *szHttpEquiv)
			Attrs.Add("http-equiv", szHttpEquiv);
		return GetOuter()->StartTag(ATL_HTML_TAG_META, NULL, Attrs);
	}

	HRESULT meta()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_META);
	}

	HRESULT noframes(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_NOFRAMES, szContent, szAttrs);
	}

	HRESULT noframesEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_NOFRAMES);
	}

	HRESULT noscript(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_NOSCRIPT, szContent, szAttrs);
	}

	HRESULT noscript()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_NOSCRIPT);
	}

	HRESULT col(int nSpan=1, LPCTSTR szWidth=NULL, LPCTSTR szHeight=NULL, LPCTSTR szVAlign=NULL,
		LPCTSTR szHAlign=NULL, LPCTSTR szAttrs=NULL)
	{
		AtlHtmlAttrs Attrs;
		Attrs.Set(szAttrs);


		if (nSpan != 1)
			Attrs.AddFormat("span", "\"%d\"", nSpan);
		if (szWidth && *szWidth)
			Attrs.Add("width", szWidth);
		if (szHeight && *szHeight)
			Attrs.Add("height", szHeight);
		if (szVAlign && *szVAlign)
			Attrs.Add("valign", szVAlign);
		if (szHAlign && *szHAlign)
			Attrs.Add("align", szHAlign);
		return GetOuter()->StartTag(ATL_HTML_TAG_COL, NULL, Attrs);
	}

	HRESULT colgroup(int nSpan=1, LPCTSTR szWidth=NULL, LPCTSTR szHeight=NULL, LPCTSTR szVAlign=NULL,
		LPCTSTR szHAlign=NULL, LPCTSTR szAttrs=NULL)
	{
		AtlHtmlAttrs Attrs;
		Attrs.Set(szAttrs);

		if (nSpan != 1)
			Attrs.AddFormat("span", "\"%d\"", nSpan);
		if (szWidth && *szWidth)
			Attrs.Add("width", szWidth);
		if (szHeight && *szHeight)
			Attrs.Add("height", szHeight);
		if (szVAlign && *szVAlign)
			Attrs.Add("valign", szVAlign);
		if (szHAlign && *szHAlign)
			Attrs.Add("align", szHAlign);
		return GetOuter()->StartTag(ATL_HTML_TAG_COL, NULL, Attrs);
	}

	HRESULT colgroupEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_COLGROUP);
	}

	HRESULT fieldset(LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_FIELDSET, NULL, szAttrs);
	}

	HRESULT fieldsetEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_FIELDSET);
	}

	HRESULT legend(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_LEGEND, szContent, szAttrs);
	}

	HRESULT legendEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_LEGEND);
	}

	HRESULT tbody(LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_TBODY, NULL, szAttrs);
	}

	HRESULT tbodyEnd(LPCTSTR szAttrs=NULL)
	{
		szAttrs; // unused
		return GetOuter()->StartTag(ATL_HTML_TAG_TBODY);
	}

	HRESULT tfoot(LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_TFOOT, NULL, szAttrs);
	}

	HRESULT tfootEnd(LPCTSTR szAttrs=NULL)
	{
		szAttrs; // unused
		return GetOuter()->StartTag(ATL_HTML_TAG_TFOOT);
	}

	HRESULT th(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		if (!m_tableState.m_bRowOpen)
			GetOuter()->tr();
		m_tableState.m_bDataOpen = true;
		return GetOuter()->StartTag(ATL_HTML_TAG_TH, szContent, szAttrs);
	}

	HRESULT thEnd()
	{
		ATLASSERT(m_tableState.m_bDataOpen);
		m_tableState.m_bDataOpen = false;
		return GetOuter()->EndTag(ATL_HTML_TAG_TH);
	}

	HRESULT title(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_TITLE, szContent, szAttrs);
	}

	HRESULT tt(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_TT, szContent, szAttrs);
	}

	HRESULT ttEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_TT);
	}

	// unfortunately, we can't use small since it is defined as char
	// in rpcndr.h!
	HRESULT _small(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_SMALL, szContent, szAttrs);
	}

	HRESULT _smallEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_SMALL);
	}

	HRESULT span(LPCTSTR szContent=NULL, LPCTSTR szAttrs=NULL)
	{
		return GetOuter()->StartTag(ATL_HTML_TAG_SPAN, szContent, szAttrs);
	}

	HRESULT spanEnd()
	{
		return GetOuter()->EndTag(ATL_HTML_TAG_SPAN);
	}
};

class CHtmlGen : public CHtmlGenBase<CHtmlGen>
{
public:
};

class CStreamOnWriteStream : public IStream
{
public:
	IWriteStream *m_pWriteStream;

	CStreamOnWriteStream()
	{
		m_pWriteStream = NULL;
	}

	void Init(IWriteStream *pWriteStream)
	{
		m_pWriteStream = pWriteStream;
	}

	// IUnknown methods
	STDMETHOD(QueryInterface)(REFIID riid, void **ppv)
	{
		if (!ppv)
			return E_POINTER;

		*ppv = NULL;

		if (IsEqualGUID(riid, IID_IUnknown) ||
			IsEqualGUID(riid, IID_IStream) ||
			IsEqualGUID(riid, IID_ISequentialStream))
		{
			*ppv = (IStream *) this;
		}
		if (!*ppv)
			return E_NOINTERFACE;
		return S_OK;
	}

	ULONG __stdcall AddRef()
	{
		return 1;
	}

	ULONG __stdcall Release()
	{
		return 1;
	}

	// ISequentialStream methods
    HRESULT STDMETHODCALLTYPE Read(void * /*pDest*/, ULONG /*dwMaxLen*/, ULONG * /*pdwRead*/)
	{
		return E_NOTIMPL;
	}

    HRESULT STDMETHODCALLTYPE Write(const void *pv, ULONG cb, ULONG *pcbWritten)
	{
		ATLASSERT(m_pWriteStream);
		HRESULT hr = m_pWriteStream->WriteStream((const char *) pv, cb, pcbWritten);
		return (hr==S_OK) ? S_OK : STG_E_WRITEFAULT;
	}
	
	// IStream methods
    HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER /*dlibMove*/, DWORD /*dwOrigin*/, ULARGE_INTEGER * /*plibNewPosition*/)
	{
		return E_NOTIMPL;
	}

    HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER /*libNewSize*/) 
	{
		return E_NOTIMPL;
	}
    
    HRESULT STDMETHODCALLTYPE CopyTo(IStream * /*pstm*/, ULARGE_INTEGER /*cb*/, ULARGE_INTEGER * /*pcbRead*/, ULARGE_INTEGER * /*pcbWritten*/)
	{
		return E_NOTIMPL;
	}

    HRESULT STDMETHODCALLTYPE Commit(DWORD /*grfCommitFlags*/)
	{
		return E_NOTIMPL;
	}
    
    HRESULT STDMETHODCALLTYPE Revert(void)
	{
		return E_NOTIMPL;
	}
    
    HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER /*libOffset*/, ULARGE_INTEGER /*cb*/, DWORD /*dwLockType*/)
	{
		return E_NOTIMPL;
	}
		
    HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER /*libOffset*/, ULARGE_INTEGER /*cb*/, DWORD /*dwLockType*/)
	{
		return E_NOTIMPL;
	}
    
    HRESULT STDMETHODCALLTYPE Stat(STATSTG * /*pstatstg*/, DWORD /*grfStatFlag*/)
	{
		return E_NOTIMPL;
	}

    
    HRESULT STDMETHODCALLTYPE Clone(IStream ** /*ppstm*/)
	{
		return E_NOTIMPL;
	}
};

} // namespace ATL
