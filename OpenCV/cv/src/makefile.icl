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

OUTBIN = ..\..\bin\cv096$(SUFFIX).dll
OUTLIB = ..\..\lib\cv$(SUFFIX).lib

OBJS = ..\..\_temp\cv$(DR)_icl\cvadapthresh.obj \
..\..\_temp\cv$(DR)_icl\cvcalccontrasthistogram.obj \
..\..\_temp\cv$(DR)_icl\cvcanny.obj ..\..\_temp\cv$(DR)_icl\cvcolor.obj \
..\..\_temp\cv$(DR)_icl\cvcontours.obj ..\..\_temp\cv$(DR)_icl\cvconvolve.obj \
..\..\_temp\cv$(DR)_icl\cvcorner.obj ..\..\_temp\cv$(DR)_icl\cvcornersubpix.obj \
..\..\_temp\cv$(DR)_icl\cvderiv.obj ..\..\_temp\cv$(DR)_icl\cvfeatureselect.obj \
..\..\_temp\cv$(DR)_icl\cvfloodfill.obj ..\..\_temp\cv$(DR)_icl\cvhistogram.obj \
..\..\_temp\cv$(DR)_icl\cvimgwarp.obj ..\..\_temp\cv$(DR)_icl\cvmineval.obj \
..\..\_temp\cv$(DR)_icl\cvmoments.obj ..\..\_temp\cv$(DR)_icl\cvmorph.obj \
..\..\_temp\cv$(DR)_icl\cvprecorner.obj ..\..\_temp\cv$(DR)_icl\cvpyramids.obj \
..\..\_temp\cv$(DR)_icl\cvsamplers.obj ..\..\_temp\cv$(DR)_icl\cvsmooth.obj \
..\..\_temp\cv$(DR)_icl\cvtemplmatch.obj ..\..\_temp\cv$(DR)_icl\cvthresh.obj \
..\..\_temp\cv$(DR)_icl\cvapprox.obj ..\..\_temp\cv$(DR)_icl\cvcontourtree.obj \
..\..\_temp\cv$(DR)_icl\cvconvhull.obj ..\..\_temp\cv$(DR)_icl\cvdistransform.obj \
..\..\_temp\cv$(DR)_icl\cvdominants.obj ..\..\_temp\cv$(DR)_icl\cvemd.obj \
..\..\_temp\cv$(DR)_icl\cvgeometry.obj ..\..\_temp\cv$(DR)_icl\cvhough.obj \
..\..\_temp\cv$(DR)_icl\cvlinefit.obj ..\..\_temp\cv$(DR)_icl\cvmatchcontours.obj \
..\..\_temp\cv$(DR)_icl\cvpgh.obj ..\..\_temp\cv$(DR)_icl\cvpyrsegmentation.obj \
..\..\_temp\cv$(DR)_icl\cvrotcalipers.obj ..\..\_temp\cv$(DR)_icl\cvshapedescr.obj \
..\..\_temp\cv$(DR)_icl\cvsubdivision2d.obj ..\..\_temp\cv$(DR)_icl\cvcamshift.obj \
..\..\_temp\cv$(DR)_icl\cvcondens.obj ..\..\_temp\cv$(DR)_icl\cvkalman.obj \
..\..\_temp\cv$(DR)_icl\cvlkpyramid.obj ..\..\_temp\cv$(DR)_icl\cvmotempl.obj \
..\..\_temp\cv$(DR)_icl\cvoptflowbm.obj ..\..\_temp\cv$(DR)_icl\cvoptflowhs.obj \
..\..\_temp\cv$(DR)_icl\cvoptflowlk.obj ..\..\_temp\cv$(DR)_icl\cvsnakes.obj \
..\..\_temp\cv$(DR)_icl\cvcalcimagehomography.obj \
..\..\_temp\cv$(DR)_icl\cvcalibinit.obj ..\..\_temp\cv$(DR)_icl\cvcalibration.obj \
..\..\_temp\cv$(DR)_icl\cvfundam.obj ..\..\_temp\cv$(DR)_icl\cvposit.obj \
..\..\_temp\cv$(DR)_icl\cvundistort.obj ..\..\_temp\cv$(DR)_icl\cvhaar.obj \
..\..\_temp\cv$(DR)_icl\cvaccum.obj ..\..\_temp\cv$(DR)_icl\cvsumpixels.obj \
..\..\_temp\cv$(DR)_icl\cvswitcher.obj ..\..\_temp\cv$(DR)_icl\cvtables.obj \
..\..\_temp\cv$(DR)_icl\cvutils.obj ..\..\_temp\cv$(DR)_icl\cvprecomp.obj 



