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

#if (defined(_WINSOCKAPI_) && !defined(_WINSOCK2API_))
	#error <atlsoap.h> requires <winsock2.h> -- include <winsock2.h> before you include <windows.h> or <winsock.h>
#endif

#if ((_WIN32_WINNT < 0x0400) && (_WIN32_WINDOWS <= 0x0400))
	#error <atlsoap.h> requires _WIN32_WINNT >= 0x0400 or _WIN32_WINDOWS > 0x0400
#endif

[ emitidl("restricted") ];

#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <float.h>
#include <atlstr.h>
#include <atlcoll.h>
#include <atlisapi.h>
#include <atlstencil.h>
#include <atlutil.h>
#include <atlhttp.h>
#include <atlenc.h>
#include <msxml.h>

#ifndef ATLSOAP_MAX_ID_LEN
	#define ATLSOAP_MAX_ID_LEN 256
#endif

typedef CAtlMap<CStringW, CComPtr<IXMLDOMNode>, CStringElementTraits<CStringW> > ATL_ID_HASH;

namespace ATL
{

class CStreamOnServerContext : public IStream
{
public:
	IHttpServerContext *m_pServerContext;
	DWORD m_dwBytesRead;

	CStreamOnServerContext(IHttpServerContext *pServerContext) throw()
	{
		m_pServerContext = pServerContext;
		m_dwBytesRead = 0;
	}

	// IUnknown methods
	STDMETHOD(QueryInterface)(REFIID riid, void **ppv) throw()
	{
		if (!ppv)
			return E_POINTER;

		*ppv = NULL;

		if (InlineIsEqualGUID(riid, IID_IUnknown) ||
			InlineIsEqualGUID(riid, IID_IStream) ||
			InlineIsEqualGUID(riid, IID_ISequentialStream))
		{
			*ppv = (IStream *) this;
		}
		if (!*ppv)
			return E_NOINTERFACE;
		return S_OK;
	}

	ULONG __stdcall AddRef() throw()
	{
		return 1;
	}

	ULONG __stdcall Release() throw()
	{
		return 1;
	}

	// ISequentialStream methods
    HRESULT STDMETHODCALLTYPE Read(void *pDest, ULONG dwMaxLen, ULONG *pdwRead) throw()
	{
		ATLASSERT(pDest != NULL);

		ATLASSERT(m_pServerContext != NULL);

		DWORD dwToRead = min(m_pServerContext->GetTotalBytes(), dwMaxLen);
		BOOL bRet = ReadClientData(m_pServerContext, (LPSTR) pDest, &dwToRead, m_dwBytesRead);
		m_dwBytesRead += dwToRead;
		if (!bRet)
			return E_FAIL;

		if (pdwRead)
			*pdwRead = dwToRead;
		return S_OK;
	}

    HRESULT STDMETHODCALLTYPE Write(const void * /*pv*/, ULONG /*cb*/, ULONG * /*pcbWritten*/) throw()
	{
		return E_NOTIMPL;
	}
	
	// IStream methods
    HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER /*dlibMove*/, DWORD /*dwOrigin*/, ULARGE_INTEGER * /*plibNewPosition*/) throw()
	{
		return E_NOTIMPL;
	}

    HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER /*libNewSize*/) throw()
	{
		return E_NOTIMPL;
	}
    
    HRESULT STDMETHODCALLTYPE CopyTo(IStream * /*pstm*/, ULARGE_INTEGER /*cb*/, ULARGE_INTEGER * /*pcbRead*/, ULARGE_INTEGER * /*pcbWritten*/) throw()
	{
		return E_NOTIMPL;
	}

    HRESULT STDMETHODCALLTYPE Commit(DWORD /*grfCommitFlags*/) throw()
	{
		return E_NOTIMPL;
	}
    
    HRESULT STDMETHODCALLTYPE Revert(void) throw()
	{
		return E_NOTIMPL;
	}
    
    HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER /*libOffset*/, ULARGE_INTEGER /*cb*/, DWORD /*dwLockType*/) throw()
	{
		return E_NOTIMPL;
	}
		
    HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER /*libOffset*/, ULARGE_INTEGER /*cb*/, DWORD /*dwLockType*/) throw()
	{
		return E_NOTIMPL;
	}
    
    HRESULT STDMETHODCALLTYPE Stat(STATSTG * /*pstatstg*/, DWORD /*grfStatFlag*/) throw()
	{
		return E_NOTIMPL;
	}

    HRESULT STDMETHODCALLTYPE Clone(IStream ** /*ppstm*/) throw()
	{
		return E_NOTIMPL;
	}
};

inline HRESULT GetAttribute(IXMLDOMNode *pNode, LPCWSTR szAttrName, BSTR *pbstrVal) throw()
{
	if (!pNode || !pbstrVal || !szAttrName)
		return E_INVALIDARG;

	*pbstrVal = NULL;
	CComPtr<IXMLDOMNamedNodeMap> spAttrs;

	HRESULT hr = pNode->get_attributes(&spAttrs);
	if (hr != S_OK)
		return hr;
	
	CComPtr<IXMLDOMNode> spAttr;
	
	CComBSTR bstrAttrName(szAttrName);
	
	hr = spAttrs->getNamedItem(bstrAttrName, &spAttr);
	if (hr != S_OK)
		return hr;
	
	CComVariant varVal;
	hr = spAttr->get_nodeValue(&varVal);
	if (hr != S_OK)
		return hr;
	
	hr = varVal.ChangeType(VT_BSTR);
	if (hr != S_OK)
		return hr;

	*pbstrVal = varVal.bstrVal;
	varVal.vt = VT_EMPTY;

	return S_OK;
}


inline HRESULT BuildHash(IXMLDOMNode *pNode, ATL_ID_HASH *pidHash) throw()
{
	if (!pNode || !pidHash)
		return E_INVALIDARG;

	CComBSTR bstrId;

	HRESULT hr = GetAttribute(pNode, L"id", &bstrId);
	if (bstrId)
	{
		pidHash->SetAt(CStringW(bstrId), pNode);
	}
	else
	{
		return hr;
	}

	CComPtr<IXMLDOMNode> spChild;
	CComPtr<IXMLDOMNode> spSibling;

	pNode->get_firstChild(&spChild);
	if (spChild)
	{
		hr = BuildHash(spChild, pidHash);
		if (FAILED(hr))
			return hr;
	}

	pNode->get_nextSibling(&spSibling);
	if (!spSibling)
		return S_OK;
	return BuildHash(spSibling, pidHash);
}

inline HRESULT GetNodeById(LPCWSTR szId, IXMLDOMNode **ppNode, ATL_ID_HASH *pidHash) throw()
{
	if (!szId || !pidHash)
		return E_INVALIDARG;

	if (!ppNode)
		return E_POINTER;

	*ppNode = NULL;
	CComPtr<IXMLDOMNode> spNode;
	bool bFound = pidHash->Lookup(szId, spNode);
	if (bFound)
	{
		*ppNode = spNode.Detach();
		return S_OK;
	}
	return E_FAIL;
}


