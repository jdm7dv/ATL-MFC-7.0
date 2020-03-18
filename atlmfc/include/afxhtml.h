// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-2000 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __AFXHTML_H__
#define __AFXHTML_H__

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __AFXDISP_H__
	#include <afxdisp.h>
#endif

#ifndef __exdisp_h__
	#include <exdisp.h>
#endif

#ifndef __mshtmhst_h__
#include <mshtmhst.h>
#endif

#ifndef __mshtml_h__
#include <mshtml.h>
#endif

#ifndef __dhtmled_h__
#include <dhtmled.h>
#endif

#ifndef __TRIEDCID_H__
#include <triedcid.h>
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif
#ifndef _AFX_FULLTYPEINFO
#pragma component(mintypeinfo, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

/////////////////////////////////////////////////////////////////////////////
// AFXHTML - MFC Visual HTML classes

// Classes declared in this file

//CObject
	//CCmdTarget;
		//CWnd
			class CHtmlEditCtrl;
			//CView
				//CFormView
					class CHtmlView;
					class CHtmlEditView;

//CObject
	//CCmdTarget;
		//CDocument
			class CHtmlEditDoc;


#undef AFX_DATA
#define AFX_DATA AFX_OLE_DATA

/////////////////////////////////////////////////////////////////////////////
// CHtmlView

class CHtmlView : public CFormView
{
protected: // create from serialization only
	CHtmlView();
	DECLARE_DYNCREATE(CHtmlView)
	DECLARE_EVENTSINK_MAP()

// Attributes
public:
	CString GetType() const;
	long GetLeft() const;
	void SetLeft(long nNewValue);
	long GetTop() const;
	void SetTop(long nNewValue);
	long GetHeight() const;
	void SetHeight(long nNewValue);
	long GetWidth() const;
	void SetWidth(long nNewValue);
	void SetVisible(BOOL bNewValue);
	BOOL GetVisible() const;
	CString GetLocationName() const;
	READYSTATE GetReadyState() const;
	BOOL GetOffline() const;
	void SetOffline(BOOL bNewValue);
	BOOL GetSilent() const;
	void SetSilent(BOOL bNewValue);
	BOOL GetTopLevelContainer() const;
	CString GetLocationURL() const;
	BOOL GetBusy() const;
	LPDISPATCH GetApplication() const;
	LPDISPATCH GetParentBrowser() const;
	LPDISPATCH GetContainer() const;
	LPDISPATCH GetHtmlDocument() const;
	CString GetFullName() const;
	int GetToolBar() const;
	void SetToolBar(int nNewValue);
	BOOL GetMenuBar() const;
	void SetMenuBar(BOOL bNewValue);
	BOOL GetFullScreen() const;
	void SetFullScreen(BOOL bNewValue);
	OLECMDF QueryStatusWB(OLECMDID cmdID) const;
	BOOL GetRegisterAsBrowser() const;
	void SetRegisterAsBrowser(BOOL bNewValue);
	BOOL GetRegisterAsDropTarget() const;
	void SetRegisterAsDropTarget(BOOL bNewValue);
	BOOL GetTheaterMode() const;
	void SetTheaterMode(BOOL bNewValue);
	BOOL GetAddressBar() const;
	void SetAddressBar(BOOL bNewValue);
	BOOL GetStatusBar() const;
	void SetStatusBar(BOOL bNewValue);

// Operations
public:
	void GoBack();
	void GoForward();
	void GoHome();
	void GoSearch();
	void Navigate(LPCTSTR URL, DWORD dwFlags = 0,
		LPCTSTR lpszTargetFrameName = NULL,
		LPCTSTR lpszHeaders = NULL, LPVOID lpvPostData = NULL,
		DWORD dwPostDataLen = 0);
	void Navigate2(LPITEMIDLIST pIDL, DWORD dwFlags = 0,
		LPCTSTR lpszTargetFrameName = NULL);
	void Navigate2(LPCTSTR lpszURL, DWORD dwFlags = 0,
		LPCTSTR lpszTargetFrameName = NULL,	LPCTSTR lpszHeaders = NULL,
		LPVOID lpvPostData = NULL, DWORD dwPostDataLen = 0);
	void Navigate2(LPCTSTR lpszURL, DWORD dwFlags,
		CByteArray& baPostedData,
		LPCTSTR lpszTargetFrameName = NULL, LPCTSTR lpszHeader = NULL);
	void Refresh();
	void Refresh2(int nLevel);
	void Stop();
	void PutProperty(LPCTSTR lpszProperty, const VARIANT& vtValue);
	void PutProperty(LPCTSTR lpszPropertyName, double dValue);
	void PutProperty(LPCTSTR lpszPropertyName, LPCTSTR lpszValue);
	void PutProperty(LPCTSTR lpszPropertyName, long lValue);
	void PutProperty(LPCTSTR lpszPropertyName, short nValue);
	BOOL GetProperty(LPCTSTR lpszProperty, CString& strValue);
	COleVariant GetProperty(LPCTSTR lpszProperty);
	void ExecWB(OLECMDID cmdID, OLECMDEXECOPT cmdexecopt, VARIANT* pvaIn,
		VARIANT* pvaOut);
	BOOL GetSource(CString& strRef);
	BOOL LoadFromResource(LPCTSTR lpszResource);
	BOOL LoadFromResource(UINT nRes);