INC = ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h \
./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h 


CXXFLAGS2 = /I"../include" /I"../../cxcore/include" /I"." /nologo /GX /G6 /W4 "/Qwd68,171,424,444,869,981,522,9" /c /Fo 
LINKFLAGS2 = /libpath:"../../lib" /nologo /subsystem:windows /dll /pdb:none /machine:I386 /out:$(OUTBIN) /implib:$(OUTLIB) /nodefaultlib:libm /nodefaultlib:libirc 

!ifdef debug

CXXFLAGS = /D"WIN32" /D"CVAPI_EXPORTS" /D"_WINDOWS" /D"_DEBUG" /MDd /Gm /Zi /Od /FD /GZ $(CXXFLAGS2)
LIBS = kernel32.lib user32.lib gdi32.lib cxcored.lib 
LINKFLAGS = $(LINKFLAGS2) /debug

!else

CXXFLAGS = /D"WIN32" /D"NDEBUG" /D"CVAPI_EXPORTS" /D"_WINDOWS" /MD /O3 /Ob2 $(CXXFLAGS2)
LIBS = kernel32.lib user32.lib gdi32.lib cxcore.lib 
LINKFLAGS = $(LINKFLAGS2) 

!endif


$(OUTBIN): $(OBJS)
	-mkdir ..\..\bin 2> nul
	-mkdir ..\..\lib 2> nul
	$(LINK) $(LINKFLAGS) $** $(LIBS) 
	

all: $(OUTBIN)