inline HRESULT BeginParse(IXMLDOMDocument **ppDoc, IXMLDOMNode **ppHeader, IXMLDOMNode **ppMethod, ATL_ID_HASH *pidHash, IStream *pStream) throw()
{
	if (!pStream || !pidHash)
		return E_INVALIDARG;

	if (!ppDoc || !ppHeader || !ppMethod)
		return E_POINTER;

	*ppDoc = NULL;
	*ppHeader = NULL;
	*ppMethod= NULL;

	CComPtr<IXMLDOMDocument> spdoc;

	HRESULT hr = CoCreateInstance(__uuidof(DOMDocument), NULL, CLSCTX_INPROC, __uuidof(IXMLDOMDocument), (void **) &spdoc);
	if (FAILED(hr))
		return hr;

	spdoc->put_async(VARIANT_FALSE);

	CComPtr<IPersistStreamInit> spSI;
	hr = spdoc->QueryInterface(&spSI);
	if (!spSI)
		return hr;
	hr = spSI->Load(pStream);
	if (FAILED(hr))
		return hr;

	CComPtr<IXMLDOMElement> spRoot;

	hr = spdoc->get_documentElement(&spRoot);
	if (!spRoot)
	{
		return E_FAIL;
	}

	// hack: build a hash table of all elements that have ids
	BuildHash(spRoot, pidHash);

	// check for the envelope
	CComBSTR bstrName;
	hr = spRoot->get_baseName(&bstrName);
	if (!bstrName)
		return E_FAIL;

	if (wcscmp(bstrName, L"Envelope"))
	{
		return E_FAIL;
	}

	CComPtr<IXMLDOMNode> spHeader;
	CComPtr<IXMLDOMNode> spBody;

	hr = spRoot->get_firstChild(&spHeader);
	if (FAILED(hr) || !spHeader)
	{
		return E_FAIL;
	}

	bstrName.Empty();
	hr = spHeader->get_baseName(&bstrName);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	if (!wcscmp(bstrName, L"Header"))
	{
		// get the body
		spHeader->get_nextSibling(&spBody);

		bstrName.Empty();
		hr = spBody->get_baseName(&bstrName);
		if (FAILED(hr))
			return hr;

		if (wcscmp(bstrName, L"Body"))
		{
			return E_INVALIDARG;
		}
	}
	else if (!wcscmp(bstrName, L"Body"))
	{
		spBody = spHeader;
		spHeader.Release();
	}
	else
	{
		return E_FAIL;
	}

	CComPtr<IXMLDOMNode> spMethod;

	hr = spBody->get_firstChild(&spMethod);
	if (!spMethod)
	{
		return E_FAIL;
	}

	*ppDoc = spdoc.Detach();
	*ppHeader = spHeader.Detach();
	*ppMethod = spMethod.Detach();

	return S_OK;
}

inline HRESULT GetHref(IXMLDOMNode *pNode, IXMLDOMNode **ppRef, ATL_ID_HASH *pidHash) throw()
{
	if (!pNode || !pidHash)
		return E_INVALIDARG;

	if (!ppRef)
		return E_POINTER;

	*ppRef = NULL;
	
	CComBSTR bstrHref;

	HRESULT hr = GetAttribute(pNode, L"href", &bstrHref);
	if (!bstrHref)
	{
		*ppRef = pNode;
		pNode->AddRef();
		return S_OK;
	}
	
	CComPtr<IXMLDOMDocument> spDoc;
	hr = pNode->get_ownerDocument(&spDoc);
	if (FAILED(hr))
		return hr;
	
	return GetNodeById(bstrHref+1, ppRef, pidHash);
}

}

// the following template function will not compile
// this is by design to catch types that are not handled by specializations
template <typename Elemtype>
inline HRESULT AtlGetXMLValue(IXMLDOMNode * /*pParam*/, Elemtype * /*pVal*/, ATL_ID_HASH * /*pidHash*/) throw()
{
}

inline HRESULT AtlGetXMLValue<BSTR>(IXMLDOMNode *pParam, BSTR *pbstrVal, ATL_ID_HASH * /*pidHash*/) throw()
{
	if (!pParam)
		return E_INVALIDARG;

	if (!pbstrVal)
		return E_POINTER;

	CComPtr<IXMLDOMNode> spChild;
	if (pParam->get_firstChild(&spChild) == S_OK)
	{
		CComPtr<IXMLDOMNode> spXmlChild;
		if (spChild->get_firstChild(&spXmlChild) == S_OK)
			return (pParam->get_xml(pbstrVal) == S_OK ? S_OK : E_FAIL);
	}

	return (pParam->get_text(pbstrVal) == S_OK ? S_OK : E_FAIL);
}

inline HRESULT AtlGetXMLValue<VARIANT>(IXMLDOMNode *pParam, VARIANT *pvarVal, ATL_ID_HASH *pidHash) throw()
{
	if (!pParam || !pidHash)
		return E_INVALIDARG;

	if (!pvarVal)
		return E_POINTER;

	// get the value
	pvarVal->vt = VT_BSTR;
	return AtlGetXMLValue(pParam, &pvarVal->bstrVal, pidHash);
}

inline HRESULT AtlGetXMLValue<CURRENCY>(IXMLDOMNode *pParam, CURRENCY *pcyVal, ATL_ID_HASH *pidHash) throw()
{
	if (!pParam || !pidHash)
		return E_INVALIDARG;

	if (!pcyVal)
		return E_POINTER;

	CComBSTR bstrVal;
	HRESULT hr = AtlGetXMLValue(pParam, &bstrVal, pidHash);
	if (FAILED(hr))
		return hr;
		
	//REVIEW: what locale do we use?
	return VarCyFromStr(bstrVal, LOCALE_SYSTEM_DEFAULT, 0, pcyVal);
}

inline HRESULT AtlGetXMLValue<bool>(IXMLDOMNode *pParam, bool *pbVal, ATL_ID_HASH *pidHash) throw()
{
	if (!pParam || !pidHash)
		return E_INVALIDARG;

	if (!pbVal)
		return E_POINTER;

	CComBSTR bstrVal;
	HRESULT hr = AtlGetXMLValue(pParam, &bstrVal, pidHash);
	if (FAILED(hr))
		return hr;
	
	if (!wcscmp(bstrVal, L"true") || !wcscmp(bstrVal, L"1"))
		*pbVal = true;
	else if (!wcscmp(bstrVal, L"false") || !wcscmp(bstrVal, L"0"))
		*pbVal = false;
	else
		return E_FAIL; // malformed
	return S_OK;
}

inline HRESULT AtlGetXMLValue<char>(IXMLDOMNode *pParam, char *pnVal, ATL_ID_HASH *pidHash) throw()
{
	if (!pParam || !pidHash)
		return E_INVALIDARG;

	if (!pnVal)
		return E_POINTER;

	CComBSTR bstrVal;
	HRESULT hr = AtlGetXMLValue(pParam, &bstrVal, pidHash);
	if (FAILED(hr))
		return hr;
	*pnVal = (char) _wtoi(bstrVal);
	return S_OK;
}

inline HRESULT AtlGetXMLValue<unsigned char>(IXMLDOMNode *pParam, unsigned char *pnVal, ATL_ID_HASH *pidHash) throw()
{
	if (!pParam || !pidHash)
		return E_INVALIDARG;

	if (!pnVal)
		return E_POINTER;

	CComBSTR bstrVal;
	HRESULT hr = AtlGetXMLValue(pParam, &bstrVal, pidHash);
	if (FAILED(hr))
		return hr;
	*pnVal = (unsigned char) _wtoi(bstrVal);
	return S_OK;
}

