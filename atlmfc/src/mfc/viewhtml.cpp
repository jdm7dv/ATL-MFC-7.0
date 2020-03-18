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

#include <mshtmcid.h>	// CMDSETID_Forms3 definition
#include <mshtmhst.h>	// IDM_menu item definitions
#include <mshtml.h>

#ifdef AFX_HTML_SEG
#pragma code_seg(AFX_HTML_SEG)
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHtmlView

BEGIN_MESSAGE_MAP(CHtmlView, CFormView)
	//{{AFX_MSG_MAP(CHtmlView)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CHtmlView, CFormView)
	ON_EVENT(CHtmlView, AFX_IDW_PANE_FIRST, 102 /* StatusTextChange */, OnStatusTextChange, VTS_BSTR)
	ON_EVENT(CHtmlView, AFX_IDW_PANE_FIRST, 108 /* ProgressChange */, OnProgressChange, VTS_I4 VTS_I4)
	ON_EVENT(CHtmlView, AFX_IDW_PANE_FIRST, 105 /* CommandStateChange */, OnCommandStateChange, VTS_I4 VTS_BOOL)
	ON_EVENT(CHtmlView, AFX_IDW_PANE_FIRST, 106 /* DownloadBegin */, OnDownloadBegin, VTS_NONE)
	ON_EVENT(CHtmlView, AFX_IDW_PANE_FIRST, 104 /* DownloadComplete */, OnDownloadComplete, VTS_NONE)
	ON_EVENT(CHtmlView, AFX_IDW_PANE_FIRST, 113 /* TitleChange */, OnTitleChange, VTS_BSTR)
	ON_EVENT(CHtmlView, AFX_IDW_PANE_FIRST, 252 /* NavigateComplete2 */, NavigateComplete2, VTS_DISPATCH VTS_PVARIANT)
	ON_EVENT(CHtmlView, AFX_IDW_PANE_FIRST, 250 /* BeforeNavigate2 */, BeforeNavigate2, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	ON_EVENT(CHtmlView, AFX_IDW_PANE_FIRST, 112 /* PropertyChange */, OnPropertyChange, VTS_BSTR)
	ON_EVENT(CHtmlView, AFX_IDW_PANE_FIRST, 251 /* NewWindow2 */, OnNewWindow2, VTS_PDISPATCH VTS_PBOOL)
	ON_EVENT(CHtmlView, AFX_IDW_PANE_FIRST, 259 /* DocumentComplete */, DocumentComplete, VTS_DISPATCH VTS_PVARIANT)
	ON_EVENT(CHtmlView, AFX_IDW_PANE_FIRST, 253 /* OnQuit */, OnQuit, VTS_NONE)
	ON_EVENT(CHtmlView, AFX_IDW_PANE_FIRST, 254 /* OnVisible */, OnVisible, VTS_BOOL)
	ON_EVENT(CHtmlView, AFX_IDW_PANE_FIRST, 255 /* OnToolBar */, OnToolBar, VTS_BOOL)
	ON_EVENT(CHtmlView, AFX_IDW_PANE_FIRST, 256 /* OnMenuBar */, OnMenuBar, VTS_BOOL)
	ON_EVENT(CHtmlView, AFX_IDW_PANE_FIRST, 257 /* OnStatusBar */, OnStatusBar, VTS_BOOL)
	ON_EVENT(CHtmlView, AFX_IDW_PANE_FIRST, 258 /* OnFullScreen */, OnFullScreen, VTS_BOOL)
	ON_EVENT(CHtmlView, AFX_IDW_PANE_FIRST, 260 /* OnTheaterMode */, OnTheaterMode, VTS_BOOL)
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHtmlView construction/destruction

CHtmlView::CHtmlView()
	: CFormView((LPCTSTR) NULL)
{
	m_pBrowserApp = NULL;
}

CHtmlView::~CHtmlView()
{
	if (m_pBrowserApp != NULL)
		m_pBrowserApp->Release();
}

BOOL CHtmlView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_CLIPCHILDREN;

	return CFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlView drawing

void CHtmlView::OnDraw(CDC* /* pDC */)
{
	// this class should never do its own drawing;
	// the browser control should handle everything

	ASSERT(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlView printing

void CHtmlView::OnFilePrint()
{
	// get the HTMLDocument

	if (m_pBrowserApp != NULL)
	{
		LPOLECOMMANDTARGET lpTarget = NULL;
		LPDISPATCH lpDisp = GetHtmlDocument();

		if (lpDisp != NULL)
		{
			// the control will handle all printing UI

			if (SUCCEEDED(lpDisp->QueryInterface(IID_IOleCommandTarget,
					(LPVOID*) &lpTarget)))
			{
				lpTarget->Exec(NULL, OLECMDID_PRINT, 0, NULL, NULL);
				lpTarget->Release();
			}
			lpDisp->Release();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlView diagnostics

#ifdef _DEBUG
void CHtmlView::AssertValid() const
{
	CFormView::AssertValid();
}

void CHtmlView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHtmlView message handlers

void CHtmlView::OnDestroy()
{
	RELEASE(m_pBrowserApp);
}

void CHtmlView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if (::IsWindow(m_wndBrowser.m_hWnd))
	{
		// need to push non-client borders out of the client area
		CRect rect;
		GetClientRect(rect);
		::AdjustWindowRectEx(rect,
			m_wndBrowser.GetStyle(), FALSE, WS_EX_CLIENTEDGE);
		m_wndBrowser.SetWindowPos(NULL, rect.left, rect.top,
			rect.Width(), rect.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CHtmlView::OnPaint()
{
	Default();
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlView operations

#pragma warning(disable:4100)

class CHtmlControlSite : public COleControlSite
{
public:
	CHtmlControlSite(COleControlContainer* pParentWnd);
	~CHtmlControlSite();

	CHtmlView* GetView() const;

	BEGIN_INTERFACE_PART(DocHostUIHandler, IDocHostUIHandler)
		STDMETHOD(ShowContextMenu)(DWORD, LPPOINT, LPUNKNOWN, LPDISPATCH);
		STDMETHOD(GetHostInfo)(DOCHOSTUIINFO*);
		STDMETHOD(ShowUI)(DWORD, LPOLEINPLACEACTIVEOBJECT,
			LPOLECOMMANDTARGET, LPOLEINPLACEFRAME, LPOLEINPLACEUIWINDOW);
		STDMETHOD(HideUI)(void);
		STDMETHOD(UpdateUI)(void);
		STDMETHOD(EnableModeless)(BOOL);
		STDMETHOD(OnDocWindowActivate)(BOOL);
		STDMETHOD(OnFrameWindowActivate)(BOOL);
		STDMETHOD(ResizeBorder)(LPCRECT, LPOLEINPLACEUIWINDOW, BOOL);
		STDMETHOD(TranslateAccelerator)(LPMSG, const GUID*, DWORD);
		STDMETHOD(GetOptionKeyPath)(OLECHAR **, DWORD);
		STDMETHOD(GetDropTarget)(LPDROPTARGET, LPDROPTARGET*);
		STDMETHOD(GetExternal)(LPDISPATCH*);
		STDMETHOD(TranslateUrl)(DWORD, OLECHAR*, OLECHAR **);
		STDMETHOD(FilterDataObject)(LPDATAOBJECT , LPDATAOBJECT*);
	END_INTERFACE_PART(DocHostUIHandler)

	DECLARE_INTERFACE_MAP()
};

BEGIN_INTERFACE_MAP(CHtmlControlSite, COleControlSite)
	INTERFACE_PART(CHtmlControlSite, IID_IDocHostUIHandler, DocHostUIHandler)
END_INTERFACE_MAP()

CHtmlControlSite::CHtmlControlSite(COleControlContainer* pContainer)
: COleControlSite(pContainer)
{
}

CHtmlControlSite::~CHtmlControlSite()
{
}

inline CHtmlView* CHtmlControlSite::GetView() const
{
	return STATIC_DOWNCAST(CHtmlView, m_pCtrlCont->m_pWnd);
}

STDMETHODIMP CHtmlControlSite::XDocHostUIHandler::GetExternal(LPDISPATCH *lppDispatch)
{
	METHOD_PROLOGUE_EX_(CHtmlControlSite, DocHostUIHandler)

	CHtmlView* pView = pThis->GetView();
	return pView->OnGetExternal(lppDispatch);
}

HRESULT CHtmlView::OnGetExternal(LPDISPATCH*)
{
	// default tells control we don't have an external
	return S_FALSE;
}

STDMETHODIMP CHtmlControlSite::XDocHostUIHandler::ShowContextMenu(
	DWORD dwID, LPPOINT ppt, LPUNKNOWN pcmdtReserved, LPDISPATCH pdispReserved)
{
	METHOD_PROLOGUE_EX_(CHtmlControlSite, DocHostUIHandler)

	CHtmlView* pView = pThis->GetView();
	return pView->OnShowContextMenu(dwID, ppt, pcmdtReserved, pdispReserved);
}

HRESULT CHtmlView::OnShowContextMenu(DWORD, LPPOINT, LPUNKNOWN, LPDISPATCH)
{
	// default tells control that we didn't show a menu,
	// and the control should show the menu

	return S_FALSE;
}

STDMETHODIMP CHtmlControlSite::XDocHostUIHandler::GetHostInfo(
	DOCHOSTUIINFO *pInfo)
{
	METHOD_PROLOGUE_EX_(CHtmlControlSite, DocHostUIHandler)
	CHtmlView* pView = pThis->GetView();
	return pView->OnGetHostInfo(pInfo);
}

HRESULT CHtmlView::OnGetHostInfo(DOCHOSTUIINFO*)
{
	// default indicates we don't have info
	return S_OK;
}

STDMETHODIMP CHtmlControlSite::XDocHostUIHandler::ShowUI(
	DWORD dwID, LPOLEINPLACEACTIVEOBJECT pActiveObject,
	LPOLECOMMANDTARGET pCommandTarget, LPOLEINPLACEFRAME pFrame,
	LPOLEINPLACEUIWINDOW pDoc)
{
	METHOD_PROLOGUE_EX_(CHtmlControlSite, DocHostUIHandler)

	CHtmlView* pView = pThis->GetView();
	return pView->OnShowUI(dwID, pActiveObject, pCommandTarget, pFrame, pDoc);
}

HRESULT CHtmlView::OnShowUI(DWORD, LPOLEINPLACEACTIVEOBJECT,
	LPOLECOMMANDTARGET, LPOLEINPLACEFRAME, LPOLEINPLACEUIWINDOW)
{
	// default means we don't have any UI, and control should show its UI

	return S_FALSE;
}

STDMETHODIMP CHtmlControlSite::XDocHostUIHandler::HideUI(void)
{
	METHOD_PROLOGUE_EX_(CHtmlControlSite, DocHostUIHandler)

	CHtmlView* pView = pThis->GetView();
	return pView->OnHideUI();
}

HRESULT CHtmlView::OnHideUI()
{
	// we don't have UI by default, so just pretend we hid it
	return S_OK;
}

STDMETHODIMP CHtmlControlSite::XDocHostUIHandler::UpdateUI(void)
{
	METHOD_PROLOGUE_EX_(CHtmlControlSite, DocHostUIHandler)

	CHtmlView* pView = pThis->GetView();
	return pView->OnUpdateUI();
}

HRESULT CHtmlView::OnUpdateUI()
{
	// we don't have UI by default, so just pretend we updated it
	return S_OK;
}

STDMETHODIMP CHtmlControlSite::XDocHostUIHandler::EnableModeless(BOOL fEnable)
{
	METHOD_PROLOGUE_EX_(CHtmlControlSite, DocHostUIHandler)
	CHtmlView* pView = pThis->GetView();
	return pView->OnEnableModeless(fEnable);
}

HRESULT CHtmlView::OnEnableModeless(BOOL)
{
	// we don't have any UI by default, so pretend we updated it
	return S_OK;
}

STDMETHODIMP CHtmlControlSite::XDocHostUIHandler::OnDocWindowActivate(BOOL fActivate)
{
	METHOD_PROLOGUE_EX_(CHtmlControlSite, DocHostUIHandler)
	CHtmlView* pView = pThis->GetView();
	return pView->OnDocWindowActivate(fActivate);
}

HRESULT CHtmlView::OnDocWindowActivate(BOOL)
{
	// we don't have any UI by default, so pretend we updated it
	return S_OK;
}

STDMETHODIMP CHtmlControlSite::XDocHostUIHandler::OnFrameWindowActivate(
	BOOL fActivate)
{
	METHOD_PROLOGUE_EX_(CHtmlControlSite, DocHostUIHandler)
	CHtmlView* pView = pThis->GetView();
	return pView->OnFrameWindowActivate(fActivate);
}

HRESULT CHtmlView::OnFrameWindowActivate(BOOL)
{
	// we don't have any UI by default, so pretend we updated it
	return S_OK;
}

STDMETHODIMP CHtmlControlSite::XDocHostUIHandler::ResizeBorder(
	LPCRECT prcBorder, LPOLEINPLACEUIWINDOW pUIWindow, BOOL fFrameWindow)
{
	METHOD_PROLOGUE_EX_(CHtmlControlSite, DocHostUIHandler)
	CHtmlView* pView = pThis->GetView();
	return pView->OnResizeBorder(prcBorder, pUIWindow, fFrameWindow);
}

HRESULT CHtmlView::OnResizeBorder(LPCRECT, LPOLEINPLACEUIWINDOW, BOOL)
{
	// we don't have any UI by default, so pretend we updated it
	return S_OK;
}

STDMETHODIMP CHtmlControlSite::XDocHostUIHandler::TranslateAccelerator(
	LPMSG lpMsg, const GUID* pguidCmdGroup, DWORD nCmdID)
{
	METHOD_PROLOGUE_EX_(CHtmlControlSite, DocHostUIHandler)
	CHtmlView* pView = pThis->GetView();
	return pView->OnTranslateAccelerator(lpMsg, pguidCmdGroup, nCmdID);
}

HRESULT CHtmlView::OnTranslateAccelerator(LPMSG, const GUID*, DWORD)
{
	// no translation here
	return S_FALSE;
}

STDMETHODIMP CHtmlControlSite::XDocHostUIHandler::GetOptionKeyPath(
	LPOLESTR* pchKey, DWORD dwReserved)
{
	METHOD_PROLOGUE_EX_(CHtmlControlSite, DocHostUIHandler)
	CHtmlView* pView = pThis->GetView();
	return pView->OnGetOptionKeyPath(pchKey, dwReserved);
}

HRESULT CHtmlView::OnGetOptionKeyPath(LPOLESTR*, DWORD)	
{
	// no replacement option key
	return S_FALSE;
}

STDMETHODIMP CHtmlControlSite::XDocHostUIHandler::GetDropTarget(
	LPDROPTARGET pDropTarget, LPDROPTARGET* ppDropTarget)
{
	METHOD_PROLOGUE_EX_(CHtmlControlSite, DocHostUIHandler)
	CHtmlView* pView = pThis->GetView();
	return pView->OnGetDropTarget(pDropTarget, ppDropTarget);
}

HRESULT CHtmlView::OnGetDropTarget(LPDROPTARGET, LPDROPTARGET*)
{
	// no additional drop target
	return S_FALSE;
}

STDMETHODIMP CHtmlControlSite::XDocHostUIHandler::TranslateUrl(
	DWORD dwTranslate, OLECHAR* pchURLIn, OLECHAR** ppchURLOut)
{
	METHOD_PROLOGUE_EX_(CHtmlControlSite, DocHostUIHandler)
	CHtmlView* pView = pThis->GetView();
	return pView->OnTranslateUrl(dwTranslate, pchURLIn, ppchURLOut);
}

HRESULT CHtmlView::OnTranslateUrl(DWORD, OLECHAR*, OLECHAR**)
{
	// no translation happens by default
	return S_FALSE;
}

STDMETHODIMP CHtmlControlSite::XDocHostUIHandler::FilterDataObject(
	LPDATAOBJECT pDataObject, LPDATAOBJECT* ppDataObject)
{
	METHOD_PROLOGUE_EX_(CHtmlControlSite, DocHostUIHandler)
	CHtmlView* pView = pThis->GetView();
	return pView->OnFilterDataObject(pDataObject, ppDataObject);
}

HRESULT CHtmlView::OnFilterDataObject(LPDATAOBJECT pDataObject,
									LPDATAOBJECT* ppDataObject)
{
	// no data objects by default
	return S_FALSE;
}

STDMETHODIMP_(ULONG) CHtmlControlSite::XDocHostUIHandler::AddRef()
{
	METHOD_PROLOGUE_EX_(CHtmlControlSite, DocHostUIHandler)
	return pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CHtmlControlSite::XDocHostUIHandler::Release()
{
	METHOD_PROLOGUE_EX_(CHtmlControlSite, DocHostUIHandler)
	return pThis->ExternalRelease();
}

STDMETHODIMP CHtmlControlSite::XDocHostUIHandler::QueryInterface(
          REFIID iid, LPVOID far* ppvObj)     
{
	METHOD_PROLOGUE_EX_(CHtmlControlSite, DocHostUIHandler)
	return pThis->ExternalQueryInterface(&iid, ppvObj);
}

BOOL CHtmlView::CreateControlSite(COleControlContainer* pContainer, 
   COleControlSite** ppSite, UINT /* nID */, REFCLSID /* clsid */)
{
	ASSERT(ppSite != NULL);
	*ppSite = new CHtmlControlSite(pContainer);
	return TRUE;
}

BOOL CHtmlView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
						DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
						UINT nID, CCreateContext* pContext)
{
	// create the view window itself
	m_pCreateContext = pContext;
	if (!CView::Create(lpszClassName, lpszWindowName,
				dwStyle, rect, pParentWnd,  nID, pContext))
	{
		return FALSE;
	}

	// assure that control containment is on
	AfxEnableControlContainer();

	RECT rectClient;
	GetClientRect(&rectClient);

	// create the control window
	// AFX_IDW_PANE_FIRST is a safe but arbitrary ID
	if (!m_wndBrowser.CreateControl(CLSID_WebBrowser, lpszWindowName,
				WS_VISIBLE | WS_CHILD, rectClient, this, AFX_IDW_PANE_FIRST))
	{
		DestroyWindow();
		return FALSE;
	}

	// cache the dispinterface
	LPUNKNOWN lpUnk = m_wndBrowser.GetControlUnknown();
	HRESULT hr = lpUnk->QueryInterface(IID_IWebBrowser2, (void**) &m_pBrowserApp);
	if (!SUCCEEDED(hr))
	{
		m_pBrowserApp = NULL;
		m_wndBrowser.DestroyWindow();
		DestroyWindow();
		return FALSE;
	}

	return TRUE;
}

BOOL CHtmlView::PreTranslateMessage(MSG* pMsg) 
{
	ASSERT(pMsg != NULL);
	ASSERT_VALID(this);
	ASSERT(m_hWnd != NULL);

	// allow tooltip messages to be filtered (skip CFormView)
	if (CView::PreTranslateMessage(pMsg))
		return TRUE;

	// don't translate dialog messages when in Shift+F1 help mode
	CFrameWnd* pFrameWnd = GetTopLevelFrame();
	if (pFrameWnd != NULL && pFrameWnd->m_bHelpMode)
		return FALSE;

	// call all frame windows' PreTranslateMessage first
	pFrameWnd = GetParentFrame();   // start with first parent frame
	while (pFrameWnd != NULL)
	{
		// allow owner & frames to translate
		if (pFrameWnd->PreTranslateMessage(pMsg))
			return TRUE;

		// try parent frames until there are no parent frames
		pFrameWnd = pFrameWnd->GetParentFrame();
	}

	// check if the browser control wants to handle the message
	BOOL bRet = FALSE;
	IOleInPlaceActiveObject* pInPlace;
	if(m_pBrowserApp != NULL && SUCCEEDED(m_pBrowserApp->QueryInterface(IID_IOleInPlaceActiveObject, (void**)&pInPlace)))
	{
		bRet = (pInPlace->TranslateAccelerator(pMsg) == S_OK) ? TRUE : FALSE;
		pInPlace->Release();
	}

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlView properties

CString CHtmlView::GetType() const
{
	ASSERT(m_pBrowserApp != NULL);

	CComBSTR bstr;
	m_pBrowserApp->get_Type(&bstr);
	CString retVal(bstr);
	return retVal;
}

long CHtmlView::GetLeft() const
{
	ASSERT(m_pBrowserApp != NULL);

	long result;
	m_pBrowserApp->get_Left(&result);
	return result;
}


long CHtmlView::GetTop() const
{
	ASSERT(m_pBrowserApp != NULL);
	long result;
	m_pBrowserApp->get_Top(&result);
	return result;
}

int CHtmlView::GetToolBar() const
{
	ASSERT(m_pBrowserApp != NULL);
	int result;
	m_pBrowserApp->get_ToolBar(&result);
	return result;
}

long CHtmlView::GetHeight() const
{
	ASSERT(m_pBrowserApp != NULL);
	long result;
	m_pBrowserApp->get_Height(&result);
	return result;
}

long CHtmlView::GetWidth() const
{
	ASSERT(m_pBrowserApp != NULL);
	long result;
	m_pBrowserApp->get_Width(&result);
	return result;
}

BOOL CHtmlView::GetVisible() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_Visible(&result);
	return result;
}

CString CHtmlView::GetLocationName() const
{
	ASSERT(m_pBrowserApp != NULL);

	CComBSTR bstr;
	m_pBrowserApp->get_LocationName(&bstr);
	CString retVal(bstr);
	return retVal;
}

CString CHtmlView::GetLocationURL() const
{
	ASSERT(m_pBrowserApp != NULL);

	CComBSTR bstr;
	m_pBrowserApp->get_LocationURL(&bstr);
	CString retVal(bstr);
	return retVal;
}

BOOL CHtmlView::GetBusy() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_Busy(&result);
	return result;
}

READYSTATE CHtmlView::GetReadyState() const
{
	ASSERT(m_pBrowserApp != NULL);

	READYSTATE result;
	m_pBrowserApp->get_ReadyState(&result);
	return result;
}

BOOL CHtmlView::GetOffline() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_Offline(&result);
	return result;
}

BOOL CHtmlView::GetSilent() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_Silent(&result);
	return result;
}

LPDISPATCH CHtmlView::GetApplication() const
{
	ASSERT(m_pBrowserApp != NULL);

	LPDISPATCH result;
	m_pBrowserApp->get_Application(&result);
	return result;
}


LPDISPATCH CHtmlView::GetParentBrowser() const
{
	ASSERT(m_pBrowserApp != NULL);

	LPDISPATCH result;
	m_pBrowserApp->get_Parent(&result);
	return result;
}

LPDISPATCH CHtmlView::GetContainer() const
{
	ASSERT(m_pBrowserApp != NULL);

	LPDISPATCH result;
	m_pBrowserApp->get_Container(&result);
	return result;
}

LPDISPATCH CHtmlView::GetHtmlDocument() const
{
	ASSERT(m_pBrowserApp != NULL);

	LPDISPATCH result;
	m_pBrowserApp->get_Document(&result);
	return result;
}

BOOL CHtmlView::GetTopLevelContainer() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_TopLevelContainer(&result);
	return result;
}

BOOL CHtmlView::GetMenuBar() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_MenuBar(&result);
	return result;
}

BOOL CHtmlView::GetFullScreen() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_FullScreen(&result);
	return result;
}

BOOL CHtmlView::GetStatusBar() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_StatusBar(&result);
	return result;
}

OLECMDF CHtmlView::QueryStatusWB(OLECMDID cmdID) const
{
	ASSERT(m_pBrowserApp != NULL);

	OLECMDF result;
	m_pBrowserApp->QueryStatusWB(cmdID, &result);
	return result;
}

void CHtmlView::ExecWB(OLECMDID cmdID, OLECMDEXECOPT cmdexecopt,
	VARIANT* pvaIn, VARIANT* pvaOut)
{
	ASSERT(m_pBrowserApp != NULL);

	m_pBrowserApp->ExecWB(cmdID, cmdexecopt, pvaIn, pvaOut);
}

HRESULT CHtmlView::ExecFormsCommand(DWORD dwCommandID,
	VARIANT* pVarIn, VARIANT* pVarOut)
{
	HRESULT hr = E_FAIL;

	IHTMLDocument2* pDoc = (IHTMLDocument2*) GetHtmlDocument();
	if (pDoc != NULL)
	{
		IOleCommandTarget* pCmdTarget = NULL;
		hr = pDoc->QueryInterface(IID_IOleCommandTarget, (void**) &pCmdTarget);

		if (SUCCEEDED(hr) && pCmdTarget != NULL)
		{
			hr = pCmdTarget->Exec(&CMDSETID_Forms3, dwCommandID,
				OLECMDEXECOPT_DONTPROMPTUSER, pVarOut, pVarIn);
			pCmdTarget->Release();
		}
		pDoc->Release();
	}

	return hr;
}

HRESULT CHtmlView::QueryFormsCommand(DWORD dwCommandID,
	BOOL* pbSupported, BOOL* pbEnabled, BOOL* pbChecked)
{
	HRESULT hr = E_FAIL;

	IHTMLDocument2* pDoc = (IHTMLDocument2*) GetHtmlDocument();
	if (pDoc != NULL)
	{
		IOleCommandTarget* pCmdTarget = NULL;
		hr = pDoc->QueryInterface(IID_IOleCommandTarget, (void**) &pCmdTarget);
		if (SUCCEEDED(hr) && pCmdTarget != NULL)
		{
			OLECMD cmdInfo;
			cmdInfo.cmdID = dwCommandID;
			cmdInfo.cmdf = 0;

			hr = pCmdTarget->QueryStatus(&CMDSETID_Forms3, 1, &cmdInfo, NULL);

			if (SUCCEEDED(hr))
			{
				if (pbSupported != NULL)
					*pbSupported = (cmdInfo.cmdf & OLECMDF_SUPPORTED) ? TRUE : FALSE;
				if (pbEnabled != NULL)
					*pbEnabled = (cmdInfo.cmdf & OLECMDF_ENABLED) ? TRUE : FALSE;
				if (pbChecked != NULL)
					*pbChecked = (cmdInfo.cmdf & OLECMDF_LATCHED) ? TRUE : FALSE;
			}
			pCmdTarget->Release();
		}

		pDoc->Release();
	}

	return hr;
}

BOOL CHtmlView::GetRegisterAsBrowser() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_RegisterAsBrowser(&result);
	return result;
}

BOOL CHtmlView::GetRegisterAsDropTarget() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_RegisterAsDropTarget(&result);
	return result;
}