	HRESULT QueryFormsCommand(DWORD dwCommandID, BOOL* pbSupported,
								BOOL* pbEnabled, BOOL* pbChecked);
	HRESULT ExecFormsCommand(DWORD dwCommandID, VARIANT* pVarIn,
								VARIANT* pVarOut);

// Overrides
public:
	virtual BOOL CreateControlSite(COleControlContainer* pContainer, 
	   COleControlSite** ppSite, UINT nID, REFCLSID clsid);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
		DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID,
		CCreateContext* pContext = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	//{{AFX_MSG(CHtmlView)
	afx_msg void OnFilePrint();
	//}}AFX_MSG

	// DocHostUIHandler overrideables
	virtual HRESULT OnShowContextMenu(DWORD dwID, LPPOINT ppt,
		LPUNKNOWN pcmdtReserved, LPDISPATCH pdispReserved);
	virtual HRESULT OnGetExternal(LPDISPATCH *lppDispatch);
	virtual HRESULT OnGetHostInfo(DOCHOSTUIINFO *pInfo);
	virtual HRESULT OnShowUI(DWORD dwID,
		LPOLEINPLACEACTIVEOBJECT pActiveObject,
		LPOLECOMMANDTARGET pCommandTarget, LPOLEINPLACEFRAME pFrame,
		LPOLEINPLACEUIWINDOW pDoc);
	virtual HRESULT OnHideUI();
	virtual HRESULT OnUpdateUI();
	virtual HRESULT OnEnableModeless(BOOL fEnable);
	virtual HRESULT OnDocWindowActivate(BOOL fActivate);
	virtual HRESULT OnFrameWindowActivate(BOOL fActivate);
	virtual HRESULT OnResizeBorder(LPCRECT prcBorder,
		LPOLEINPLACEUIWINDOW pUIWindow, BOOL fFrameWindow);
	virtual HRESULT OnTranslateAccelerator(LPMSG lpMsg,
		const GUID* pguidCmdGroup, DWORD nCmdID);
	virtual HRESULT OnGetOptionKeyPath(LPOLESTR* pchKey, DWORD dwReserved);
	virtual HRESULT OnFilterDataObject(LPDATAOBJECT pDataObject,
		LPDATAOBJECT* ppDataObject);
	virtual HRESULT OnTranslateUrl(DWORD dwTranslate,
		OLECHAR* pchURLIn, OLECHAR** ppchURLOut);
	virtual HRESULT OnGetDropTarget(LPDROPTARGET pDropTarget,
		LPDROPTARGET* ppDropTarget);

	// Events
	virtual void OnNavigateComplete2(LPCTSTR strURL);
	virtual void OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags,
		LPCTSTR lpszTargetFrameName, CByteArray& baPostedData,
		LPCTSTR lpszHeaders, BOOL* pbCancel);
	virtual void OnStatusTextChange(LPCTSTR lpszText);
	virtual void OnProgressChange(long nProgress, long nProgressMax);
	virtual void OnCommandStateChange(long nCommand, BOOL bEnable);
	virtual void OnDownloadBegin();
	virtual void OnDownloadComplete();
	virtual void OnTitleChange(LPCTSTR lpszText);
	virtual void OnPropertyChange(LPCTSTR lpszProperty);
	virtual void OnNewWindow2(LPDISPATCH* ppDisp, BOOL* Cancel);
	virtual void OnDocumentComplete(LPCTSTR lpszURL);
	virtual void OnQuit();
	virtual void OnVisible(BOOL bVisible);
	virtual void OnToolBar(BOOL bToolBar);
	virtual void OnMenuBar(BOOL bMenuBar);
	virtual void OnStatusBar(BOOL bStatusBar);
	virtual void OnFullScreen(BOOL bFullScreen);
	virtual void OnTheaterMode(BOOL bTheaterMode);

// Implementation
public:
	virtual ~CHtmlView();
	virtual void OnDraw(CDC* pDC);
	CWnd m_wndBrowser;
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	IWebBrowser2* m_pBrowserApp;