inline HRESULT AtlGetXMLValue<short>(IXMLDOMNode *pParam, short *pnVal, ATL_ID_HASH *pidHash) throw()
{
	if (!pParam || !pidHash)
		return E_INVALIDARG;

	if (!pnVal)
		return E_POINTER;

	CComBSTR bstrVal;
	HRESULT hr = AtlGetXMLValue(pParam, &bstrVal, pidHash);
	if (FAILED(hr))
		return hr;
	*pnVal = (short) _wtoi(bstrVal);
	return S_OK;
}

inline HRESULT AtlGetXMLValue<unsigned short>(IXMLDOMNode *pParam, unsigned short *pnVal, ATL_ID_HASH *pidHash) throw()
{
	if (!pParam || !pidHash)
		return E_INVALIDARG;

	if (!pnVal)
		return E_POINTER;

	CComBSTR bstrVal;
	HRESULT hr = AtlGetXMLValue(pParam, &bstrVal, pidHash);
	if (FAILED(hr))
		return hr;
	*pnVal = (unsigned short) _wtoi(bstrVal);
	return S_OK;
}

inline HRESULT AtlGetXMLValue<int>(IXMLDOMNode *pParam, int *pnVal, ATL_ID_HASH *pidHash) throw()
{
	if (!pParam || !pidHash)
		return E_INVALIDARG;

	if (!pnVal)
		return E_POINTER;

	CComBSTR bstrVal;
	HRESULT hr = AtlGetXMLValue(pParam, &bstrVal, pidHash);
	if (FAILED(hr))
		return hr;
	*pnVal = _wtoi(bstrVal);
	return S_OK;
}

inline HRESULT AtlGetXMLValue<unsigned int>(IXMLDOMNode *pParam, unsigned int *pnVal, ATL_ID_HASH *pidHash) throw()
{
	if (!pParam || !pidHash)
		return E_INVALIDARG;

	if (!pnVal)
		return E_POINTER;

	CComBSTR bstrVal;
	HRESULT hr = AtlGetXMLValue(pParam, &bstrVal, pidHash);
	if (FAILED(hr))
		return hr;
	WCHAR *wszEnd;
	*pnVal = (unsigned int) wcstoul(bstrVal, &wszEnd, 10);
	return S_OK;
}

inline HRESULT AtlGetXMLValue<long>(IXMLDOMNode *pParam, long *pnVal, ATL_ID_HASH *pidHash) throw()
{
	if (!pParam || !pidHash)
		return E_INVALIDARG;

	if (!pnVal)
		return E_POINTER;

	CComBSTR bstrVal;
	HRESULT hr = AtlGetXMLValue(pParam, &bstrVal, pidHash);
	if (FAILED(hr))
		return hr;
	*pnVal = _wtol(bstrVal);
	return S_OK;
}

inline HRESULT AtlGetXMLValue<unsigned long>(IXMLDOMNode *pParam, unsigned long *pnVal, ATL_ID_HASH *pidHash) throw()
{
	if (!pParam || !pidHash)
		return E_INVALIDARG;

	if (!pnVal)
		return E_POINTER;

	CComBSTR bstrVal;
	HRESULT hr = AtlGetXMLValue(pParam, &bstrVal, pidHash);
	if (FAILED(hr))
		return hr;
	WCHAR *wszEnd;
	*pnVal = wcstoul(bstrVal, &wszEnd, 10);
	return S_OK;
}

inline HRESULT AtlGetXMLValue<__int64>(IXMLDOMNode *pParam, __int64 *pnVal, ATL_ID_HASH *pidHash) throw()
{
	if (!pParam || !pidHash)
		return E_INVALIDARG;

	if (!pnVal)
		return E_POINTER;

	CComBSTR bstrVal;
	HRESULT hr = AtlGetXMLValue(pParam, &bstrVal, pidHash);
	if (FAILED(hr))
		return hr;
	WCHAR *wszEnd;
	*pnVal = _wcstoi64(bstrVal, &wszEnd, 10);
	return S_OK;
}

inline HRESULT AtlGetXMLValue<unsigned __int64>(IXMLDOMNode *pParam, unsigned __int64 *pnVal, ATL_ID_HASH *pidHash) throw()
{
	if (!pParam || !pidHash)
		return E_INVALIDARG;

	if (!pnVal)
		return E_POINTER;

	CComBSTR bstrVal;
	HRESULT hr = AtlGetXMLValue(pParam, &bstrVal, pidHash);
	if (FAILED(hr))
		return hr;
	WCHAR *wszEnd;
	*pnVal = _wcstoui64(bstrVal, &wszEnd, 10);
	return S_OK;
}

inline HRESULT AtlGetXMLValue<double>(IXMLDOMNode *pParam, double *pdVal, ATL_ID_HASH *pidHash) throw()
{
	if (!pParam || !pidHash)
		return E_INVALIDARG;

	if (!pdVal)
		return E_POINTER;

	CComBSTR bstrVal;
	HRESULT hr = AtlGetXMLValue(pParam, &bstrVal, pidHash);
	if (FAILED(hr))
		return hr;
	if (!wcscmp(bstrVal, L"INF"))
	{
		*(((int *) pdVal)+0) = 0x0000000;
		*(((int *) pdVal)+1) = 0x7FF00000;
		return S_OK;
	}
	if (!wcscmp(bstrVal, L"-INF"))
	{
		*(((int *) pdVal)+0) = 0x0000000;
		*(((int *) pdVal)+1) = 0xFFF00000;
		return S_OK;
	}
	if (!wcscmp(bstrVal, L"NAN"))
	{
		*(((int *) pdVal)+0) = 0x0000000;
		*(((int *) pdVal)+1) = 0xFFF80000;
		return S_OK;
	}

	WCHAR *wszEnd;
	*pdVal = wcstod(bstrVal, &wszEnd);
	return S_OK;
}

inline HRESULT AtlGetXMLValue<float>(IXMLDOMNode *pParam, float *pdVal, ATL_ID_HASH *pidHash) throw()
{
	if (!pParam || !pidHash)
		return E_INVALIDARG;

	if (!pdVal)
		return E_POINTER;

	double d = *pdVal;
	HRESULT hr = AtlGetXMLValue(pParam, &d, pidHash);
	if (FAILED(hr))
		return hr;
	*pdVal = (float) d;
	return S_OK;
}

template <typename Elemtype>
inline HRESULT AtlCleanupValue(Elemtype * /*pElem*/) throw()
{
	return S_OK;
}

inline HRESULT AtlCleanupValue<BSTR>(BSTR *pElem) throw()
{
	if (!pElem)
		return E_INVALIDARG;

	if (*pElem)
		SysFreeString(*pElem);
	return S_OK;
}

inline HRESULT AtlCleanupValue<VARIANT>(VARIANT *pElem) throw()
{
	VariantClear(pElem);
	return S_OK;
}