BOOL CHtmlView::GetTheaterMode() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_TheaterMode(&result);
	return result;
}

BOOL CHtmlView::GetAddressBar() const
{
	ASSERT(m_pBrowserApp != NULL);

	VARIANT_BOOL result;
	m_pBrowserApp->get_AddressBar(&result);
	return result;
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlView operations

BOOL CHtmlView::GetSource(CString& refString)
{
	BOOL bRetVal = FALSE;
	LPDISPATCH pDisp = GetHtmlDocument();

	if (pDisp != NULL)
	{
		HGLOBAL hMemory;
		hMemory = GlobalAlloc(GMEM_MOVEABLE, 0);
		if (hMemory != NULL)
		{
			LPPERSISTSTREAMINIT pPersistStream = NULL;
			pDisp->QueryInterface(IID_IPersistStreamInit,
				(LPVOID*) &pPersistStream);

			if (pPersistStream != NULL)
			{
				LPSTREAM pStream;
				if (SUCCEEDED(CreateStreamOnHGlobal(hMemory, TRUE, &pStream)))
				{
					pPersistStream->Save(pStream, FALSE);

					LPCTSTR pstr = (LPCTSTR) GlobalLock(hMemory);
					if (pstr != NULL)
					{
						// Stream is always ANSI, but CString
						// assignment operator will convert implicitly.

						refString = pstr;
						GlobalUnlock(hMemory);
						bRetVal = TRUE;
					}
					pStream->Release();
				}
				pPersistStream->Release();
			}
		}
	}

	return bRetVal;
}

BOOL CHtmlView::LoadFromResource(LPCTSTR lpszResource)
{
	HINSTANCE hInstance = AfxGetResourceHandle();
	ASSERT(hInstance != NULL);

	CString strResourceURL;
	BOOL bRetVal = TRUE;
	LPTSTR lpszModule = new TCHAR[_MAX_PATH];

	if (GetModuleFileName(hInstance, lpszModule, _MAX_PATH))
	{
		strResourceURL.Format(_T("res://%s/%s"), lpszModule, lpszResource);
		Navigate(strResourceURL, 0, 0, 0);
	}
	else
		bRetVal = FALSE;

	delete [] lpszModule;
	return bRetVal;
}

BOOL CHtmlView::LoadFromResource(UINT nRes)
{
	HINSTANCE hInstance = AfxGetResourceHandle();
	ASSERT(hInstance != NULL);

	CString strResourceURL;
	BOOL bRetVal = TRUE;
	LPTSTR lpszModule = new TCHAR[_MAX_PATH];

	if (GetModuleFileName(hInstance, lpszModule, _MAX_PATH))
	{
		strResourceURL.Format(_T("res://%s/%d"), lpszModule, nRes);
		Navigate(strResourceURL, 0, 0, 0);
	}
	else
		bRetVal = FALSE;

	delete [] lpszModule;
	return bRetVal;
}

void CHtmlView::Navigate(LPCTSTR lpszURL, DWORD dwFlags /* = 0 */,
	LPCTSTR lpszTargetFrameName /* = NULL */ ,
	LPCTSTR lpszHeaders /* = NULL */, LPVOID lpvPostData /* = NULL */,
	DWORD dwPostDataLen /* = 0 */)
{
	USES_CONVERSION;
	CString strURL(lpszURL);
	CComBSTR bstrURL;
	bstrURL.Attach(strURL.AllocSysString());

	COleSafeArray vPostData;
	if (lpvPostData != NULL)
	{
		if (dwPostDataLen == 0)
			dwPostDataLen = lstrlen((LPCTSTR) lpvPostData);

		vPostData.CreateOneDim(VT_UI1, dwPostDataLen, lpvPostData);
	}

	m_pBrowserApp->Navigate(bstrURL,
		COleVariant((long) dwFlags, VT_I4),
		COleVariant(lpszTargetFrameName, VT_BSTR),
		vPostData,
		COleVariant(lpszHeaders, VT_BSTR));
}

void CHtmlView::Navigate2(LPITEMIDLIST pIDL, DWORD dwFlags /* = 0 */,
	LPCTSTR lpszTargetFrameName /* = NULL */)
{
	ASSERT(m_pBrowserApp != NULL);

	COleVariant vPIDL(pIDL);
	COleVariant empty;

	m_pBrowserApp->Navigate2(vPIDL,
		COleVariant((long) dwFlags, VT_I4),
		COleVariant(lpszTargetFrameName, VT_BSTR),
		empty, empty);
}

void CHtmlView::Navigate2(LPCTSTR lpszURL, DWORD dwFlags /* = 0 */,
	LPCTSTR lpszTargetFrameName /* = NULL */,
	LPCTSTR lpszHeaders /* = NULL */,
	LPVOID lpvPostData /* = NULL */, DWORD dwPostDataLen /* = 0 */)
{
	ASSERT(m_pBrowserApp != NULL);

	COleSafeArray vPostData;
	if (lpvPostData != NULL)
	{
		if (dwPostDataLen == 0)
			dwPostDataLen = lstrlen((LPCTSTR) lpvPostData);

		vPostData.CreateOneDim(VT_UI1, dwPostDataLen, lpvPostData);
	}

	COleVariant vURL(lpszURL, VT_BSTR);
	COleVariant vHeaders(lpszHeaders, VT_BSTR);
	COleVariant vTargetFrameName(lpszTargetFrameName, VT_BSTR);
	COleVariant vFlags((long) dwFlags, VT_I4);

	m_pBrowserApp->Navigate2(vURL,
		vFlags, vTargetFrameName, vPostData, vHeaders);
}

void CHtmlView::Navigate2(LPCTSTR lpszURL, DWORD dwFlags,
	CByteArray& baPostData, LPCTSTR lpszTargetFrameName /* = NULL */,
	LPCTSTR lpszHeaders /* = NULL */)
{
	ASSERT(m_pBrowserApp != NULL);

	COleVariant vPostData = baPostData;
	COleVariant vURL(lpszURL, VT_BSTR);
	COleVariant vHeaders(lpszHeaders, VT_BSTR);
	COleVariant vTargetFrameName(lpszTargetFrameName, VT_BSTR);
	COleVariant vFlags((long) dwFlags, VT_I4);

	ASSERT(m_pBrowserApp != NULL);

	m_pBrowserApp->Navigate2(vURL, vFlags, vTargetFrameName,
		vPostData, vHeaders);
}

void CHtmlView::PutProperty(LPCTSTR lpszProperty, const VARIANT& vtValue)
{
	ASSERT(m_pBrowserApp != NULL);

	CString strProp(lpszProperty);
	BSTR bstrProp = strProp.AllocSysString();
	m_pBrowserApp->PutProperty(bstrProp, vtValue);
	::SysFreeString(bstrProp);
}

BOOL CHtmlView::GetProperty(LPCTSTR lpszProperty, CString& strValue)
{
	ASSERT(m_pBrowserApp != NULL);

	CString strProperty(lpszProperty);
	BSTR bstrProperty = strProperty.AllocSysString();

	BOOL bResult = FALSE;
	VARIANT vReturn;
	vReturn.vt = VT_BSTR;
	vReturn.bstrVal = NULL;
	HRESULT hr = m_pBrowserApp->GetProperty(bstrProperty, &vReturn);

	if (SUCCEEDED(hr))
	{
		strValue = CString(vReturn.bstrVal);
		bResult = TRUE;
	}

	::SysFreeString(bstrProperty);
	return bResult;
}

COleVariant CHtmlView::GetProperty(LPCTSTR lpszProperty)
{
	COleVariant result;

	static BYTE parms[] =
		VTS_BSTR;
	m_wndBrowser.InvokeHelper(0x12f, DISPATCH_METHOD,
		VT_VARIANT, (void*)&result, parms, lpszProperty);

	return result;
}

CString CHtmlView::GetFullName() const
{
	ASSERT(m_pBrowserApp != NULL);

	CComBSTR bstr;
	m_pBrowserApp->get_FullName(&bstr);
	CString retVal(bstr);
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlView event reflectors

void CHtmlView::NavigateComplete2(LPDISPATCH /* pDisp */, VARIANT* URL)
{
	ASSERT(V_VT(URL) == VT_BSTR);

	USES_CONVERSION;

	CString str = OLE2T(V_BSTR(URL));
	OnNavigateComplete2(str);
}

void CHtmlView::BeforeNavigate2(LPDISPATCH /* pDisp */, VARIANT* URL,
		VARIANT* Flags, VARIANT* TargetFrameName,
		VARIANT* PostData, VARIANT* Headers, VARIANT_BOOL* Cancel) 
{
	ASSERT(V_VT(URL) == VT_BSTR);
	ASSERT(V_VT(TargetFrameName) == VT_BSTR);
	ASSERT(V_VT(PostData) == (VT_VARIANT | VT_BYREF));
	ASSERT(V_VT(Headers) == VT_BSTR);
	ASSERT(Cancel != NULL);

	USES_CONVERSION;

	VARIANT* vtPostedData = V_VARIANTREF(PostData);
	CByteArray array;
	if (V_VT(vtPostedData) & VT_ARRAY)
	{
		// must be a vector of bytes
		ASSERT(vtPostedData->parray->cDims == 1 && vtPostedData->parray->cbElements == 1);

		vtPostedData->vt |= VT_UI1;
		COleSafeArray safe(vtPostedData);

		DWORD dwSize = safe.GetOneDimSize();
		LPVOID pVoid;
		safe.AccessData(&pVoid);

		array.SetSize(dwSize);
		LPBYTE lpByte = array.GetData();

		memcpy(lpByte, pVoid, dwSize);
		safe.UnaccessData();
	}
	// make real parameters out of the notification

	CString strTargetFrameName(V_BSTR(TargetFrameName));
	CString strURL = CString(V_BSTR(URL));
	CString strHeaders = CString(V_BSTR(Headers));
	DWORD nFlags = V_I4(Flags);


	BOOL bCancel = FALSE;
	// notify the user's class
	OnBeforeNavigate2(strURL, nFlags, strTargetFrameName,
		array, strHeaders, &bCancel);

	if (bCancel)
		*Cancel = AFX_OLE_TRUE;
	else
		*Cancel = AFX_OLE_FALSE;
}

void CHtmlView::DocumentComplete(LPDISPATCH pDisp, VARIANT* URL)
{
	UNUSED_ALWAYS(pDisp);
	ASSERT(V_VT(URL) == VT_BSTR);

	CString str(V_BSTR(URL));
	OnDocumentComplete(str);
}

void CHtmlView::OnEditCopy() 
{
	ExecFormsCommand(IDM_COPY, NULL, NULL);
	return;
}

void CHtmlView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	BOOL bEnabled = FALSE;

	// Since input variables aren't touched for failure case of
	// QueryCommand(), we can ignore return value.
	QueryFormsCommand(IDM_COPY, NULL, &bEnabled, NULL);
	pCmdUI->Enable(bEnabled);
}

void CHtmlView::OnEditCut() 
{
	ExecFormsCommand(IDM_CUT, NULL, NULL);
	return;
}

void CHtmlView::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	BOOL bEnabled = FALSE;

	// Since input variables aren't touched for failure case of
	// QueryCommand(), we can ignore return value.
	QueryFormsCommand(IDM_CUT, NULL, &bEnabled, NULL);
	pCmdUI->Enable(bEnabled);
}

