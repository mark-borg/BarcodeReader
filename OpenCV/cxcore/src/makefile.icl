# Makefile for Intel Proton Compiler >=5.0

CXX = icl
LINK = link

!ifdef debug

SUFFIX = d
DR = _Dbg

!else

SUFFIX = 
DR = _Rls

!endif

OUTBIN = ..\..\bin\cxcore096$(SUFFIX).dll
OUTLIB = ..\..\lib\cxcore$(SUFFIX).lib

OBJS = ..\..\_temp\cxcore$(DR)_icl\cxalloc.obj \
..\..\_temp\cxcore$(DR)_icl\cxarithm.obj ..\..\_temp\cxcore$(DR)_icl\cxarray.obj \
..\..\_temp\cxcore$(DR)_icl\cxcmp.obj ..\..\_temp\cxcore$(DR)_icl\cxconvert.obj \
..\..\_temp\cxcore$(DR)_icl\cxcopy.obj ..\..\_temp\cxcore$(DR)_icl\cxdatastructs.obj \
..\..\_temp\cxcore$(DR)_icl\cxdrawing.obj ..\..\_temp\cxcore$(DR)_icl\cxdxt.obj \
..\..\_temp\cxcore$(DR)_icl\cxerror.obj \
..\..\_temp\cxcore$(DR)_icl\cxjacobieigens.obj \
..\..\_temp\cxcore$(DR)_icl\cxlogic.obj ..\..\_temp\cxcore$(DR)_icl\cxmathfuncs.obj \
..\..\_temp\cxcore$(DR)_icl\cxmatmul.obj ..\..\_temp\cxcore$(DR)_icl\cxmatrix.obj \
..\..\_temp\cxcore$(DR)_icl\cxmean.obj ..\..\_temp\cxcore$(DR)_icl\cxmeansdv.obj \
..\..\_temp\cxcore$(DR)_icl\cxminmaxloc.obj ..\..\_temp\cxcore$(DR)_icl\cxnorm.obj \
..\..\_temp\cxcore$(DR)_icl\cxouttext.obj \
..\..\_temp\cxcore$(DR)_icl\cxpersistence.obj \
..\..\_temp\cxcore$(DR)_icl\cxprecomp.obj ..\..\_temp\cxcore$(DR)_icl\cxrand.obj \
..\..\_temp\cxcore$(DR)_icl\cxsumpixels.obj ..\..\_temp\cxcore$(DR)_icl\cxsvd.obj \
..\..\_temp\cxcore$(DR)_icl\cxswitcher.obj ..\..\_temp\cxcore$(DR)_icl\cxtables.obj \
..\..\_temp\cxcore$(DR)_icl\cxutils.obj 



INC = ../include/cxcore.h ../include/cxerror.h ../include/cxmisc.h \
../include/cxtypes.h ./_cxcore.h ./_cxipp.h 


CXXFLAGS2 = /I"../include" /I"." /nologo /GX /G6 /W4 "/Qwd68,171,424,444,869,981,522,9" /c /Fo 
LINKFLAGS2 = /nologo /subsystem:windows /dll /pdb:none /machine:I386 /out:$(OUTBIN) /implib:$(OUTLIB) /nodefaultlib:libm /nodefaultlib:libirc 

!ifdef debug

CXXFLAGS = /D"WIN32" /D"CVAPI_EXPORTS" /D"_WINDOWS" /D"_DEBUG" /MDd /Gm /Zi /Od /FD /GZ $(CXXFLAGS2)
LIBS = kernel32.lib user32.lib gdi32.lib advapi32.lib 
LINKFLAGS = $(LINKFLAGS2) /debug

!else

CXXFLAGS = /D"WIN32" /D"NDEBUG" /D"CVAPI_EXPORTS" /D"_WINDOWS" /MD /O3 /Ob2 $(CXXFLAGS2)
LIBS = kernel32.lib user32.lib gdi32.lib advapi32.lib 
LINKFLAGS = $(LINKFLAGS2) 

!endif


$(OUTBIN): $(OBJS)
	-mkdir ..\..\bin 2> nul
	-mkdir ..\..\lib 2> nul
	$(LINK) $(LINKFLAGS) $** $(LIBS) 
	

all: $(OUTBIN)

..\..\_temp\cxcore$(DR)_icl\cxalloc.obj: .\cxalloc.cpp ../include/cxcore.h ../include/cxerror.h ../include/cxmisc.h ../include/cxtypes.h ./_cxcore.h ./_cxipp.h
	@-mkdir ..\..\_temp\cxcore$(DR)_icl 2>nul
	-$(CXX) $(CXXFLAGS)..\..\_temp\cxcore$(DR)_icl\cxalloc.obj .\cxalloc.cpp