inline int AtlArrayMultiDimIndexToIndex(const int *nDims, LPCWSTR wszStr) throw()
{
	if (!nDims || !wszStr)
		return 0;

	if (*wszStr != '[')
		return 0;

	wszStr++;

	int nOffset = 0;
	int nCount = nDims[0];
	int nCurrDim = 1;

	while (*wszStr && nCount>0 && *wszStr != ']')
	{
		LPWSTR wszEnd;
		long nIndex = wcstol(wszStr, &wszEnd, 10);
		
		if (nIndex != 0)
		{
			for (int i=nCurrDim+1; i<=nDims[0]; i++)
				nIndex *= nDims[i];	
			nOffset += nIndex;
		}

		nCurrDim++;
		nCount--;
		wszStr = wszEnd;
		if (*wszStr == ',')
			wszStr++;
	}

	return nOffset;
}

inline HRESULT AtlGetArraySize(IXMLDOMNode *pParam, int *pnSize) throw()
{
	if (!pParam || !pnSize)
	{
		return E_INVALIDARG;
	}

	*pnSize = -1;
	CComPtr<IXMLDOMNamedNodeMap> spAttrs;

	HRESULT hr = pParam->get_attributes(&spAttrs);
	if (hr != S_OK)
	{
		return E_FAIL;
	}
	
	CComPtr<IXMLDOMNode> spAttr;
	hr = spAttrs->nextNode(&spAttr);
	while (hr == S_OK)
	{
		CComBSTR bstrBaseName;
		hr = spAttr->get_baseName(&bstrBaseName);
		if (hr == S_OK && !wcscmp(bstrBaseName, L"arrayType"))
		{
			CComBSTR bstrNamespaceURI;
			hr = spAttr->get_namespaceURI(&bstrNamespaceURI);
			if (hr == S_OK && !wcscmp(bstrNamespaceURI, L"http://schemas.xmlsoap.org/soap/encoding/"))
			{
				break;
			}
		}
		spAttr.Release();
		hr = spAttrs->nextNode(&spAttr);
	}

	if (!spAttr.p)
	{
		return E_FAIL;
	}

	CComVariant varVal;
	hr = spAttr->get_nodeValue(&varVal);
	if (hr != S_OK)
	{
		return hr;
	}
	
	hr = varVal.ChangeType(VT_BSTR);
	if (hr != S_OK)
	{
		return E_FAIL;
	}
	
	WCHAR *wsz = wcschr(varVal.bstrVal, '[');
	if (!wsz)
	{
		return E_FAIL;
	}

	*pnSize = 1;
	while (wsz)
	{
		wsz++;
		*pnSize*= _wtoi(wsz);

		wsz = wcschr(wsz, '[');
	}

	return S_OK;
}

template <typename ElemType>
inline HRESULT AtlGetXMLArrayValue(
	IXMLDOMNode *pParam, 
	ElemType **pArray, 
	const int *nDims, 
	ATL_ID_HASH *pidHash, 
	bool bAlloc=true,
	int *pnSize=NULL) throw()
{
	if (!pArray || !pParam || !nDims || !pidHash)
		return E_INVALIDARG;

	int nOffset = 0;
	if (pnSize)
		*pnSize = 0;
	CComBSTR bstrOffset;

	HRESULT hr = GetAttribute(pParam, L"offset", &bstrOffset);
	if (bstrOffset)
	{
		nOffset = AtlArrayMultiDimIndexToIndex(nDims, bstrOffset);
	}

	int nCount = 1;
	int i;

	if (nDims[0] < 0)
	{
		hr = AtlGetArraySize(pParam, &nCount);
		if (FAILED(hr))
			return hr;
		
		if (nCount <= 0)
			return E_FAIL;
	}
	else
	{
		for (i=0; i<nDims[0]; i++)
			nCount *= nDims[i+1];
	}

	if (bAlloc)
		*pArray = (ElemType *) malloc(nCount*sizeof(ElemType));

	CComPtr<IXMLDOMNode> spElem;
	hr = pParam->get_firstChild(&spElem);

	if (hr == S_OK)
	{
		memset(*pArray, 0x00, nCount*sizeof(ElemType));

		while (spElem)
		{
			CComBSTR bstrPos;
			hr = GetAttribute(spElem, L"position", &bstrPos);
			if (hr == S_OK && bstrPos)
			{
				hr = AtlGetXMLValue(spElem, &((*pArray)[AtlArrayMultiDimIndexToIndex(nDims, bstrPos)]), pidHash);
				if (FAILED(hr))
					break;
			}
			else if (nOffset < nCount)
			{
				// todo: validate that the type of the element
				// is correct
				hr = AtlGetXMLValue(spElem, &((*pArray)[nOffset]), pidHash);
				if (FAILED(hr))
					break;
				nOffset++;
			}
			
			CComPtr<IXMLDOMNode> spNext;
			spElem->get_nextSibling(&spNext);
			if (!spNext)
				break;

			spElem.Release();
			spElem.Attach(spNext.Detach());
		}
	}

	if (hr != S_OK)
	{
		if (bAlloc)
		{
			free(*pArray);
			*pArray = NULL;
		}
		hr = E_FAIL;
	}
	else
	{
		if (pnSize)
			*pnSize = nCount;
	}

	return hr;
}

template <typename ElemType>
inline HRESULT AtlCleanupArray(ElemType *pArray, const int *nDims) throw()
{
	if (!pArray || !nDims)
		return E_INVALIDARG;

	int nCount = 1;
	int i;
	for (i=0; i<nDims[0]; i++)
		nCount *= nDims[i+1];

	for (i = 0; i < nCount; i++)
		AtlCleanupValue(&pArray[i]);	
	
	return S_OK;
}

template <typename ElemType>
inline HRESULT AtlGetSafeArrayXMLValue(IXMLDOMNode *pParam, VARTYPE vt, SAFEARRAY **ppsa, ATL_ID_HASH *pidHash) throw()
{
	if (!pParam || !ppsa || !pidHash)
		return E_INVALIDARG;

	CComBSTR bstrXSDType;
	
	HRESULT hr = GetAttribute(pParam, L"xsd:type", &bstrXSDType);
	if (hr != S_OK)
		return hr;

	LPCWSTR wszCurr = wcschr(bstrXSDType, '[');;
	if (!wszCurr)
		return E_FAIL;
	

	SAFEARRAYBOUND bounds[10];

	int nDimsCount = 0;
	do
	{
		wszCurr++;
		bounds[nDimsCount].lLbound = 0;
		bounds[nDimsCount].cElements = _wtoi(wszCurr);
		nDimsCount++;

		wszCurr += wcscspn(wszCurr, L",]");
		if (!wszCurr)
			return E_FAIL;

	} while (wszCurr && *wszCurr != ']');

	if (nDimsCount < 1)
		return E_FAIL;

    *ppsa = SafeArrayCreate(vt, nDimsCount, bounds);
	if (*ppsa == NULL)
		return E_OUTOFMEMORY;

	CComPtr<IXMLDOMNode> spElem;
	hr = pParam->get_firstChild(&spElem);
	if (FAILED(hr))
		return hr;

	long lIndices[10];
	memset(&lIndices, 0x00, sizeof(lIndices));

	while (spElem)
	{
		ElemType elem;
		hr = AtlGetXMLValue(spElem, &elem, pidHash);
		if (FAILED(hr))
			return hr;
		
		// put in the safe array

//		if (vt == VT_BSTR)
//			SafeArrayPutElement(*ppsa, lIndices, elem);
//		else
		{
			SafeArrayPutElement(*ppsa, lIndices, &elem);
		}

		// increment the index
		bool bDone = true;
		for (int i=0; i<nDimsCount; i++)
		{
			lIndices[i]++;
			if ((DWORD)lIndices[i] < bounds[nDimsCount-i-1].cElements)
			{
				bDone = false;
				break;
			}
			lIndices[i] = 0;
		}

		AtlCleanupValue(&elem);

		if (bDone)
			break;

		CComPtr<IXMLDOMNode> spNext;
		hr = spElem->get_nextSibling(&spNext);
		if (!spNext)
			break;

		spElem.Release();
		spElem.Attach(spNext.Detach());
	}

	return S_OK;	
}