void CHtmlView::OnEditPaste() 
{
	ExecFormsCommand(IDM_PASTE, NULL, NULL);
	return;
}

void CHtmlView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	BOOL bEnabled = FALSE;

	// Since input variables aren't touched for failure case of
	// QueryCommand(), we can ignore return value.
	QueryFormsCommand(IDM_PASTE, NULL, &bEnabled, NULL);
	pCmdUI->Enable(bEnabled);
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlView Events

void CHtmlView::OnProgressChange(long lProgress, long lProgressMax)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(lProgress);
	UNUSED_ALWAYS(lProgressMax);
}

void CHtmlView::OnCommandStateChange(long lCommand, BOOL bEnable)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(lCommand);
	UNUSED_ALWAYS(bEnable);
}

void CHtmlView::OnDownloadBegin()
{
	// user will override to handle this notification
}

void CHtmlView::OnDownloadComplete()
{
	// user will override to handle this notification
}

void CHtmlView::OnTitleChange(LPCTSTR lpszText)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(lpszText);
}

void CHtmlView::OnPropertyChange(LPCTSTR lpszProperty)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(lpszProperty);
}

void CHtmlView::OnNewWindow2(LPDISPATCH* ppDisp, BOOL* bCancel)
{
	// default to continuing
	bCancel = FALSE;

	// user will override to handle this notification
	UNUSED_ALWAYS(ppDisp);
}

