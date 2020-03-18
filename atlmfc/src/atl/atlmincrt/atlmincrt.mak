# This is a part of the Active Template Library.
# Copyright (C) 1996-2000 Microsoft Corporation
# All rights reserved.
#
# This source code is only intended as a supplement to the
# Active Template Library Reference and related
# electronic documentation provided with the library.
# See these sources for detailed information regarding the
# Active Template Library product.

TARGETNAME=atlmincrt
TARGETTYPE=LIB

!include ..\atlcommon.mak

OBJS=$(D)\atlinit.obj $(D)\atlinitdll.obj $(D)\atlinitexe.obj $(D)\atlinitexeu.obj

!include ..\atltarg.mak