// generic AtlGenXMLValue function
template <typename ParamType>
inline HRESULT AtlGenXMLValue(IWriteStream *pStream, ParamType *pVal) throw()
{
	if (!pStream || !pVal)
		return E_INVALIDARG;

	CWriteStreamHelper s(pStream);
	BOOL bRet = s.Write(*pVal);
	if (!bRet)
		return E_FAIL;
	return S_OK;
}


ATL_NOINLINE inline HRESULT AtlGenXMLValue<BSTR>(IWriteStream *pStream, BSTR *pVal) throw()
{
	if (pVal == NULL)
		return E_INVALIDARG;

	// null string == empty string
	if (*pVal == NULL || **pVal == NULL)
		return S_OK;

	if (!pStream)
		return E_INVALIDARG;

	int nSrcLen = (int) wcslen(*pVal);
	int nLen = AtlUnicodeToUTF8(*pVal, nSrcLen, NULL, 0);
	LPSTR sz = (LPSTR)malloc(nLen+1);
	if (!sz)
		return E_OUTOFMEMORY;

	nLen = AtlUnicodeToUTF8(*pVal, nSrcLen, sz, nLen);
	BOOL bRet = TRUE;
	if (nLen)
	{
		sz[nLen] = '\0';

		// check if we need to escape the XML string
		int nEscLen = EscapeXML(sz, nLen, NULL, 0);
		if (nEscLen)
		{
			if (nEscLen > nLen)
			{
				LPSTR szEsc = (LPSTR) malloc(nEscLen+1);
				if (szEsc)
				{
					nEscLen = EscapeXML(sz, nLen, szEsc, nEscLen);
					if (nEscLen)
					{
						szEsc[nEscLen] = '\0';
						free(sz);
						sz = szEsc;
						nLen = nEscLen;
					}
					else
					{
						free(szEsc);
						bRet = FALSE;
					}
				}
				else
				{
					free(sz);
					return E_OUTOFMEMORY;
				}
			}
		}
		else
		{
			bRet = FALSE;
		}

		if (bRet)
			bRet = SUCCEEDED(pStream->WriteStream(sz, nLen, NULL));
	}
	else
	{
		bRet = FALSE;
	}

	free(sz);
	if (!bRet)
		return E_FAIL;
	return S_OK;
}
    
ATL_NOINLINE inline HRESULT AtlGenXMLValue<VARIANT>(IWriteStream *pStream, VARIANT *pVal) throw()
{
	HRESULT hr = E_FAIL;
	if (pVal->vt & VT_BYREF)
		return E_FAIL;

	switch(pVal->vt)
	{
	case VT_BSTR:
		{
			VARIANT vTemp;
			VariantInit(&vTemp);
			VariantChangeType(&vTemp, pVal, 0, VT_BSTR);
			CWriteStreamHelper s(pStream);
			hr = AtlGenXMLValue(pStream, &(vTemp.bstrVal));
			VariantClear(&vTemp);
		}
		break;
	case VT_UI1:
		hr = AtlGenXMLValue(pStream, &pVal->bVal);
		break;
	case VT_I2:
		hr = AtlGenXMLValue(pStream, &pVal->iVal);
		break;
	case VT_I4:
		hr = AtlGenXMLValue(pStream, &pVal->lVal);
		break;
	case VT_R4:
		hr = AtlGenXMLValue(pStream, &pVal->fltVal);
		break;
	case VT_R8:
		hr = AtlGenXMLValue(pStream, &pVal->dblVal);
		break;
	case VT_BOOL:
		{
			bool bVal = pVal->boolVal ? true : false;
			hr = AtlGenXMLValue(pStream, &bVal);
		}
		break;
	case VT_I1:
		hr = AtlGenXMLValue(pStream, &pVal->cVal);
		break;
	case VT_UI2:
		hr = AtlGenXMLValue(pStream, &pVal->uiVal);
		break;
	case VT_INT:
		hr = AtlGenXMLValue(pStream, &pVal->intVal);
		break;
	case VT_UINT:
		hr = AtlGenXMLValue(pStream, &pVal->uintVal);
		break;
	default: 
		// we don't support anything besides simple types for the variant
		return E_FAIL;
	}
	return hr;
}

inline HRESULT AtlGenXMLValue<double>(IWriteStream *pStream, double *pVal) throw()
{
	if (!pStream || !pVal)
		return E_INVALIDARG;

	CWriteStreamHelper s(pStream);
	switch (_fpclass(*pVal))
	{
	case _FPCLASS_SNAN:	// fallthrough
	case _FPCLASS_QNAN:
		s << "NAN";
		break;

	case _FPCLASS_NINF:
		s << "-INF";
		break;

	case _FPCLASS_PINF:
		s << "INF";
		break;

	case _FPCLASS_NZ:
		s << "-0";
		break;

	default:
		s << *pVal;
		break;
	}
	return S_OK;
}

inline HRESULT AtlGenXMLValue<bool>(IWriteStream *pStream, bool *pVal) throw()
{
	if (!pStream || !pVal)
		return E_INVALIDARG;

	CWriteStreamHelper s(pStream);
	if (*pVal)
		s << "true";
	else
		s << "false";

	return S_OK;
}

template <typename ElemType>
inline HRESULT AtlGenXMLArrayValue(IWriteStream *pStream, ElemType *pArray, const int *nDims, const char *szType) throw()
{
	// empty arrays are okay
	if (!pArray || !nDims)
		return S_OK;

	if (!pStream || !szType)
		return E_INVALIDARG;

	int nCount = 1;
	int i;
	for (i=0; i<nDims[0]; i++)
		nCount *= nDims[i+1];
		
	for (i=0; i<nCount; i++)
	{
		pStream->WriteStream("\r\n<", -1, NULL);
		pStream->WriteStream(szType, -1, NULL);
		pStream->WriteStream(">", -1, NULL);
		AtlGenXMLValue(pStream, &pArray[i]);
		pStream->WriteStream("</", -1, NULL);
		pStream->WriteStream(szType, -1, NULL);
		pStream->WriteStream(">", -1, NULL);
	}
	return S_OK;
}

struct AtlEnumEntry
{
	LPCWSTR wszName;
	LPSTR szName;
	int nLen;
	int nVal;
};

// BLOB data type
// use this struct when you want to send BLOB data
// the provider and proxy generator will only 
// properly special case BLOB data when using this
// struct.
[ export ]
typedef struct _tagATLSOAP_BLOB
{
	unsigned long size;
	unsigned char *data;
} ATLSOAP_BLOB;