void CHtmlView::OnDocumentComplete(LPCTSTR lpszURL)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(lpszURL);
}

void CHtmlView::OnQuit()
{
	// user will override to handle this notification
}

void CHtmlView::OnVisible(BOOL bVisible)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(bVisible);
}

void CHtmlView::OnToolBar(BOOL bToolBar)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(bToolBar);
}

void CHtmlView::OnMenuBar(BOOL bMenuBar)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(bMenuBar);
}

void CHtmlView::OnStatusBar(BOOL bStatusBar)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(bStatusBar);
}

void CHtmlView::OnFullScreen(BOOL bFullScreen)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(bFullScreen);
}

void CHtmlView::OnTheaterMode(BOOL bTheaterMode)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(bTheaterMode);
}

void CHtmlView::OnNavigateComplete2(LPCTSTR lpszURL)
{
	// user will override to handle this notification
	UNUSED_ALWAYS(lpszURL);
}

void CHtmlView::OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags,
	LPCTSTR lpszTargetFrameName, CByteArray& baPostData,
	LPCTSTR lpszHeaders, BOOL* bCancel)
{
	// default to continuing
	bCancel = FALSE;

	// user will override to handle this notification
	UNUSED_ALWAYS(lpszURL);
	UNUSED_ALWAYS(nFlags);
	UNUSED_ALWAYS(lpszTargetFrameName);
	UNUSED_ALWAYS(baPostData);
	UNUSED_ALWAYS(lpszHeaders);
}

