// This is a part of the Active Template Library.
// Copyright (C) 1996-2000 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the	
// Active Template Library product.

// atl.cpp : Implementation of DLL Exports.

#include "stdafx.h"
#include "resource.h"
#include "RegObj.h"

#define _ATLBASE_IMPL
#include <atlbase.h>
#define _ATLCOM_IMPL
#include <atlcom.h>
#define _ATLWIN_IMPL
#define _ATLHOST_IMPL
#include <atlwin.h>
#define _ATLCTL_IMPL
#include <atlctl.h>
#define _ATLCONV_IMPL
#include <atlconv.h>
#include <atlhost.h>

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_Registrar, CDLLRegObject)
	OBJECT_ENTRY_NON_CREATEABLE(CAxHostWindow)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		bAtlAxWinInitialized = false;
		OSVERSIONINFOA info;
		info.dwOSVersionInfoSize = sizeof(info);
		if (GetVersionExA(&info))
		{
#ifdef _UNICODE
			if (info.dwPlatformId != VER_PLATFORM_WIN32_NT)
			{
				MessageBoxA(NULL, "Can not run Unicode version of ATL70.DLL on Windows 95.\nPlease install the correct version.", "ATL", MB_ICONSTOP|MB_OK);
				return FALSE;
			}
#else
			if (info.dwPlatformId == VER_PLATFORM_WIN32_NT)
			{
				OutputDebugString(_T("Running Ansi version of ATL70.DLL on Windows NT : Slight Performace loss.\nPlease install the UNICODE version on NT.\n"));
			}
#endif
		}
#ifdef _DEBUG
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF);
		int n = 0;
		_CrtSetBreakAlloc(n);
#endif
		_Module.Init(ObjectMap, hInstance, &LIBID_ATLLib);
#ifdef _ATL_DEBUG_INTERFACES
		int ni = 0;
		_Module.m_nIndexBreakAt = ni;
#endif // _ATL_DEBUG_INTERFACES
		DisableThreadLibraryCalls(hInstance);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
#ifdef _DEBUG
		::OutputDebugString(_T("ATL70.DLL exiting.\n"));
#endif
		_Module.Term();
		if (bAtlAxWinInitialized)
			AtlAxWinTerm();
#ifdef _DEBUG
		if (_CrtDumpMemoryLeaks())
			::MessageBeep(MB_ICONEXCLAMATION);
#endif
	}
	return TRUE;    // ok
}

STDAPI AtlCreateRegistrar(IRegistrar** ppReg)
{
	if (ppReg == NULL)
		return E_POINTER;
	*ppReg = NULL;

	return( CDLLRegObject::_CreatorClass::CreateInstance(NULL, IID_IRegistrar, (void**)ppReg) );
}

static const void* g_apForceFunctions[] =
{
	AtlCreateTargetDC,
	AtlGetObjectSourceInterface,
	AtlHiMetricToPixel,
	AtlIPersistPropertyBag_Load,
	AtlIPersistPropertyBag_Save,
	AtlIPersistStreamInit_Load,
	AtlIPersistStreamInit_Save,
	AtlPixelToHiMetric,
	AtlWinModuleRegisterClassExA,
	AtlWinModuleRegisterClassExW,
	AtlWinModuleRegisterWndClassInfoA,
	AtlWinModuleRegisterWndClassInfoW
};

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

/*
STDAPI DllCanUnloadNow(void)
{
	return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	// registers object, typelib and all interfaces in typelib
	return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	//No need to unregister typelib since ATL is a system component.
	return S_OK;
}
*/