..\..\_temp\cv$(DR)_icl\cvadapthresh.obj: .\cvadapthresh.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	@-mkdir ..\..\_temp\cv$(DR)_icl 2>nul
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvadapthresh.obj .\cvadapthresh.cpp
..\..\_temp\cv$(DR)_icl\cvcalccontrasthistogram.obj: .\cvcalccontrasthistogram.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvcalccontrasthistogram.obj .\cvcalccontrasthistogram.cpp
..\..\_temp\cv$(DR)_icl\cvcanny.obj: .\cvcanny.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvcanny.obj .\cvcanny.cpp
..\..\_temp\cv$(DR)_icl\cvcolor.obj: .\cvcolor.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvcolor.obj .\cvcolor.cpp
..\..\_temp\cv$(DR)_icl\cvcontours.obj: .\cvcontours.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvcontours.obj .\cvcontours.cpp
..\..\_temp\cv$(DR)_icl\cvconvolve.obj: .\cvconvolve.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvconvolve.obj .\cvconvolve.cpp
..\..\_temp\cv$(DR)_icl\cvcorner.obj: .\cvcorner.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvcorner.obj .\cvcorner.cpp
..\..\_temp\cv$(DR)_icl\cvcornersubpix.obj: .\cvcornersubpix.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvcornersubpix.obj .\cvcornersubpix.cpp
..\..\_temp\cv$(DR)_icl\cvderiv.obj: .\cvderiv.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvderiv.obj .\cvderiv.cpp
..\..\_temp\cv$(DR)_icl\cvfeatureselect.obj: .\cvfeatureselect.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvfeatureselect.obj .\cvfeatureselect.cpp
..\..\_temp\cv$(DR)_icl\cvfloodfill.obj: .\cvfloodfill.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvfloodfill.obj .\cvfloodfill.cpp
..\..\_temp\cv$(DR)_icl\cvhistogram.obj: .\cvhistogram.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvhistogram.obj .\cvhistogram.cpp
..\..\_temp\cv$(DR)_icl\cvimgwarp.obj: .\cvimgwarp.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvimgwarp.obj .\cvimgwarp.cpp
..\..\_temp\cv$(DR)_icl\cvmineval.obj: .\cvmineval.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvmineval.obj .\cvmineval.cpp
..\..\_temp\cv$(DR)_icl\cvmoments.obj: .\cvmoments.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvmoments.obj .\cvmoments.cpp
..\..\_temp\cv$(DR)_icl\cvmorph.obj: .\cvmorph.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvmorph.obj .\cvmorph.cpp
..\..\_temp\cv$(DR)_icl\cvprecorner.obj: .\cvprecorner.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvprecorner.obj .\cvprecorner.cpp
..\..\_temp\cv$(DR)_icl\cvpyramids.obj: .\cvpyramids.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvpyramids.obj .\cvpyramids.cpp
..\..\_temp\cv$(DR)_icl\cvsamplers.obj: .\cvsamplers.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvsamplers.obj .\cvsamplers.cpp
..\..\_temp\cv$(DR)_icl\cvsmooth.obj: .\cvsmooth.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvsmooth.obj .\cvsmooth.cpp
..\..\_temp\cv$(DR)_icl\cvtemplmatch.obj: .\cvtemplmatch.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvtemplmatch.obj .\cvtemplmatch.cpp
..\..\_temp\cv$(DR)_icl\cvthresh.obj: .\cvthresh.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvthresh.obj .\cvthresh.cpp
..\..\_temp\cv$(DR)_icl\cvapprox.obj: .\cvapprox.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvapprox.obj .\cvapprox.cpp
..\..\_temp\cv$(DR)_icl\cvcontourtree.obj: .\cvcontourtree.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvcontourtree.obj .\cvcontourtree.cpp
..\..\_temp\cv$(DR)_icl\cvconvhull.obj: .\cvconvhull.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvconvhull.obj .\cvconvhull.cpp
..\..\_temp\cv$(DR)_icl\cvdistransform.obj: .\cvdistransform.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvdistransform.obj .\cvdistransform.cpp
..\..\_temp\cv$(DR)_icl\cvdominants.obj: .\cvdominants.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvdominants.obj .\cvdominants.cpp
..\..\_temp\cv$(DR)_icl\cvemd.obj: .\cvemd.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvemd.obj .\cvemd.cpp
..\..\_temp\cv$(DR)_icl\cvgeometry.obj: .\cvgeometry.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvgeometry.obj .\cvgeometry.cpp
..\..\_temp\cv$(DR)_icl\cvhough.obj: .\cvhough.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvhough.obj .\cvhough.cpp
..\..\_temp\cv$(DR)_icl\cvlinefit.obj: .\cvlinefit.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvlinefit.obj .\cvlinefit.cpp
..\..\_temp\cv$(DR)_icl\cvmatchcontours.obj: .\cvmatchcontours.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvmatchcontours.obj .\cvmatchcontours.cpp
..\..\_temp\cv$(DR)_icl\cvpgh.obj: .\cvpgh.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvpgh.obj .\cvpgh.cpp
..\..\_temp\cv$(DR)_icl\cvpyrsegmentation.obj: .\cvpyrsegmentation.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvpyrsegmentation.obj .\cvpyrsegmentation.cpp
..\..\_temp\cv$(DR)_icl\cvrotcalipers.obj: .\cvrotcalipers.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvrotcalipers.obj .\cvrotcalipers.cpp
..\..\_temp\cv$(DR)_icl\cvshapedescr.obj: .\cvshapedescr.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvshapedescr.obj .\cvshapedescr.cpp
..\..\_temp\cv$(DR)_icl\cvsubdivision2d.obj: .\cvsubdivision2d.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvsubdivision2d.obj .\cvsubdivision2d.cpp
..\..\_temp\cv$(DR)_icl\cvcamshift.obj: .\cvcamshift.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvcamshift.obj .\cvcamshift.cpp
..\..\_temp\cv$(DR)_icl\cvcondens.obj: .\cvcondens.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvcondens.obj .\cvcondens.cpp
..\..\_temp\cv$(DR)_icl\cvkalman.obj: .\cvkalman.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvkalman.obj .\cvkalman.cpp
..\..\_temp\cv$(DR)_icl\cvlkpyramid.obj: .\cvlkpyramid.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvlkpyramid.obj .\cvlkpyramid.cpp
..\..\_temp\cv$(DR)_icl\cvmotempl.obj: .\cvmotempl.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvmotempl.obj .\cvmotempl.cpp
..\..\_temp\cv$(DR)_icl\cvoptflowbm.obj: .\cvoptflowbm.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvoptflowbm.obj .\cvoptflowbm.cpp
..\..\_temp\cv$(DR)_icl\cvoptflowhs.obj: .\cvoptflowhs.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvoptflowhs.obj .\cvoptflowhs.cpp
..\..\_temp\cv$(DR)_icl\cvoptflowlk.obj: .\cvoptflowlk.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvoptflowlk.obj .\cvoptflowlk.cpp
..\..\_temp\cv$(DR)_icl\cvsnakes.obj: .\cvsnakes.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvsnakes.obj .\cvsnakes.cpp
..\..\_temp\cv$(DR)_icl\cvcalcimagehomography.obj: .\cvcalcimagehomography.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvcalcimagehomography.obj .\cvcalcimagehomography.cpp
..\..\_temp\cv$(DR)_icl\cvcalibinit.obj: .\cvcalibinit.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvcalibinit.obj .\cvcalibinit.cpp
..\..\_temp\cv$(DR)_icl\cvcalibration.obj: .\cvcalibration.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvcalibration.obj .\cvcalibration.cpp
..\..\_temp\cv$(DR)_icl\cvfundam.obj: .\cvfundam.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvfundam.obj .\cvfundam.cpp
..\..\_temp\cv$(DR)_icl\cvposit.obj: .\cvposit.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvposit.obj .\cvposit.cpp
..\..\_temp\cv$(DR)_icl\cvundistort.obj: .\cvundistort.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvundistort.obj .\cvundistort.cpp
..\..\_temp\cv$(DR)_icl\cvhaar.obj: .\cvhaar.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvhaar.obj .\cvhaar.cpp
..\..\_temp\cv$(DR)_icl\cvaccum.obj: .\cvaccum.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvaccum.obj .\cvaccum.cpp
..\..\_temp\cv$(DR)_icl\cvsumpixels.obj: .\cvsumpixels.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvsumpixels.obj .\cvsumpixels.cpp
..\..\_temp\cv$(DR)_icl\cvswitcher.obj: .\cvswitcher.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvswitcher.obj .\cvswitcher.cpp
..\..\_temp\cv$(DR)_icl\cvtables.obj: .\cvtables.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvtables.obj .\cvtables.cpp
..\..\_temp\cv$(DR)_icl\cvutils.obj: .\cvutils.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvutils.obj .\cvutils.cpp
..\..\_temp\cv$(DR)_icl\cvprecomp.obj: .\cvprecomp.cpp ../include/cv.h ../include/cv.hpp ../include/cvcompat.h ../include/cvtypes.h ./_cv.h ./_cvgeom.h ./_cvimgproc.h ./_cvipp.h ./_cvlist.h ./_cvmatrix.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\cv$(DR)_icl\cvprecomp.obj .\cvprecomp.cpp