void CHtmlView::OnStatusTextChange(LPCTSTR pszText)
{
	// try to set the status bar text via the frame

	CFrameWnd* pFrame = GetParentFrame();
	if (pFrame != NULL)
		pFrame->SetMessageText(pszText);
}

extern "C" void _AfxHtmlEditCtrlFakeEntry()
{
	ASSERT( FALSE );
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlEditCtrl
CHtmlEditCtrl::CHtmlEditCtrl()
{
}

CHtmlEditCtrl::~CHtmlEditCtrl()
{
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlEditCtrl message handlers

BOOL CHtmlEditCtrl::Create(LPCTSTR lpszWindowName, DWORD /*dwStyle*/, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* /*pContext*/) 
{
	// create the control window
	CLSID CLSID_TriEditDocument = CLSID_NULL;
	::CLSIDFromProgID(OLESTR("DHTMLEdit.DHTMLEdit"),&CLSID_TriEditDocument);

	if (CLSID_TriEditDocument==CLSID_NULL ||
		!CreateControl(CLSID_TriEditDocument, lpszWindowName,
				WS_VISIBLE | WS_CHILD, rect, pParentWnd, nID))
	{
		DestroyWindow();
		return FALSE;
	}

	return TRUE;

}

/////////////////////////////////////////////////////////////////////////////
// CHtmlEditView

BEGIN_MESSAGE_MAP(CHtmlEditView, CFormView)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, OnFilePrint)
	ON_COMMAND(ID_EDIT_COPY, OnCopy)
	ON_COMMAND(ID_EDIT_CUT, OnCut)
	ON_COMMAND(ID_EDIT_PASTE, OnPaste)
	ON_COMMAND(ID_EDIT_UNDO, OnUndo)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateCopyUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateCutUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdatePasteUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateUndoUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateSelectAllUI)