// Event reflectors (not normally overridden)
protected:
	virtual void NavigateComplete2(LPDISPATCH pDisp, VARIANT* URL);
	virtual void BeforeNavigate2(LPDISPATCH pDisp, VARIANT* URL,
		VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData,
		VARIANT* Headers, VARIANT_BOOL* Cancel);
	virtual void DocumentComplete(LPDISPATCH pDisp, VARIANT* URL);

// Generated message map functions
protected:
	//{{AFX_MSG(CHtmlView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CHtmlEditCtrlBase
template <class T>
class CHtmlEditCtrlBase 
{
public:
//Methods
	BOOL ExecCommand(long cmdID, long cmdExecOpt, VARIANT* pInVar,VARIANT* pOutVar) const
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();

		HRESULT hr;
		ATLASSERT(spDoc != NULL);
 
		CComVariant vaIn;
		CComVariant vaOut;

		if (pInVar)
			vaIn.Copy(pInVar);
		if (pOutVar)
			vaOut.Copy(pOutVar);

		hr = spDoc->ExecCommand((DHTMLEDITCMDID)cmdID,(OLECMDEXECOPT)cmdExecOpt,&vaIn,&vaOut);
		if (FAILED(hr) && pOutVar != NULL)	
		{
			pOutVar->vt = VT_ERROR;
			pOutVar->scode = hr;
			return FALSE;
		} 
		else
		{
			if(pInVar)
				::VariantCopy(pInVar,&vaIn);
			if(pOutVar)
				::VariantCopy(pOutVar,&vaOut);
			return TRUE;
		}
	}

	long QueryStatus(long cmdID) const
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();

		ATLASSERT(spDoc != NULL);
		DHTMLEDITCMDF cmdF;
		spDoc->QueryStatus((DHTMLEDITCMDID)cmdID, &cmdF);
		return (long)cmdF;
	}

	BOOL SetContextMenu(LPTSTR *ppMenuStrings, OLE_TRISTATE *pState, DWORD cItems)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		if(ppMenuStrings==NULL || pState==NULL)
			return FALSE;
		
		USES_CONVERSION;
		HRESULT hr = E_FAIL;
		SAFEARRAY *psaStr = NULL, *psaTS = NULL;
		long index = 0;
		BSTR szItem = NULL;
		VARIANT vStrings,vState;
		vStrings.vt = VT_ARRAY|VT_BSTR;
		vState.vt = VT_ARRAY|VT_I4;
	
		//create arrays for the strings and tristates
		SAFEARRAYBOUND saBound = {cItems,0};
		psaStr = ::SafeArrayCreate(VT_BSTR,1,&saBound);
		psaTS = ::SafeArrayCreate(VT_I4, 1, &saBound);
		if (psaStr && !psaTS)
		{
			::SafeArrayDestroy(psaStr);
			return FALSE;
		}
		if (!psaStr && psaTS)
		{
			::SafeArrayDestroy(psaTS);
			return FALSE;
		}
	
		for(index = 0; index < (long)cItems; index++)
		{
			szItem = ::SysAllocString( T2OLE(ppMenuStrings[index]) );
			if (szItem)
			{
				hr = ::SafeArrayPutElement(psaStr, &index, szItem);
				if(FAILED(hr))
					break; //couldn't add to the safearray
				hr = ::SafeArrayPutElement(psaTS, &index, &pState[index]);
				if(FAILED(hr))
					break;
			}
			else
			{ 
				//couldn't allocate the BSTR
				hr = E_OUTOFMEMORY;
				break;
			}
		}
	
		if (FAILED(hr))
		{
			//failed to create all elements of the arrays
			//better free any BSTRs we allocated
			for(long n = 0; n < index; n++)
				if(SUCCEEDED(::SafeArrayGetElement(psaStr, &n, reinterpret_cast<void*>(&szItem))))
					::SysFreeString(szItem);
	
			::SafeArrayDestroy(psaStr);
			::SafeArrayDestroy(psaTS);
			return FALSE;
		}
	
		vStrings.parray = psaStr;
		vState.parray = psaTS;
	
		//arrays are created, call the method
		return SUCCEEDED(spDoc->SetContextMenu(&vStrings, &vState));
	}
	
	BOOL NewDocument()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return SUCCEEDED(spDoc->NewDocument());
	}

	BOOL LoadURL(LPCTSTR szURL)
	{
		USES_CONVERSION;
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();

		ATLASSERT(spDoc != NULL);
		CComBSTR var(szURL);
		return SUCCEEDED(spDoc->LoadURL(var));
	}

	BOOL FilterSourceCode(LPCTSTR szSourceCodeIn, BSTR *pszSourceOut)
	{
		USES_CONVERSION;
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();

		ATLASSERT(spDoc != NULL);
		CComBSTR var(szSourceCodeIn);
		return SUCCEEDED(spDoc->FilterSourceCode(var, pszSourceOut));
	}

	BOOL Refresh()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();

		ATLASSERT(spDoc != NULL);
		return SUCCEEDED(spDoc->Refresh());
	}

	BOOL LoadDocument(LPCTSTR szName)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		ATLASSERT(spDoc != NULL);
		
		CComVariant vtName(szName);
		CComVariant vtPromptUser;
		vtPromptUser.vt = VT_BOOL;
		vtPromptUser.boolVal = ATL_VARIANT_FALSE;

		return SUCCEEDED(spDoc->LoadDocument(&vtName,&vtPromptUser));
	}
	
	BOOL SaveDocument(LPCTSTR szName)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		ATLASSERT(spDoc != NULL);
		CComVariant vtName(szName);
		CComVariant vtPromptUser;
		vtPromptUser.vt = VT_BOOL;
		vtPromptUser.boolVal = ATL_VARIANT_FALSE;

		return SUCCEEDED(spDoc->SaveDocument(&vtName,&vtPromptUser));
	}
	
	BOOL PrintDocument(BOOL bShowUI=FALSE)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		CComVariant vPrint(bShowUI);
		return SUCCEEDED(spDoc->PrintDocument(&vPrint));
	}

