// This is a part of the Active Template Library.
// Copyright (C) 1996-2000 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the	
// Active Template Library product.

#include "stdafx.h"
#include "Common.h"
#include "Allocate.h"

#pragma warning(disable : 4074)
#pragma init_seg(compiler)

const char *g_pszUpdateEventName	= "AtlTraceModuleManager_ProcessAddedStatic";
const char *g_pszAllocFileMapName	= "AtlDebugAllocator_FileMappingNameStatic";
const char *g_pszAllocMutexName		= "AtlDebugAllocator_MutexNameStatic";

const char *g_pszKernelObjFmt = "%s_%lu";

CAtlAllocator g_Allocator;

static bool Init()
{
	const int nSize = 64;
	char szFileMappingName[nSize], szMutexName[nSize];

	sprintf(szFileMappingName, g_pszKernelObjFmt,
		g_pszAllocFileMapName, GetCurrentProcessId());
	sprintf(szMutexName, g_pszKernelObjFmt,
		g_pszAllocMutexName, GetCurrentProcessId());

	// REVIEW: surely four megs is enough?
	return g_Allocator.Init(szFileMappingName, szMutexName, 4 * 1024 * 1024);
}

static const bool g_bInitialized = Init();