END_MESSAGE_MAP()


BEGIN_EVENTSINK_MAP(CHtmlEditView, CFormView)
	ON_EVENT(CHtmlEditView, AFX_IDW_PANE_FIRST, 0x00000001 /* DocumentComplete */, OnDocumentComplete, VTS_NONE)
	ON_EVENT(CHtmlEditView, AFX_IDW_PANE_FIRST, 0x00000002 /* DisplayChanged */, OnDisplayChanged, VTS_NONE)
	ON_EVENT(CHtmlEditView, AFX_IDW_PANE_FIRST, 0x00000003 /* ShowContextMenu */, OnShowContextMenu, VTS_I4 VTS_I4)
	ON_EVENT(CHtmlEditView, AFX_IDW_PANE_FIRST, 0x00000004 /* ContextMenuAction */, OnContextMenuAction, VTS_I4)
	ON_EVENT(CHtmlEditView, AFX_IDW_PANE_FIRST, 0x00000005 /* OnMouseDown */, OnMouseDown, VTS_NONE)
	ON_EVENT(CHtmlEditView, AFX_IDW_PANE_FIRST, 0x00000006 /* OnMouseMove */, OnMouseMove, VTS_NONE)
	ON_EVENT(CHtmlEditView, AFX_IDW_PANE_FIRST, 0x00000007 /* OnMouseUp */, OnMouseUp, VTS_NONE)
	ON_EVENT(CHtmlEditView, AFX_IDW_PANE_FIRST, 0x00000008 /* OnMouseOut */, OnMouseOut, VTS_NONE)
	ON_EVENT(CHtmlEditView, AFX_IDW_PANE_FIRST, 0x00000009 /* OnMouseOver */, OnMouseOver, VTS_NONE)
	ON_EVENT(CHtmlEditView, AFX_IDW_PANE_FIRST, 0x0000000a /* OnClick */, OnClick, VTS_NONE)
	ON_EVENT(CHtmlEditView, AFX_IDW_PANE_FIRST, 0x0000000b /* OnDblClick */, OnDblClick, VTS_NONE)
	ON_EVENT(CHtmlEditView, AFX_IDW_PANE_FIRST, 0x0000000c /* OnKeyDown */, OnKeyDown, VTS_NONE)
	ON_EVENT(CHtmlEditView, AFX_IDW_PANE_FIRST, 0x0000000d /* OnKeyPress */, OnKeyPress, VTS_NONE)
	ON_EVENT(CHtmlEditView, AFX_IDW_PANE_FIRST, 0x0000000e /* OnKeyUp */, OnKeyUp, VTS_NONE)
	ON_EVENT(CHtmlEditView, AFX_IDW_PANE_FIRST, 0x0000000f /* OnBlur */, OnBlur, VTS_NONE)
	ON_EVENT(CHtmlEditView, AFX_IDW_PANE_FIRST, 0x00000010 /* OnReadyStateChange */, OnReadyStateChange, VTS_NONE)