//Properties
	BOOL GetDocument(IHTMLDocument2** ppDoc)
	{	
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return SUCCEEDED(spDoc->get_DOM(ppDoc));
	}

	BOOL GetDocumentHTML(CString& szHTML)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();

		USES_CONVERSION;
		
		ATLASSERT(spDoc != NULL);
		CComBSTR szBuff;
		if(SUCCEEDED(spDoc->get_DocumentHTML(&szBuff)))
		{
			szHTML = OLE2T(szBuff);
			return TRUE;
		}
		else
			return FALSE;
	}
	
	BOOL SetDocumentHTML(LPCTSTR szHTML)
	{
		USES_CONVERSION;
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return SUCCEEDED(spDoc->put_DocumentHTML(CComBSTR(szHTML)));
	}
	
	BOOL GetActivateApplets()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		VARIANT_BOOL bActivate;
		spDoc->get_ActivateApplets(&bActivate);
		return bActivate != ATL_VARIANT_FALSE ? TRUE : FALSE;
	}
	
	BOOL SetActivateApplets(BOOL bNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return S_OK == spDoc->put_ActivateApplets(bNewValue ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
	}
	
	BOOL GetActivateActiveXControls()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
	
		VARIANT_BOOL bActivate;
		spDoc->get_ActivateActiveXControls(&bActivate);
		return bActivate != ATL_VARIANT_FALSE ? TRUE : FALSE;
	}
	
	BOOL SetActivateActiveXControls(BOOL bNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return SUCCEEDED(spDoc->put_ActivateActiveXControls(bNewValue ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE));
	}
	
	BOOL GetActivateDTCs()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		VARIANT_BOOL bActivate;
		spDoc->get_ActivateDTCs(&bActivate);
		return bActivate != ATL_VARIANT_FALSE ? TRUE : FALSE;
	}
	
	BOOL SetActivateDTCs(BOOL bNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return S_OK == spDoc->put_ActivateDTCs(bNewValue ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
	}

	BOOL GetShowDetails()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		ATLASSERT(spDoc != NULL);
	
		VARIANT_BOOL bShowDetails;
		spDoc->get_ShowDetails(&bShowDetails);
		return bShowDetails != ATL_VARIANT_FALSE ? TRUE : FALSE;
	}
	
	BOOL SetShowDetails(BOOL bNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();

		ATLASSERT(spDoc != NULL);
		return S_OK == spDoc->put_ShowDetails(bNewValue ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
	}
	
	BOOL GetShowBorders()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
	
		VARIANT_BOOL bShowBorders;
		spDoc->get_ShowBorders(&bShowBorders);
		return bShowBorders != ATL_VARIANT_FALSE ? TRUE : FALSE;
	}
	
	BOOL SetShowBorders(BOOL bNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return S_OK == spDoc->put_ShowBorders(bNewValue ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
	}
	
	long GetAppearance()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
	
		long nAppearance;
		spDoc->get_Appearance((DHTMLEDITAPPEARANCE*)&nAppearance);
		return nAppearance;
	}
	
	BOOL SetAppearance(long nNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return SUCCEEDED(spDoc->put_Appearance((DHTMLEDITAPPEARANCE)nNewValue));
	}
	
	BOOL GetScrollbars()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
	
		VARIANT_BOOL bShowScrollBars;
		spDoc->get_Scrollbars(&bShowScrollBars);
		return bShowScrollBars != ATL_VARIANT_FALSE ? TRUE : FALSE;
	}
	
	BOOL SetScrollbars(BOOL bNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return S_OK == spDoc->put_Scrollbars(bNewValue ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
	}
	
	long GetScrollbarAppearance()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
	
		long nAppearance;
		spDoc->get_ScrollbarAppearance((DHTMLEDITAPPEARANCE*)&nAppearance);
		return nAppearance;
	}
	
	BOOL SetScrollbarAppearance(long nNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return SUCCEEDED(spDoc->put_ScrollbarAppearance((DHTMLEDITAPPEARANCE)nNewValue));
	}
	
	BOOL GetSourceCodePreservation()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
	
		VARIANT_BOOL bSrcPrev;
		spDoc->get_SourceCodePreservation(&bSrcPrev);
		return bSrcPrev != ATL_VARIANT_FALSE ? TRUE : FALSE;
	}
	
	BOOL SetSourceCodePreservation(BOOL bNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return S_OK == spDoc->put_SourceCodePreservation(bNewValue ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
	}
	
	BOOL GetAbsoluteDropMode()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		VARIANT_BOOL bAbsDropMode;
		spDoc->get_AbsoluteDropMode(&bAbsDropMode);
		return bAbsDropMode != ATL_VARIANT_FALSE ? TRUE : FALSE;
	}
	
	BOOL SetAbsoluteDropMode(BOOL bNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return S_OK == spDoc->put_AbsoluteDropMode(bNewValue ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
	}
	
	long GetSnapToGridX()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		long nSnap;
		spDoc->get_SnapToGridX(&nSnap);
		return nSnap;
	}
	
	BOOL SetSnapToGridX(long nNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return SUCCEEDED(spDoc->put_SnapToGridX(nNewValue));
	}
	
	long GetSnapToGridY()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		long nSnap;
		spDoc->get_SnapToGridY(&nSnap);
		return nSnap;
	}
	
	BOOL SetSnapToGridY(long nNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return SUCCEEDED(spDoc->put_SnapToGridY(nNewValue));
	}
	
	BOOL GetSnapToGrid()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
	
		VARIANT_BOOL bSnap;
		spDoc->get_SnapToGrid(&bSnap);
		return bSnap != ATL_VARIANT_FALSE ? TRUE : FALSE;
	}
	
	BOOL SetSnapToGrid(BOOL bNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return S_OK == spDoc->put_SnapToGrid(bNewValue ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
	}
	
	BOOL GetIsDirty()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		VARIANT_BOOL bIsDirty;
		spDoc->get_IsDirty(&bIsDirty);
		return bIsDirty != ATL_VARIANT_FALSE ? TRUE : FALSE;
	}
	
	BOOL GetCurrentDocumentPath(CString& szPath)
	{
		USES_CONVERSION;
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		CComBSTR szBuff;
		if(SUCCEEDED(spDoc->get_CurrentDocumentPath(&szBuff)))
		{
			szPath = OLE2T(szBuff);
			return TRUE;
		}
		else
			return FALSE;
	}
	BOOL GetBaseURL(CString &szURL)
	{
		USES_CONVERSION;
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		CComBSTR szBuff;
		if(SUCCEEDED(spDoc->get_BaseURL(&szBuff)))
		{
			szURL = OLE2T(szBuff);
			return TRUE;
		}
		else
			return FALSE;
	}
	
	BOOL SetBaseURL(LPCTSTR lpszNewValue)
	{
		USES_CONVERSION;
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		CComBSTR var(lpszNewValue);
		return SUCCEEDED(spDoc->put_BaseURL(var));
	}
	
	BOOL GetDocumentTitle(CString& szTitle)
	{
		USES_CONVERSION;
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		CComBSTR szBuff;
		if(SUCCEEDED(spDoc->get_DocumentTitle(&szBuff)))
		{
			szTitle = OLE2T(szBuff);
			return TRUE;
		}
		else
			return FALSE;
	}

	BOOL GetUseDivOnCarriageReturn()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		VARIANT_BOOL bValue;
		spDoc->get_UseDivOnCarriageReturn(&bValue);
		return bValue != ATL_VARIANT_FALSE ? TRUE : FALSE;
	}

	BOOL SetUseDivOnCarriageReturn(BOOL bNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return S_OK == spDoc->put_UseDivOnCarriageReturn(bNewValue ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
	}

	BOOL GetBusy()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		VARIANT_BOOL bValue;
		spDoc->get_Busy(&bValue);
		return bValue != ATL_VARIANT_FALSE ? TRUE : FALSE;
	}

	BOOL GetBrowseMode()
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		VARIANT_BOOL bMode;
		spDoc->get_BrowseMode(&bMode);
		return bMode != ATL_VARIANT_FALSE ? TRUE : FALSE;
	}
	
	BOOL SetBrowseMode(BOOL bNewValue)
	{
		T* pT = (T*)this;
		CComQIPtr<IDHTMLEdit> spDoc = pT->GetControlUnknown();
		
		ATLASSERT(spDoc != NULL);
		return S_OK == spDoc->put_BrowseMode(bNewValue ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
	}
	
//Commands
	BOOL Bold() const
	{
		return ExecCommand(DECMD_BOLD, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL Copy() const
	{
		return ExecCommand(DECMD_COPY, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL Cut() const
	{
		return ExecCommand(DECMD_CUT, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL Delete() const
	{
		return ExecCommand(DECMD_DELETE, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}	

	BOOL DeleteCells() const
	{
		return ExecCommand(DECMD_DELETECELLS, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL DeleteColumns() const
	{
		return ExecCommand(DECMD_DELETECOLS, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL DeleteRows() const
	{
		return ExecCommand(DECMD_DELETEROWS, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL FindText() const
	{
		return ExecCommand(DECMD_FINDTEXT, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL Font() const
	{
		return ExecCommand(DECMD_FONT,OLECMDEXECOPT_DODEFAULT,NULL, NULL);
	}

	BOOL GetBackColor(CString& szColor) const
	{
		CComVariant vaRet;
		BOOL bRet = FALSE;

		if (ExecCommand(DECMD_GETBACKCOLOR, OLECMDEXECOPT_DODEFAULT, NULL, &vaRet))
		{
			USES_CONVERSION;
			szColor = OLE2T(vaRet.bstrVal);
			bRet = TRUE;
		}

		return bRet;
	}

	BOOL GetBlockFormat(CString& szFormat) const
	{
		USES_CONVERSION;

		CComVariant vaRet;
		HRESULT hr = ExecCommand(DECMD_GETBACKCOLOR,OLECMDEXECOPT_DODEFAULT,NULL, &vaRet);

		if(SUCCEEDED(hr))
		{
			szFormat = OLE2T(vaRet.bstrVal);
			return TRUE;
		}
		else
			return FALSE;
	}

	BOOL GetBlockFormatNames(CStringArray &sa)
	{
		CComPtr<IDEGetBlockFmtNamesParam> spFmt;
		BOOL bRet = FALSE;
	
		//create a block format holder object
		if (FAILED(spFmt.CoCreateInstance(L"DEGetBlockFmtNamesParam.DEGetBlockFmtNamesParam")))
			return FALSE;
		
		CComVariant vaRet;
		CComVariant vaNameObj((LPUNKNOWN)spFmt);
		if (ExecCommand(DECMD_GETBLOCKFMTNAMES,OLECMDEXECOPT_DONTPROMPTUSER,&vaNameObj, NULL))
		{						 
			VariantInit(&vaRet);
			spFmt->get_Names(&vaRet);
			if((vaRet.vt & VT_ARRAY) && (vaRet.vt & VT_BSTR))
			{
				USES_CONVERSION;
				SAFEARRAY *psa = vaRet.parray;
				VARIANT vaName;
				long lBound = 0,uBound = 0;
				if(S_OK == SafeArrayGetLBound(psa,1,&lBound) &&
				   S_OK == SafeArrayGetUBound(psa,1,&uBound) )
				{
					for(long i=lBound; i<uBound; i++)
					{	
						if( (S_OK == SafeArrayGetElement(psa, &i, &vaName)) && (vaName.vt & VT_BSTR) )
							sa.Add(CString(OLE2T(vaName.bstrVal)));
					}				
					bRet = TRUE;
				}
			}
		}
		return bRet;
	}

	BOOL GetFontFace(CString& szFace) const
	{
		USES_CONVERSION;

		CComVariant vaRet;
		if(ExecCommand(DECMD_GETFONTNAME,OLECMDEXECOPT_DODEFAULT,NULL,&vaRet ))
		{
			szFace = OLE2T(vaRet.bstrVal);
			return TRUE;
		}
		else
			return FALSE;
	}

	short GetFontSize() const
	{
		CComVariant vaRet;
		if (ExecCommand(DECMD_GETFONTSIZE,OLECMDEXECOPT_DODEFAULT,NULL,&vaRet ) && vaRet.vt==VT_I4)
			return vaRet.iVal;
		return -1;
	}

	BOOL GetForeColor(CString& szColor) const
	{
		USES_CONVERSION;
		CComVariant vaRet;
		HRESULT hr = ExecCommand(DECMD_GETFORECOLOR,OLECMDEXECOPT_DODEFAULT,NULL, &vaRet);
		if(SUCCEEDED(hr))
		{
			szColor = OLE2T(vaRet.bstrVal);
			return TRUE;
		}
		else
			return FALSE;
	}

	BOOL HyperLink() const
	{
		return ExecCommand(DECMD_HYPERLINK, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL Image() const
	{
		return ExecCommand(DECMD_IMAGE, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL Indent() const
	{
		return ExecCommand(DECMD_INDENT, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL InsertCell() const
	{
		return ExecCommand(DECMD_INSERTCELL, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL InsertColumn() const
	{
		return ExecCommand(DECMD_INSERTCOL, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL InsertRow() const
	{
		return ExecCommand(DECMD_INSERTROW, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL InsertTable(DWORD dwRows, DWORD dwCols, LPCTSTR szTableAttr, LPCTSTR szCellAttr, LPCTSTR szCaption) const
	{
		CComPtr<IDEInsertTableParam> spTable = NULL;
	
		VARIANT vaTable;
		VariantInit(&vaTable);
		USES_CONVERSION;
	
		if(FAILED(spTable.CoCreateInstance(L"DEInsertTableParam.DEInsertTableParam")))
			return FALSE;
	
		CComBSTR sztable(szTableAttr), szcell(szCellAttr), szcap(szCaption);

		//set all of the table object's properties
		if(FAILED(spTable->put_NumRows(dwRows)))
			return FALSE;
		if(FAILED(spTable->put_NumCols(dwCols)))
			return FALSE;
		if(FAILED(spTable->put_TableAttrs(sztable)))
			return FALSE;
		if(FAILED(spTable->put_CellAttrs(szcell)))
			return FALSE;
		if(FAILED(spTable->put_Caption(szcap)))
			return FALSE;
	
		vaTable.vt = VT_UNKNOWN;
		vaTable.punkVal = (LPUNKNOWN) spTable;
	
		if(!ExecCommand(DECMD_INSERTTABLE,OLECMDEXECOPT_DONTPROMPTUSER, &vaTable,NULL))
			return FALSE;
		
		return TRUE;
	}

	BOOL Italic() const
	{
		return ExecCommand(DECMD_ITALIC, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL JustifyCenter() const
	{
		return ExecCommand(DECMD_JUSTIFYCENTER, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL JustifyLeft() const
	{
		return ExecCommand(DECMD_JUSTIFYLEFT, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL JustifyRight() const
	{
		return ExecCommand(DECMD_JUSTIFYRIGHT, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL LockElement() const
	{
		return ExecCommand(DECMD_LOCK_ELEMENT, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL MakeAbsolute() const
	{
		return ExecCommand(DECMD_MAKE_ABSOLUTE, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL MergeCells() const
	{
		return ExecCommand(DECMD_MERGECELLS, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL OrderList() const
	{
		return ExecCommand(DECMD_ORDERLIST, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL Outdent() const
	{
		return ExecCommand(DECMD_OUTDENT, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL Paste() const
	{
		return ExecCommand(DECMD_PASTE, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL Redo() const
	{
		return ExecCommand(DECMD_REDO, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL RemoveFormat() const
	{
		return ExecCommand(DECMD_REMOVEFORMAT, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL SelectAll() const
	{
		return ExecCommand(DECMD_SELECTALL, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL SendBackward() const
	{
		return ExecCommand(DECMD_SEND_BACKWARD, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL BringForward() const
	{
		return ExecCommand(DECMD_BRING_FORWARD, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL SendBelowText() const
	{
		return ExecCommand(DECMD_SEND_BELOW_TEXT, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}
	
	BOOL BringAboveText() const
	{
		return ExecCommand(DECMD_BRING_ABOVE_TEXT, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL SendToBack() const
	{
		return ExecCommand(DECMD_SEND_TO_BACK, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}
	
	BOOL BringToFront() const
	{
		return ExecCommand(DECMD_BRING_TO_FRONT, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL SetBackColor(LPCTSTR szColor) const
	{
		CComVariant vaName(szColor);
		return ExecCommand(DECMD_SETBACKCOLOR,OLECMDEXECOPT_DODEFAULT,&vaName, NULL);
	}

	BOOL SetBlockFormat(LPCTSTR szFormat) const
	{
		CComVariant vaName(szFormat);
		return ExecCommand(DECMD_SETBLOCKFMT,OLECMDEXECOPT_DODEFAULT,&vaName, NULL);
	}

	BOOL SetFontFace(LPCTSTR szFont) const
	{
		CComVariant vaName(szFont);
		return ExecCommand(DECMD_SETFONTNAME,OLECMDEXECOPT_DODEFAULT,&vaName, NULL);
	}

	BOOL SetFontSize(short size) const
	{
		CComVariant vaSize(size);
		return ExecCommand(DECMD_SETFONTSIZE,OLECMDEXECOPT_DODEFAULT,&vaSize, NULL);
	}
	
	BOOL SetForeColor(LPCTSTR szColor) const
	{
		CComVariant vaName(szColor);
		return ExecCommand(DECMD_SETFORECOLOR,OLECMDEXECOPT_DODEFAULT,&vaName, NULL);
	}

	BOOL SplitCell() const
	{
		return ExecCommand(DECMD_SPLITCELL, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL Underline() const
	{
		return ExecCommand(DECMD_UNDERLINE, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL Undo() const
	{
		return ExecCommand(DECMD_UNDO, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL Unlink() const
	{
		return ExecCommand(DECMD_UNLINK, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL UnorderList() const
	{
		return ExecCommand(DECMD_UNORDERLIST, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}

	BOOL Properties() const
	{
		return ExecCommand(DECMD_PROPERTIES, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
	}
}; //CHtmlEditCtrlBase

/////////////////////////////////////////////////////////////////////////////
// CHtmlEditCtrl
class CHtmlEditCtrl:
	public CWnd,
	public CHtmlEditCtrlBase<CHtmlEditCtrl>
{
// Construction
public:
	CHtmlEditCtrl();
	virtual ~CHtmlEditCtrl();
	virtual BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, UINT nID,
		CCreateContext* pContext = NULL);
};


/////////////////////////////////////////////////////////////////////////////
// CHtmlEditView
class CHtmlEditView :
	public CFormView,
	public CHtmlEditCtrlBase<CHtmlEditView>
{
protected:
	CHtmlEditView();
	DECLARE_DYNCREATE(CHtmlEditView)
	DECLARE_EVENTSINK_MAP()
public:
	LPUNKNOWN GetControlUnknown()
	{
		return m_wndTriEdit.GetControlUnknown();
	}
//event handlers
	virtual void OnDocumentComplete();
	virtual void OnDisplayChanged();
	virtual void OnShowContextMenu(long xPos, long yPos);
	virtual void OnContextMenuAction(long itemIndex);
	virtual void OnMouseDown();
	virtual void OnMouseMove();
	virtual void OnMouseUp();
	virtual void OnMouseOut();
	virtual void OnMouseOver();
	virtual void OnClick();
	virtual void OnDblClick();
	virtual void OnKeyDown();
	virtual void OnKeyPress();
	virtual void OnKeyUp();
	virtual void OnBlur();
	virtual void OnReadyStateChange();
	
// Implementation
public:
	void OnFilePrint();
	void OnCopy();
	void OnCut();
	void OnPaste();
	void OnUndo();
	void OnSelectAll();
	void OnUpdateCopyUI(CCmdUI *pCmdUI);
	void OnUpdateCutUI(CCmdUI *pCmdUI);
	void OnUpdatePasteUI(CCmdUI *pCmdUI);
	void OnUpdateUndoUI(CCmdUI *pCmdUI);
	void OnUpdateSelectAllUI(CCmdUI *pCmdUI);

protected:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
		DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID,
		CCreateContext* pContext = NULL);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
//data
	CHtmlEditCtrl m_wndTriEdit;

//message handlers
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);

public:
	virtual ~CHtmlEditView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	DECLARE_MESSAGE_MAP()
};

class AFX_NOVTABLE CHtmlEditDoc : public CDocument
{
protected: // create from serialization only
	CHtmlEditDoc();
	DECLARE_DYNAMIC(CHtmlEditDoc)

// Attributes
public:
	virtual CHtmlEditView* GetView() const;
	virtual BOOL OpenURL(LPCTSTR lpszURL);
// Implementation
public:
	virtual ~CHtmlEditDoc() = 0 { }
	virtual void DeleteContents();
	virtual BOOL IsModified();
	virtual BOOL OnOpenDocument(LPCTSTR lpszFileName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszFileName);
	virtual BOOL OnNewDocument();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

/////////////////////////////////////////////////////////////////////////////
// Inline function declarations

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif

#ifdef _AFX_ENABLE_INLINES
#define _AFXHTML_INLINE AFX_INLINE
#include <afxhtml.inl>
#endif

#undef AFX_DATA
#define AFX_DATA

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif
#ifndef _AFX_FULLTYPEINFO
#pragma component(mintypeinfo, off)
#endif

#endif // __AFXHTML_H__