ATL_NOINLINE inline HRESULT AtlGetXMLValue<ATLSOAP_BLOB>(IXMLDOMNode * pNode, ATLSOAP_BLOB * pVal, ATL_ID_HASH * pidHash) throw()
{
	if (!pNode || !pVal || !pidHash)
		return E_INVALIDARG;

	memset(pVal, 0x00, sizeof(*pVal));

	CComPtr<IXMLDOMNode> spParam;
	HRESULT hr = ATL::GetHref(pNode, &spParam, pidHash);
	if (FAILED(hr))
		return hr;

	CComBSTR bstrName;

	CComPtr<IXMLDOMNode> spChild;
	CComPtr<IXMLDOMNode> spSizeField;
	CComPtr<IXMLDOMNode> spDataField;

	hr = spParam->get_firstChild(&spChild);

	while (hr == S_OK)
	{
		CComBSTR bstrBaseName;
		hr = spChild->get_baseName(&bstrBaseName);
		if (hr != S_OK)
			break;

		if (!wcscmp(bstrBaseName, L"size"))
			spSizeField = spChild;
		else if (!wcscmp(bstrBaseName, L"data"))
			spDataField = spChild;

		CComPtr<IXMLDOMNode> spNext;
		hr = spChild->get_nextSibling(&spNext);
		spChild.Release();
		spChild.Attach(spNext.Detach());
	}

	if (!spSizeField || !spDataField)
		return E_FAIL;

	hr = AtlGetXMLValue(spSizeField, &pVal->size, pidHash);
	if (hr != S_OK || pVal->size <= 0)
		return hr;

	// assume the next bit is base64 encoded
	CComBSTR bstrData;
	hr = AtlGetXMLValue(spDataField, &bstrData, pidHash);
	if (hr != S_OK)
		return hr;

	int nLength = WideCharToMultiByte(CP_ACP, 0, bstrData, bstrData.Length(), NULL, 0, NULL, NULL);
	if (!nLength)
		return E_FAIL;
	
	char *pSrc = (char *)malloc(nLength*sizeof(char));
	if (!pSrc)
		return E_FAIL;

	WideCharToMultiByte(CP_ACP, 0, bstrData, bstrData.Length(), pSrc, nLength, NULL, NULL);

	pVal->data = (unsigned char *)malloc(pVal->size*sizeof(unsigned char));
	if (!pVal->data)
	{
		free(pSrc);
		return E_OUTOFMEMORY;
	}

	int nDataLength = pVal->size;
	if (!Base64Decode(pSrc, nLength, pVal->data, &nDataLength))
		hr = E_FAIL;

	free(pSrc);
	if (hr != S_OK)
	{
		free(pVal->data);
		memset(pVal, 0x00, sizeof(*pVal));
	}

	return hr;
}

ATL_NOINLINE inline HRESULT AtlGenXMLValue<ATLSOAP_BLOB>(IWriteStream *pStream, ATLSOAP_BLOB *pVal) throw()
{
	if (!pStream || !pVal)
		return E_INVALIDARG;

	CWriteStreamHelper s(pStream);
	s << "<size>\r\n";
	HRESULT hr = AtlGenXMLValue(pStream, &pVal->size);
	if (hr != S_OK)
		return hr;
	s << "</size>\r\n";
	s << "<data>\r\n";
	int nLength = Base64EncodeGetRequiredLength(pVal->size, ATL_BASE64_FLAG_NOCRLF);
	char *pEnc = (char *)malloc(nLength*sizeof(char));
	if (!pEnc)
		return E_OUTOFMEMORY;

	if (!Base64Encode(pVal->data, pVal->size, pEnc, &nLength, ATL_BASE64_FLAG_NOCRLF))
		hr = E_FAIL;

	if (hr == S_OK)
	{
		pStream->WriteStream(pEnc, nLength, NULL);
		s << "</data>";
	}

	free(pEnc);
	return hr;
}

inline HRESULT AtlCleanupValue<ATLSOAP_BLOB>(ATLSOAP_BLOB * pval) throw()
{
	if (pval && pval->data)
		free(pval->data);
	return S_OK;
}

namespace ATL
{
enum SOAP_ERROR_CODE { SOAP_E_VERSION_MISMATCH=100, SOAP_E_MUST_UNDERSTAND=200,
	SOAP_E_INVALID_REQUEST=300, SOAP_E_APPLICATION_FAULTED=400 };

enum SOAP_RUN_CODE { SOAP_RUN_YES, SOAP_RUN_NO, SOAP_RUN_MAYBE };

inline HRESULT GenerateSoapError(IWriteStream *pStream, SOAP_ERROR_CODE nErrorCode, SOAP_RUN_CODE nRunCode, LPCWSTR wszMessage=NULL,
						  LPCWSTR wszDetail=NULL) throw()
{
	if (!pStream)
		return E_INVALIDARG;

	ATLASSERT(nErrorCode==SOAP_E_VERSION_MISMATCH ||
		nErrorCode==SOAP_E_MUST_UNDERSTAND ||
		nErrorCode==SOAP_E_INVALID_REQUEST ||
		nErrorCode==SOAP_E_APPLICATION_FAULTED);

	ATLASSERT(nRunCode >= SOAP_RUN_YES && nRunCode <= SOAP_RUN_MAYBE);

	static const LPCWSTR s_wszStockErrors[] = {
		L"Version Mismatch", L"Must understand", L"Invalid Request",
			L"Application Faulted" };

	static const LPCSTR s_szRunCodes[] = { "Yes", "No", "Maybe" };

	if (!wszMessage)
		wszMessage = s_wszStockErrors[nErrorCode/100-1];


	if (!wszDetail)
		wszDetail = L"";

	static const LPCSTR s_szErrorFormat = 
		"<SOAP:Envelope xmlns:SOAP=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
		"	<SOAP:Body>\n"
        "		<SOAP:Fault>\n"
        "			<faultcode>%d</faultcode>\n"
        "			<faultstring>%s</faultstring>\n"
        "			<runcode>%s</runcode>\n"
		"%s\n"
        "		</SOAP:Fault>\n"
		"	</SOAP:Body>\n"
		"</SOAP:Envelope>\n";

	LPCSTR szRunCode = s_szRunCodes[nRunCode];

	int nCount = WideCharToMultiByte(CP_UTF8, 0, wszMessage, -1, NULL, 0, NULL, NULL);
	if (nCount == 0)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	char *szMessage = new char[nCount];
	if (!szMessage)
		return E_OUTOFMEMORY;

	WideCharToMultiByte(CP_UTF8, 0, wszMessage, -1, szMessage, nCount, NULL, NULL);


	nCount = WideCharToMultiByte(CP_UTF8, 0, wszDetail, -1, NULL, 0, NULL, NULL);
	if (nCount == 0)
	{
		delete [] szMessage;
		return HRESULT_FROM_WIN32(GetLastError());
	}

	char *szDetail = new char[nCount];
	if (!szDetail)
	{
		delete [] szMessage;
		return E_OUTOFMEMORY;
	}

	nCount = WideCharToMultiByte(CP_UTF8, 0, wszDetail, -1, szDetail, nCount, NULL, NULL);
	if (nCount == 0)
	{
		delete [] szMessage;
		return HRESULT_FROM_WIN32(GetLastError());
	}

	CStringA str;
	str.Format(s_szErrorFormat, nErrorCode, szMessage, szRunCode, szDetail);

	delete [] szMessage;
	delete [] szDetail;

	pStream->WriteStream(str, -1, NULL);

	return S_OK;
}

inline HRESULT GenerateDefaultAppError(IWriteStream *pStream, HRESULT hr) throw()
{
	if (!pStream)
		return E_INVALIDARG;

	WCHAR tmp[400];
	swprintf(tmp, L"<detail xmlns:e=\"http://tempuri.org\"\n"
                    L"xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\"\n"
					L"xsd:type=\"e:ApplicationFault\">"
					L"<message>COM Error</message>\n"
					L"<errorcode>0x%08X</errorcode>\n"
					L"</detail>\n", hr);

	return GenerateSoapError(pStream, SOAP_E_APPLICATION_FAULTED, SOAP_RUN_YES, NULL,
		tmp);
}

template <class THandler>
class CSoapHandler : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public IRequestHandlerImpl<THandler>
{
public:

