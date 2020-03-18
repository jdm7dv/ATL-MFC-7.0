// This is a part of the Active Template Library.
// Copyright (C) 1996-2000 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the	
// Active Template Library product.


#ifndef __ATLDEBUGAPI_H__
#define __ATLDEBUGAPI_H__

#pragma once

#ifdef __cplusplus
namespace ATL
{
extern "C" {
#endif
#define MAX_NAME_SIZE 64

typedef enum ATLTRACESTATUS
{
	ATLTRACESTATUS_INHERIT, ATLTRACESTATUS_ENABLED, ATLTRACESTATUS_DISABLED
} ATLTRACESTATUS;

BOOL __stdcall AtlTraceOpen(DWORD dwId);
void __stdcall AtlTraceClose(void);

BOOL __stdcall AtlTraceRegister(HINSTANCE hInst,
	int (__cdecl *fnCrtDbgReport)(int,const char *,int,const char *,const char *,...));
BOOL __stdcall AtlTraceUnregister(HINSTANCE hInst);

BOOL __stdcall AtlTraceRegisterCategoryA(HINSTANCE hInst, UINT nCategory, const CHAR szCategoryName[MAX_NAME_SIZE]);
BOOL __stdcall AtlTraceRegisterCategoryU(HINSTANCE hInst, UINT nCategory, const WCHAR szCategoryName[MAX_NAME_SIZE]);

BOOL __stdcall AtlTraceModifyProcess(UINT nLevel, BOOL bEnabled, BOOL bFuncAndCategoryNames, BOOL bFileNameAndLineNo);
BOOL __stdcall AtlTraceModifyModule(HINSTANCE hInst, UINT nLevel, ATLTRACESTATUS eStatus);
BOOL __stdcall AtlTraceModifyCategory(HINSTANCE hInst, UINT nCategory, UINT nLevel, ATLTRACESTATUS eStatus);
BOOL __stdcall AtlTraceGetProcess(UINT *pnLevel, BOOL *pbEnabled, BOOL *pbFuncAndCategoryNames, BOOL *pbFileNameAndLineNo);
BOOL __stdcall AtlTraceGetModule(HINSTANCE hInst, UINT *pnLevel, ATLTRACESTATUS *pStatus);
BOOL __stdcall AtlTraceGetCategory(HINSTANCE hInst, UINT nCategory, UINT *pnLevel, ATLTRACESTATUS *pStatus);

void __stdcall AtlTraceGetUpdateEventNameA(CHAR *pszEventName);
void __stdcall AtlTraceGetUpdateEventNameU(WCHAR *pszEventName);

/*void __cdecl AtlTraceA(HINSTANCE hInst, UINT nCategory, UINT nLevel, const CHAR *pszFormat, ...);
void __cdecl AtlTraceU(HINSTANCE hInst, UINT nCategory, UINT nLevel, const WCHAR *pszFormat, ...);*/

void __cdecl AtlTraceVA(HINSTANCE hInst, const char *pszFileName, int nLineNo,
						UINT nCategory, UINT nLevel, const CHAR *pszFormat, va_list ptr);
void __cdecl AtlTraceVU(HINSTANCE hInst,const char *pszFileName, int nLineNo,
						UINT nCategory, UINT nLevel, const WCHAR *pszFormat, va_list ptr);

BOOL __stdcall AtlTraceLoadSettingsA(const CHAR *pszFileName, BOOL bForceLoad);
BOOL __stdcall AtlTraceLoadSettingsU(const WCHAR *pszFileName, BOOL bForceLoad);
BOOL __stdcall AtlTraceSaveSettingsA(const CHAR *pszFileName);
BOOL __stdcall AtlTraceSaveSettingsU(const WCHAR *pszFileName);

typedef struct ATLTRACESETTINGS
{
	UINT nLevel;
	ATLTRACESTATUS eStatus;
} ATLTRACESETTINGS;

typedef struct ATLTRACEPROCESSSETTINGS
{
	UINT nLevel;
	BOOL bEnabled, bFuncAndCategoryNames, bFileNameAndLineNo;
} ATLTRACEPROCESSSETTINGS;

typedef struct ATLTRACEPROCESSINFOA
{
	CHAR szName[MAX_NAME_SIZE], szPath[MAX_PATH];
	DWORD dwId;
	ATLTRACEPROCESSSETTINGS Settings;
} ATLTRACEPROCESSINFOA;

typedef struct ATLTRACEPROCESSINFOU
{
	WCHAR szName[MAX_NAME_SIZE], szPath[MAX_PATH];
	DWORD dwId;
	ATLTRACEPROCESSSETTINGS Settings;
} ATLTRACEPROCESSINFOU;

typedef struct ATLTRACEMODULEINFOA
{
	CHAR szName[MAX_NAME_SIZE], szPath[MAX_PATH];
	HINSTANCE hInst;
	ATLTRACESETTINGS Settings;
} ATLTRACEMODULEINFOA;

typedef struct ATLTRACEMODULEINFOU
{
	WCHAR szName[MAX_NAME_SIZE], szPath[MAX_PATH];
	HINSTANCE hInst;
	ATLTRACESETTINGS Settings;
} ATLTRACEMODULEINFOU;

typedef struct ATLTRACECATEGORYINFOA
{
	CHAR szName[MAX_NAME_SIZE];
	HINSTANCE hInst;
	UINT nCategory;
	ATLTRACESETTINGS Settings;
} ATLTRACECATEGORYINFOA;

typedef struct ATLTRACECATEGORYINFOU
{
	WCHAR szName[MAX_NAME_SIZE];
	HINSTANCE hInst;
	UINT nCategory;
	ATLTRACESETTINGS Settings;
} ATLTRACECATEGORYINFOU;

BOOL __stdcall AtlTraceGetProcessInfoA(ATLTRACEPROCESSINFOA *pProcessInfo);
BOOL __stdcall AtlTraceGetProcessInfoU(ATLTRACEPROCESSINFOU *pProcessInfo);
int __stdcall AtlTraceGetModuleInfoA(ATLTRACEMODULEINFOA *pModuleInfo);
int __stdcall AtlTraceGetModuleInfoU(ATLTRACEMODULEINFOU *pModuleInfo);
int __stdcall AtlTraceGetCategoryInfoA(HINSTANCE hInst, ATLTRACECATEGORYINFOA *pAtlTraceCategoryInfo);
int __stdcall AtlTraceGetCategoryInfoU(HINSTANCE hInst, ATLTRACECATEGORYINFOU *pAtlTraceCategoryInfo);

#ifdef UNICODE
#define AtlTraceRegisterCategory AtlTraceRegisterCategoryU
#define AtlTraceGetUpdateEventName AtlTraceGetUpdateEventNameU
#define AtlTrace AtlTraceU
#define AtlTraceV AtlTraceVU
#define AtlTraceLoadSettings AtlTraceLoadSettingsU
#define AtlTraceSaveSettings AtlTraceSaveSettingsU

#define ATLTRACEPROCESSINFO ATLTRACEPROCESSINFOU
#define ATLTRACEMODULEINFO ATLTRACEMODULEINFOU
#define ATLTRACECATEGORYINFO ATLTRACECATEGORYINFOU
#define AtlTraceGetProcessInfo AtlTraceGetProcessInfoU
#define AtlTraceGetModuleInfo AtlTraceGetModuleInfoU
#define AtlTraceGetCategoryInfo AtlTraceGetCategoryInfoU
#else
#define AtlTraceRegisterCategory AtlTraceRegisterCategoryA
#define AtlTraceGetUpdateEventName AtlTraceGetUpdateEventNameA
#define AtlTrace AtlTraceA
#define AtlTraceV AtlTraceVA
#define AtlTraceLoadSettings AtlTraceLoadSettingsA
#define AtlTraceSaveSettings AtlTraceSaveSettingsA

#define ATLTRACEPROCESSINFO ATLTRACEPROCESSINFOA
#define ATLTRACEMODULEINFO ATLTRACEMODULEINFOA
#define ATLTRACECATEGORYINFO ATLTRACECATEGORYINFOA
#define AtlTraceGetProcessInfo AtlTraceGetProcessInfoA
#define AtlTraceGetModuleInof AtlTraceGetModuleInfoA
#define AtlTraceGetCategoryInfo AtlTraceGetCategoryInfoA
#endif

#ifdef __cplusplus
};

};  // namespace ATL
#endif

#endif  // __ATLDEBUGAPI_H__