END_EVENTSINK_MAP()
/////////////////////////////////////////////////////////////////////////////
// CHtmlEditView construction/destruction

CHtmlEditView::CHtmlEditView()
	: CFormView((LPCTSTR) NULL)
{
}

CHtmlEditView::~CHtmlEditView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlEditView drawing

void CHtmlEditView::OnDraw(CDC* /* pDC*/)
{
	//should never need to draw
	ASSERT(FALSE);
}

BOOL CHtmlEditView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
	DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID,
	CCreateContext* pContext) 
{
	// create the view window itself
	m_pCreateContext = pContext;
	if (!CView::Create(lpszClassName, lpszWindowName,
				dwStyle, rect, pParentWnd,	nID, pContext))
	{
		return FALSE;
	}

	AfxEnableControlContainer();
	
	RECT rectClient;
	GetClientRect(&rectClient);

	// create the control window
	// AFX_IDW_PANE_FIRST is a safe but arbitrary ID
	return m_wndTriEdit.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(0,0,0,0),
			this, AFX_IDW_PANE_FIRST);
}

void CHtmlEditView::OnPaint() 
{
	Default();
}

void CHtmlEditView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	if (::IsWindow(m_wndTriEdit.m_hWnd))
	{
		// need to push non-client borders out of the client area
		CRect rect;
		GetClientRect(rect);
		::AdjustWindowRectEx(rect, 
			m_wndTriEdit.GetStyle(), FALSE, WS_EX_CLIENTEDGE);
		m_wndTriEdit.SetWindowPos(NULL, rect.left, rect.top,
			rect.Width(), rect.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
	}	
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlEditView event handling

void CHtmlEditView::OnDocumentComplete()
{
	// user can override 
}

void CHtmlEditView::OnDisplayChanged()
{
	// user can override 
}

void CHtmlEditView::OnShowContextMenu(long xPos, long yPos)
{
	// user can override
	UNUSED_ALWAYS(xPos);
	UNUSED_ALWAYS(yPos);
}

void CHtmlEditView::OnContextMenuAction(long itemIndex)
{
	// user can override
	UNUSED_ALWAYS(itemIndex);
}

void CHtmlEditView::OnMouseDown()
{
	// user can override 
}

void CHtmlEditView::OnMouseMove()
{
	// user can override 
}

void CHtmlEditView::OnMouseUp()
{
	// user can override 
}

void CHtmlEditView::OnMouseOut()
{
	// user can override 
}

void CHtmlEditView::OnMouseOver()
{
	// user can override 
}

void CHtmlEditView::OnClick()
{
	// user can override 
}

void CHtmlEditView::OnDblClick()
{
	// user can override 
}

void CHtmlEditView::OnKeyDown()
{
	// user can override 
}

void CHtmlEditView::OnKeyPress()
{
	// user can override 
}

void CHtmlEditView::OnKeyUp()
{
	// user can override 
}

void CHtmlEditView::OnBlur()
{
	// user can override 
}

void CHtmlEditView::OnReadyStateChange()
{
	CComPtr<IHTMLDocument2> spDOM;

	//dhtmled.ocx will set focus incorrectly if we don't do this
	m_wndTriEdit.GetDocument(&spDOM);
	if (spDOM)
	{
		CComBSTR bstrState;
		spDOM->get_readyState(&bstrState);
		if (bstrState && !wcscmp(bstrState, L"complete"))
			m_wndTriEdit.SetFocus();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlEditView command handlers

BOOL CHtmlEditView::OnPreparePrinting(CPrintInfo* /*pInfo*/)
{
	// default preparation
	return TRUE;
}

void CHtmlEditView::OnFilePrint()
{
	m_wndTriEdit.PrintDocument(TRUE);
}

void CHtmlEditView::OnCopy()
{
	Copy();
}

void CHtmlEditView::OnCut()
{
	Cut();
}

void CHtmlEditView::OnPaste()
{
	Paste();
}

void CHtmlEditView::OnUndo()
{
	Undo();
}

void CHtmlEditView::OnSelectAll()
{
	SelectAll();
}

void CHtmlEditView::OnUpdateCopyUI(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(DECMDF_ENABLED == QueryStatus(DECMD_COPY));
}

void CHtmlEditView::OnUpdateCutUI(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(DECMDF_ENABLED == QueryStatus(DECMD_CUT));
}

void CHtmlEditView::OnUpdatePasteUI(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(DECMDF_ENABLED == QueryStatus(DECMD_PASTE));
}

void CHtmlEditView::OnUpdateUndoUI(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(DECMDF_ENABLED == QueryStatus(DECMD_UNDO));
}

void CHtmlEditView::OnUpdateSelectAllUI(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(DECMDF_ENABLED == QueryStatus(DECMD_SELECTALL));
}


/////////////////////////////////////////////////////////////////////////////
// CHtmlEditView diagnostics

#ifdef _DEBUG
void CHtmlEditView::AssertValid() const
{
	CFormView::AssertValid();
}

void CHtmlEditView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

CHtmlEditDoc::CHtmlEditDoc()
{
}

CHtmlEditView* CHtmlEditDoc::GetView() const
{
	CHtmlEditView* pHtmlView = NULL;

	POSITION pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = GetNextView(pos);
		pHtmlView = DYNAMIC_DOWNCAST(CHtmlEditView, pView);
		if (pHtmlView != NULL)
			break;
	}

	return pHtmlView;
}

void CHtmlEditDoc::DeleteContents()
{
	CHtmlEditView* pView = GetView();

	if (pView)
	{
		pView->NewDocument();
	}
}

BOOL CHtmlEditDoc::IsModified()
{
	CHtmlEditView* pView = GetView();

	if (pView)
	{
		return pView->GetIsDirty();
	}
	
	return FALSE;
}

BOOL CHtmlEditDoc::OpenURL(LPCTSTR lpszURL)
{
	BOOL bRet = FALSE;
	CHtmlEditView* pView = GetView();

	if (pView != NULL && lpszURL != NULL && *lpszURL != '\0')
	{
		if(pView->LoadURL(lpszURL))
		{
			SetTitle(lpszURL);
			bRet = TRUE;
		}
	}

	return bRet;
}

BOOL CHtmlEditDoc::OnOpenDocument(LPCTSTR lpszFileName)
{
	BOOL bRet = FALSE;
	CHtmlEditView* pView = GetView();
	
	if (pView != NULL)
		bRet = pView->LoadDocument(lpszFileName);
	return bRet;
}

BOOL CHtmlEditDoc::OnSaveDocument(LPCTSTR lpszFileName)
{
	BOOL bRet = FALSE;
	CHtmlEditView *pView = GetView();
	
	if (pView != NULL)
		bRet = pView->SaveDocument(lpszFileName);
	return bRet;
}

BOOL CHtmlEditDoc::OnNewDocument()
{
	BOOL bRet = FALSE;

	if (CDocument::OnNewDocument())
	{
		CHtmlEditView* pView = GetView();
		if (pView)
			bRet = pView->NewDocument();
	}

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlEditDoc diagnostics

#ifdef _DEBUG
void CHtmlEditDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CHtmlEditDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif _DEBUG

/////////////////////////////////////////////////////////////////////////////
// Inline function declarations expanded out-of-line

#ifndef _AFX_ENABLE_INLINES

// expand inlines for Html functions
static char _szAfxHtmlInl[] = "afxhtml.inl";
#undef THIS_FILE
#define THIS_FILE _szAfxHtmlInl
#define _AFXHTML_INLINE
#include "afxhtml.inl"

#endif //!_AFX_ENABLE_INLINES

/////////////////////////////////////////////////////////////////////////////
// Pre-startup code

#ifdef AFX_INIT_SEG
#pragma code_seg(AFX_INIT_SEG)
#endif

IMPLEMENT_DYNCREATE(CHtmlView, CFormView)
IMPLEMENT_DYNCREATE(CHtmlEditView, CFormView)
IMPLEMENT_DYNAMIC(CHtmlEditDoc, CDocument)