	BEGIN_COM_MAP(CSoapHandler<THandler>)
		COM_INTERFACE_ENTRY(IRequestHandler)
	END_COM_MAP()

	HTTP_CODE HandleRequest(AtlServerRequest *pRequestInfo, IServiceProvider *pProvider) throw()
	{
		THandler *pHandler = static_cast<THandler*>(this);

		m_hInstHandler = pRequestInfo->hInstDll;
		m_spExtension = pRequestInfo->pExtension;
		m_spServiceProvider = pProvider;
		m_spServerContext = pRequestInfo->pServerContext;

		CStreamOnServerContext s(pRequestInfo->pServerContext);
		CHttpResponse Response;
		Response.Initialize(pRequestInfo->pServerContext);
		Response.SetContentType("text/xml");

		if (FAILED(pHandler->DispatchSoapCall(&s, &Response)))
		{
			return HTTP_FAIL;
		}

		return HTTP_SUCCESS;
	}

	HRESULT GenerateAppError(IWriteStream *pStream, HRESULT hr) throw()
	{
		return GenerateDefaultAppError(pStream, hr);
	}
};

template <class THandler, const char *szHandlerName>
class CSDLGenerator : 
	public IRequestHandlerImpl<CSDLGenerator>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public ITagReplacerImpl<CSDLGenerator>
{
public:
	typedef CSDLGenerator<THandler, szHandlerName> _sdlGenerator;

	BEGIN_COM_MAP(_sdlGenerator)
		COM_INTERFACE_ENTRY(IRequestHandler)
		COM_INTERFACE_ENTRY(ITagReplacer)
	END_COM_MAP()

	HTTP_CODE InitializeHandler(AtlServerRequest *pRequestInfo, IServiceProvider *pServiceProvider) throw()
	{
		IRequestHandlerImpl<CSDLGenerator>::InitializeHandler(pRequestInfo, pServiceProvider);

		CHtmlStencil s;
		LPCSTR szSDL;
		THandler::__AtlGetSDL(&szSDL);
		s.LoadFromString(szSDL, (DWORD) strlen(szSDL));

		CHttpResponse response(pRequestInfo->pServerContext);

		response.SetContentType("text/xml");

		s.ParseReplacements(this);
		s.FinishParseReplacements();
		SetStream(&response);

		s.Render(this, &response);
		return HTTP_SUCCESS;
	}

	HTTP_CODE GetUrl() throw()
	{
		char szUrl[ATL_URL_MAX_URL_LENGTH];
		DWORD dwSize = sizeof(szUrl);
		char szServer[ATL_URL_MAX_HOST_NAME_LENGTH];
		dwSize = sizeof(szServer);

		if (m_spServerContext->GetServerVariable("URL", szUrl, &dwSize))
		{
			if (m_spServerContext->GetServerVariable("SERVER_NAME", szServer, &dwSize))
			{
				CStringA strUrl;
				strUrl.Format("http://%s%s?Handler=%s", szServer, szUrl, szHandlerName);

				m_pStream->WriteStream(strUrl, -1, NULL);
				return HTTP_SUCCESS;
			}
		}
		return HTTP_FAIL;
	}

	BEGIN_REPLACEMENT_METHOD_MAP(CSDLGenerator)
		REPLACEMENT_METHOD_ENTRY("GetUrl", GetUrl)
	END_REPLACEMENT_METHOD_MAP()
};

class CWriteStreamOnCString : public IWriteStream
{
public:
	CStringA m_str;
	
	HRESULT WriteStream(LPCSTR szOut, int nLen, DWORD *pdwWritten) throw()
	{
		if (nLen < 0)
			nLen = (int) strlen(szOut);
	
		m_str.Append(szOut, nLen);
		if (pdwWritten)
			*pdwWritten = nLen;
		return S_OK;
	}

	HRESULT FlushStream() throw()
	{
		return S_OK;
	}
	
	void Cleanup() throw()
	{
		m_str.Empty();
	}
	
};

class CReadStreamOnSocket : public IStream
{
public:
	CReadStreamOnSocket() :
	  m_szBuffer(NULL),
	  m_szCurr(NULL),
	  m_pSocket(NULL),
	  m_nBodyLen(0)
	 {

	 }
	CAtlHttpClient *m_pSocket;
	LPCSTR m_szBuffer;
	LPCSTR m_szCurr;
	long m_nBodyLen;

	bool Init(CAtlHttpClient* psocket) throw()
	{
		ATLASSERT(psocket != NULL);

		bool bRet = false;
		m_pSocket = psocket;
		m_szBuffer = (LPCSTR)psocket->GetBody();
		if (m_szBuffer)
		{
			m_szCurr = m_szBuffer;
			m_nBodyLen = psocket->GetBodyLength();
			if (m_nBodyLen)
				bRet = true;
		}
		return bRet;
	}

	STDMETHOD(QueryInterface)(REFIID riid, void **ppv) throw()
	{
		if (!ppv)
			return E_POINTER;

		*ppv = NULL;

		if (InlineIsEqualGUID(riid, IID_IUnknown) ||
			InlineIsEqualGUID(riid, IID_IStream) ||
			InlineIsEqualGUID(riid, IID_ISequentialStream))
		{
			*ppv = (IStream *) this;
		}
		if (!*ppv)
			return E_NOINTERFACE;
		return S_OK;
	}

	ULONG __stdcall AddRef() throw()
	{
		return 1;
	}

	ULONG __stdcall Release() throw()
	{
		return 1;
	}

	// ISequentialStream methods
    HRESULT STDMETHODCALLTYPE Read(void *pDest, ULONG dwMaxLen, ULONG *pdwRead) throw()
	{
		ATLASSERT(pDest);
		ATLASSERT(m_pSocket);
		ATLASSERT(m_szBuffer);
		ATLASSERT(pdwRead);

		*pdwRead = 0;
		long nReadSoFar = (int)(m_szCurr-m_szBuffer);
		if (nReadSoFar >= m_nBodyLen)
		{
			return S_OK; // no more to read;
		}

		long nLengthToRead = min((int)(m_nBodyLen-nReadSoFar), (LONG)dwMaxLen);
		memcpy(pDest, m_szCurr, nLengthToRead);
		m_szCurr += nLengthToRead;
		*pdwRead = (ULONG)nLengthToRead;
		return S_OK ;
	}