..\..\_temp\cxcore$(DR)_icl\cxarithm.obj: .\cxarithm.cpp ../include/cxcore.h ../include/cxerror.h ../include/cxmisc.h ../include/cxtypes.h ./_cxcore.h ./_cxipp.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cxcore$(DR)_icl\cxarithm.obj .\cxarithm.cpp
..\..\_temp\cxcore$(DR)_icl\cxarray.obj: .\cxarray.cpp ../include/cxcore.h ../include/cxerror.h ../include/cxmisc.h ../include/cxtypes.h ./_cxcore.h ./_cxipp.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cxcore$(DR)_icl\cxarray.obj .\cxarray.cpp
..\..\_temp\cxcore$(DR)_icl\cxcmp.obj: .\cxcmp.cpp ../include/cxcore.h ../include/cxerror.h ../include/cxmisc.h ../include/cxtypes.h ./_cxcore.h ./_cxipp.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cxcore$(DR)_icl\cxcmp.obj .\cxcmp.cpp
..\..\_temp\cxcore$(DR)_icl\cxconvert.obj: .\cxconvert.cpp ../include/cxcore.h ../include/cxerror.h ../include/cxmisc.h ../include/cxtypes.h ./_cxcore.h ./_cxipp.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cxcore$(DR)_icl\cxconvert.obj .\cxconvert.cpp
..\..\_temp\cxcore$(DR)_icl\cxcopy.obj: .\cxcopy.cpp ../include/cxcore.h ../include/cxerror.h ../include/cxmisc.h ../include/cxtypes.h ./_cxcore.h ./_cxipp.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cxcore$(DR)_icl\cxcopy.obj .\cxcopy.cpp
..\..\_temp\cxcore$(DR)_icl\cxdatastructs.obj: .\cxdatastructs.cpp ../include/cxcore.h ../include/cxerror.h ../include/cxmisc.h ../include/cxtypes.h ./_cxcore.h ./_cxipp.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cxcore$(DR)_icl\cxdatastructs.obj .\cxdatastructs.cpp
..\..\_temp\cxcore$(DR)_icl\cxdrawing.obj: .\cxdrawing.cpp ../include/cxcore.h ../include/cxerror.h ../include/cxmisc.h ../include/cxtypes.h ./_cxcore.h ./_cxipp.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cxcore$(DR)_icl\cxdrawing.obj .\cxdrawing.cpp
..\..\_temp\cxcore$(DR)_icl\cxdxt.obj: .\cxdxt.cpp ../include/cxcore.h ../include/cxerror.h ../include/cxmisc.h ../include/cxtypes.h ./_cxcore.h ./_cxipp.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cxcore$(DR)_icl\cxdxt.obj .\cxdxt.cpp
..\..\_temp\cxcore$(DR)_icl\cxerror.obj: .\cxerror.cpp ../include/cxcore.h ../include/cxerror.h ../include/cxmisc.h ../include/cxtypes.h ./_cxcore.h ./_cxipp.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cxcore$(DR)_icl\cxerror.obj .\cxerror.cpp
..\..\_temp\cxcore$(DR)_icl\cxjacobieigens.obj: .\cxjacobieigens.cpp ../include/cxcore.h ../include/cxerror.h ../include/cxmisc.h ../include/cxtypes.h ./_cxcore.h ./_cxipp.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cxcore$(DR)_icl\cxjacobieigens.obj .\cxjacobieigens.cpp
..\..\_temp\cxcore$(DR)_icl\cxlogic.obj: .\cxlogic.cpp ../include/cxcore.h ../include/cxerror.h ../include/cxmisc.h ../include/cxtypes.h ./_cxcore.h ./_cxipp.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cxcore$(DR)_icl\cxlogic.obj .\cxlogic.cpp
..\..\_temp\cxcore$(DR)_icl\cxmathfuncs.obj: .\cxmathfuncs.cpp ../include/cxcore.h ../include/cxerror.h ../include/cxmisc.h ../include/cxtypes.h ./_cxcore.h ./_cxipp.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cxcore$(DR)_icl\cxmathfuncs.obj .\cxmathfuncs.cpp
..\..\_temp\cxcore$(DR)_icl\cxmatmul.obj: .\cxmatmul.cpp ../include/cxcore.h ../include/cxerror.h ../include/cxmisc.h ../include/cxtypes.h ./_cxcore.h ./_cxipp.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cxcore$(DR)_icl\cxmatmul.obj .\cxmatmul.cpp
..\..\_temp\cxcore$(DR)_icl\cxmatrix.obj: .\cxmatrix.cpp ../include/cxcore.h ../include/cxerror.h ../include/cxmisc.h ../include/cxtypes.h ./_cxcore.h ./_cxipp.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cxcore$(DR)_icl\cxmatrix.obj .\cxmatrix.cpp
..\..\_temp\cxcore$(DR)_icl\cxmean.obj: .\cxmean.cpp ../include/cxcore.h ../include/cxerror.h ../include/cxmisc.h ../include/cxtypes.h ./_cxcore.h ./_cxipp.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cxcore$(DR)_icl\cxmean.obj .\cxmean.cpp
..\..\_temp\cxcore$(DR)_icl\cxmeansdv.obj: .\cxmeansdv.cpp ../include/cxcore.h ../include/cxerror.h ../include/cxmisc.h ../include/cxtypes.h ./_cxcore.h ./_cxipp.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cxcore$(DR)_icl\cxmeansdv.obj .\cxmeansdv.cpp
..\..\_temp\cxcore$(DR)_icl\cxminmaxloc.obj: .\cxminmaxloc.cpp ../include/cxcore.h ../include/cxerror.h ../include/cxmisc.h ../include/cxtypes.h ./_cxcore.h ./_cxipp.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cxcore$(DR)_icl\cxminmaxloc.obj .\cxminmaxloc.cpp
..\..\_temp\cxcore$(DR)_icl\cxnorm.obj: .\cxnorm.cpp ../include/cxcore.h ../include/cxerror.h ../include/cxmisc.h ../include/cxtypes.h ./_cxcore.h ./_cxipp.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cxcore$(DR)_icl\cxnorm.obj .\cxnorm.cpp
..\..\_temp\cxcore$(DR)_icl\cxouttext.obj: .\cxouttext.cpp ../include/cxcore.h ../include/cxerror.h ../include/cxmisc.h ../include/cxtypes.h ./_cxcore.h ./_cxipp.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cxcore$(DR)_icl\cxouttext.obj .\cxouttext.cpp
..\..\_temp\cxcore$(DR)_icl\cxpersistence.obj: .\cxpersistence.cpp ../include/cxcore.h ../include/cxerror.h ../include/cxmisc.h ../include/cxtypes.h ./_cxcore.h ./_cxipp.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cxcore$(DR)_icl\cxpersistence.obj .\cxpersistence.cpp
..\..\_temp\cxcore$(DR)_icl\cxprecomp.obj: .\cxprecomp.cpp ../include/cxcore.h ../include/cxerror.h ../include/cxmisc.h ../include/cxtypes.h ./_cxcore.h ./_cxipp.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cxcore$(DR)_icl\cxprecomp.obj .\cxprecomp.cpp
..\..\_temp\cxcore$(DR)_icl\cxrand.obj: .\cxrand.cpp ../include/cxcore.h ../include/cxerror.h ../include/cxmisc.h ../include/cxtypes.h ./_cxcore.h ./_cxipp.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cxcore$(DR)_icl\cxrand.obj .\cxrand.cpp
..\..\_temp\cxcore$(DR)_icl\cxsumpixels.obj: .\cxsumpixels.cpp ../include/cxcore.h ../include/cxerror.h ../include/cxmisc.h ../include/cxtypes.h ./_cxcore.h ./_cxipp.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cxcore$(DR)_icl\cxsumpixels.obj .\cxsumpixels.cpp
..\..\_temp\cxcore$(DR)_icl\cxsvd.obj: .\cxsvd.cpp ../include/cxcore.h ../include/cxerror.h ../include/cxmisc.h ../include/cxtypes.h ./_cxcore.h ./_cxipp.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cxcore$(DR)_icl\cxsvd.obj .\cxsvd.cpp
..\..\_temp\cxcore$(DR)_icl\cxswitcher.obj: .\cxswitcher.cpp ../include/cxcore.h ../include/cxerror.h ../include/cxmisc.h ../include/cxtypes.h ./_cxcore.h ./_cxipp.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cxcore$(DR)_icl\cxswitcher.obj .\cxswitcher.cpp
..\..\_temp\cxcore$(DR)_icl\cxtables.obj: .\cxtables.cpp ../include/cxcore.h ../include/cxerror.h ../include/cxmisc.h ../include/cxtypes.h ./_cxcore.h ./_cxipp.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cxcore$(DR)_icl\cxtables.obj .\cxtables.cpp
..\..\_temp\cxcore$(DR)_icl\cxutils.obj: .\cxutils.cpp ../include/cxcore.h ../include/cxerror.h ../include/cxmisc.h ../include/cxtypes.h ./_cxcore.h ./_cxipp.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cxcore$(DR)_icl\cxutils.obj .\cxutils.cpp
