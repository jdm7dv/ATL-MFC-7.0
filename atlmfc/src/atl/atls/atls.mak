# This is a part of the Active Template Library.
# Copyright (C) 1996-2000 Microsoft Corporation
# All rights reserved.
#
# This source code is only intended as a supplement to the
# Active Template Library Reference and related
# electronic documentation provided with the library.
# See these sources for detailed information regarding the
# Active Template Library product.

!ifndef DEBUG
DEBUG=1
!endif

!if "$(DEBUG)" == "1"
TARGETNAME=atlsd
!else
TARGETNAME=atls
!endif
TARGETTYPE=LIB

!include ..\atlcommon.mak

OBJS=$(D)\atlbase.obj $(D)\atlcommodule.obj $(D)\atlwinmodule.obj $(D)\atlstr.obj $(D)\atlmem.obj $(D)\atlimage.obj $(D)\atldebuginterfacesmodule.obj \
	$(D)\Allocate.obj $(D)\AtlDebugAPI.obj $(D)\AtlTraceModuleManager.obj $(D)\Externs.obj $(D)\LoadNSave.obj

!if "$(PLATFORM)" == "IA64"
OBJS=$(OBJS) $(D)\stdcallthunk.obj $(D)\QIThunk.obj
!endif

!include ..\atltarg.mak