    HRESULT STDMETHODCALLTYPE Write(const void * /*pv*/, ULONG /*cb*/, ULONG * /*pcbWritten*/) throw()
	{
		return E_NOTIMPL;
	}
	
	// IStream methods
    HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER /*dlibMove*/, DWORD /*dwOrigin*/, ULARGE_INTEGER * /*plibNewPosition*/) throw()
	{
		return E_NOTIMPL;
	}

    HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER /*libNewSize*/) throw()
	{
		return E_NOTIMPL;
	}
    
    HRESULT STDMETHODCALLTYPE CopyTo(IStream * /*pstm*/, ULARGE_INTEGER /*cb*/, ULARGE_INTEGER * /*pcbRead*/, ULARGE_INTEGER * /*pcbWritten*/) throw()
	{
		return E_NOTIMPL;
	}

    HRESULT STDMETHODCALLTYPE Commit(DWORD /*grfCommitFlags*/) throw()
	{
		return E_NOTIMPL;
	}
    
    HRESULT STDMETHODCALLTYPE Revert(void) throw()
	{
		return E_NOTIMPL;
	}
    
    HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER /*libOffset*/, ULARGE_INTEGER /*cb*/, DWORD /*dwLockType*/) throw()
	{
		return E_NOTIMPL;
	}
		
    HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER /*libOffset*/, ULARGE_INTEGER /*cb*/, DWORD /*dwLockType*/) throw()
	{
		return E_NOTIMPL;
	}
    
    HRESULT STDMETHODCALLTYPE Stat(STATSTG * /*pstatstg*/, DWORD /*grfStatFlag*/) throw()
	{
		return E_NOTIMPL;
	}

    
    HRESULT STDMETHODCALLTYPE Clone(IStream ** /*ppstm*/) throw()
	{
		return E_NOTIMPL;
	}
};

class CSoapSocketClient
{
public:
	ATL_URL_PORT m_nPort;
	CAtlHttpClient m_socket;

	CString m_strUrl;
	CWriteStreamOnCString m_Stream;
	CReadStreamOnSocket	m_ReadStream;

	// constructor
	CSoapSocketClient(LPCTSTR szUrl) throw()
	{
		m_strUrl = szUrl;
	}

	CSoapSocketClient(LPCTSTR szServer, LPCTSTR szUri, ATL_URL_PORT nPort=80) throw()
	{
		ATLASSERT(szServer != NULL);
		ATLASSERT(szUri != NULL);

		CUrl url;
		url.SetUrlPath(szUri);
		url.SetHostName(szServer);
		DWORD dwLen = 0;
		if (url.CreateUrl(m_strUrl.GetBuffer(ATL_URL_MAX_URL_LENGTH), &dwLen))
			m_strUrl.ReleaseBuffer();
		else
			m_strUrl.ReleaseBuffer(0);
		m_nPort = nPort;
	}

	~CSoapSocketClient() throw()
	{
		Cleanup();
	}
	
	void Cleanup() throw()
	{
		m_Stream.Cleanup();
	}

	HRESULT SendRequest(LPCTSTR szAction) throw()
	{
		bool bRet = true;
		if ((SOCKET)m_socket == INVALID_SOCKET)
			bRet = m_socket.Create();


		if (!bRet)
			return E_FAIL;
		
		bRet = false;
		
		// create extra headers to send with request
		CString strExtraHeaders(szAction);
		strExtraHeaders += _T("Accept: text/xml\r\n");

		if (m_socket.Navigate(m_strUrl, ATL_HTTP_FLAG_AUTO_REDIRECT,
			strExtraHeaders, ATL_HTTP_METHOD_POST, m_nPort, (BYTE*)(LPCSTR)m_Stream.m_str,
			m_Stream.m_str.GetLength(),  _T("text/xml")))
		{
		
			bRet = m_ReadStream.Init(&m_socket);
		}

		return bRet ? S_OK : E_FAIL;
	}

	LPCTSTR GetUrl() throw()
	{
		return (LPCTSTR)m_strUrl;
	}

	void SetUrl(LPCTSTR szUrl) throw()
	{
		m_strUrl = szUrl;
	}
};


class CWriteStreamOnFP : public IWriteStream
{
public:
	FILE *m_fp;

	CWriteStreamOnFP() throw()
	{
	}

	void Init(FILE *fp) throw()
	{
		_setmode(_fileno(fp),_O_BINARY);
		m_fp = fp;
	}
	
	HRESULT WriteStream(LPCSTR szOut, int nLen, DWORD *pdwWritten) throw()
	{
		DWORD dwWritten;
		if (nLen < 0)
			nLen = (int) strlen(szOut);
		dwWritten = (DWORD) fwrite(szOut, 1, nLen, m_fp);
		if (pdwWritten)
			*pdwWritten = dwWritten;
		return (dwWritten==(DWORD) nLen) ? S_OK : STG_E_WRITEFAULT;
	}

	HRESULT FlushStream() throw()
	{
		fflush(m_fp);
		return S_OK;
	}

};

class CWriteStreamOnFileW : public IWriteStream
{
public:
	HANDLE m_hFile;

	CWriteStreamOnFileW() throw()
		:m_hFile(INVALID_HANDLE_VALUE)
	{
	}

	HRESULT Init(LPCWSTR wszFile) throw()
	{
		if (!wszFile)
			return E_INVALIDARG;

		HRESULT hr = S_OK;
		m_hFile = CreateFileW(wszFile, GENERIC_WRITE, FILE_SHARE_READ, 
			NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (m_hFile == INVALID_HANDLE_VALUE)
			hr = AtlHresultFromLastError();
		return hr;
	}

	HRESULT WriteStream(LPCSTR szOut, int nLen, LPDWORD pdwWritten) throw()
	{
		ATLASSERT(szOut != NULL);
		ATLASSERT(m_hFile != INVALID_HANDLE_VALUE);
		
		HRESULT hr = S_OK;

		if (nLen < 0)
			nLen = (int) strlen(szOut);
		DWORD dwWritten=0;
		BOOL bRet = WriteFile(m_hFile, szOut, nLen, &dwWritten, NULL);
		if (pdwWritten)
			*pdwWritten = dwWritten;
		if (!bRet)
			hr = AtlHresultFromLastError();
		return hr;
	}

	HRESULT FlushStream() throw()
	{
		ATLASSERT(m_hFile != INVALID_HANDLE_VALUE);

		HRESULT hr = S_OK;

		BOOL bRet = FlushFileBuffers(m_hFile);
		if (!bRet)
			hr = AtlHresultFromLastError();
		return hr;
	}

	~CWriteStreamOnFileW() throw()
	{
		if (m_hFile != INVALID_HANDLE_VALUE)
			CloseHandle(m_hFile);
	}
};

// override this definition if you are using arrays of
// more than 5 dimensions
#ifndef ATLSOAP_MAX_DIMS
	#define ATLSOAP_MAX_DIMS 5
#endif

struct AtlSoapFieldTypeInfo
{
	// name of field/parameter (wide)
	LPCWSTR wszName;

	// name of field/parameter (ansi)
	LPCSTR szName;

	// offset into struct
	int nOffset;

	// unique (per compilation unit) index/identifier of type
	int nTypeIndex;

	// array dimensions
	int Dims[ATLSOAP_MAX_DIMS];
};

} // namespace ATL
