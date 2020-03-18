# This is a part of the Active Template Library.
# Copyright (C) 1996-2000 Microsoft Corporation
# All rights reserved.
#
# This source code is only intended as a supplement to the
# Active Template Library Reference and related
# electronic documentation provided with the library.
# See these sources for detailed information regarding the
# Active Template Library product.

!if "$(LIBNAME)" != ""
_LIBNAME = LIBNAME=$(LIBNAME)
!endif

!if "$(PLATFORM)" == ""
!message setting PLATFORM=INTEL
PLATFORM=INTEL
!endif

!if "$(CLEAN)" != ""
_CLEAN = CLEAN
!else
_CLEAN =
!endif

!if !exist(atlmfc.mak)
!error Run this makefile from the directory it is loacted in.
!endif


all : createdir ATLSRC MFCSRC

atl : createdir ATLSRC

mfc : createdir MFCSRC

createdir :
# create or clean destination directory
!if "$(CLEAN)" == ""
	if not exist ..\lib\$(PLATFORM) md ..\lib\$(PLATFORM)
!endif

ATLSRC :
	cd atl

# build	atl70dbg.lib
	cd atldebug
	$(MAKE) /$(MAKEFLAGS) /f atldebug.mak DEBUG=1 $(_CLEAN)
	cd ..
!if "$(CLEAN)" == ""
	copy atldebug\$(PLATFORM)\debug\atl70dbg.lib ..\..\lib\$(PLATFORM)
	copy atldebug\$(PLATFORM)\debug\atl70dbg.pdb ..\..\lib\$(PLATFORM)
!endif	
	
# build the atl static libraries
	cd atls
	$(MAKE) /$(MAKEFLAGS) /f atls.mak DEBUG=1 $(_CLEAN)
	$(MAKE) /$(MAKEFLAGS) /f atls.mak DEBUG=0 $(_CLEAN)
	cd ..
!if "$(CLEAN)" == ""
	copy atls\$(PLATFORM)\debug\atlsd.lib ..\..\lib\$(PLATFORM)
	copy atls\$(PLATFORM)\debug\atlsd.pdb ..\..\lib\$(PLATFORM)
	copy atls\$(PLATFORM)\release\atls.lib ..\..\lib\$(PLATFORM)
	copy atls\$(PLATFORM)\release\atls.pdb ..\..\lib\$(PLATFORM)
!endif
	
# build mincrt static lib	
	cd atlmincrt
	$(MAKE) /$(MAKEFLAGS) /f atlmincrt.mak DEBUG=0 $(_CLEAN)
	cd ..
!if "$(CLEAN)" == ""
	copy atlmincrt\$(PLATFORM)\release\atlmincrt.lib ..\..\lib\$(PLATFORM)
!endif

# build alt70.dll	
	$(MAKE) /$(MAKEFLAGS) /f atlidl.mak DEBUG=0 $(_CLEAN)
	$(MAKE) /$(MAKEFLAGS) /f atlidl.mak $(_CLEAN)
	$(MAKE) /$(MAKEFLAGS) /f atldll.mak UNICODE=1 DEBUG=0 $(_CLEAN)
	$(MAKE) /$(MAKEFLAGS) /f atldll.mak DEBUG=0 $(_CLEAN)
	$(MAKE) /$(MAKEFLAGS) /f atldll.mak UNICODE=1 DEBUG=1 $(_CLEAN)
	$(MAKE) /$(MAKEFLAGS) /f atldll.mak DEBUG=1 $(_CLEAN)
!if "$(CLEAN)" == ""
	copy $(PLATFORM)\releaseU\atl.lib ..\..\lib\$(PLATFORM)	
!endif

	cd ..
	
MFCSRC : 
	cd mfc
# static libs	
	$(MAKE) /$(MAKEFLAGS) DEBUG=0 $(_CLEAN)
	$(MAKE) /$(MAKEFLAGS) DEBUG=1 $(_CLEAN)
	$(MAKE) /$(MAKEFLAGS) DEBUG=0 UNICODE=1 $(_CLEAN)
	$(MAKE) /$(MAKEFLAGS) DEBUG=1 UNICODE=1 $(_CLEAN)
	
# isapi libs
	$(MAKE) /$(MAKEFLAGS) /f mfcisapi.mak DEBUG=0 $(_CLEAN)	
	$(MAKE) /$(MAKEFLAGS) /f mfcisapi.mak DEBUG=1 $(_CLEAN)
	
# dlls
	$(MAKE) /$(MAKEFLAGS) /f mfcdll.mak $(_LIBNAME) debug=0 $(_CLEAN)
	$(MAKE) /$(MAKEFLAGS) /f mfcdll.mak $(_LIBNAME) debug=1 $(_CLEAN)
	$(MAKE) /$(MAKEFLAGS) /f mfcdll.mak $(_LIBNAME) debug=0 UNICODE=1 $(_CLEAN)
	$(MAKE) /$(MAKEFLAGS) /f mfcdll.mak $(_LIBNAME) debug=1 UNICODE=1 $(_CLEAN)

# localized dlls
	$(MAKE) /$(MAKEFLAGS) /f mfcintl.mak fra
	$(MAKE) /$(MAKEFLAGS) /f mfcintl.mak deu
	$(MAKE) /$(MAKEFLAGS) /f mfcintl.mak jpn
	$(MAKE) /$(MAKEFLAGS) /f mfcintl.mak ita
	$(MAKE) /$(MAKEFLAGS) /f mfcintl.mak esp
	$(MAKE) /$(MAKEFLAGS) /f mfcintl.mak chs
	$(MAKE) /$(MAKEFLAGS) /f mfcintl.mak cht
	$(MAKE) /$(MAKEFLAGS) /f mfcintl.mak kor
	$(MAKE) /$(MAKEFLAGS) /f mfcintl.mak enu
	cd ..