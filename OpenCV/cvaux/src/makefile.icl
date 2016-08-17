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

OUTBIN = ..\..\bin/cvaux096$(SUFFIX).dll
OUTLIB = ..\..\lib\cvaux$(SUFFIX).lib

OBJS = ..\..\_temp\cvaux$(DR)_icl\camshift.obj \
..\..\_temp\cvaux$(DR)_icl\cv3dtracker.obj ..\..\_temp\cvaux$(DR)_icl\cvaux.obj \
..\..\_temp\cvaux$(DR)_icl\cvauxutils.obj \
..\..\_temp\cvaux$(DR)_icl\cvcalibfilter.obj \
..\..\_temp\cvaux$(DR)_icl\cvcorrespond.obj \
..\..\_temp\cvaux$(DR)_icl\cvcorrimages.obj \
..\..\_temp\cvaux$(DR)_icl\cvcreatehandmask.obj \
..\..\_temp\cvaux$(DR)_icl\cvdpstereo.obj \
..\..\_temp\cvaux$(DR)_icl\cveigenobjects.obj \
..\..\_temp\cvaux$(DR)_icl\cvepilines.obj ..\..\_temp\cvaux$(DR)_icl\cvface.obj \
..\..\_temp\cvaux$(DR)_icl\cvfacedetection.obj \
..\..\_temp\cvaux$(DR)_icl\cvfacetemplate.obj \
..\..\_temp\cvaux$(DR)_icl\cvfindface.obj \
..\..\_temp\cvaux$(DR)_icl\cvfindhandregion.obj ..\..\_temp\cvaux$(DR)_icl\cvhmm.obj \
..\..\_temp\cvaux$(DR)_icl\cvhmm1d.obj ..\..\_temp\cvaux$(DR)_icl\cvhmmobs.obj \
..\..\_temp\cvaux$(DR)_icl\cvlcm.obj ..\..\_temp\cvaux$(DR)_icl\cvlee.obj \
..\..\_temp\cvaux$(DR)_icl\cvlevmar.obj \
..\..\_temp\cvaux$(DR)_icl\cvlevmarprojbandle.obj \
..\..\_temp\cvaux$(DR)_icl\cvlevmartrif.obj ..\..\_temp\cvaux$(DR)_icl\cvlines.obj \
..\..\_temp\cvaux$(DR)_icl\cvlmeds.obj ..\..\_temp\cvaux$(DR)_icl\cvmorphcontours.obj \
..\..\_temp\cvaux$(DR)_icl\cvmorphing.obj ..\..\_temp\cvaux$(DR)_icl\cvprewarp.obj \
..\..\_temp\cvaux$(DR)_icl\cvscanlines.obj ..\..\_temp\cvaux$(DR)_icl\cvsegment.obj \
..\..\_temp\cvaux$(DR)_icl\cvsubdiv2.obj ..\..\_temp\cvaux$(DR)_icl\cvtexture.obj \
..\..\_temp\cvaux$(DR)_icl\cvtrifocal.obj \
..\..\_temp\cvaux$(DR)_icl\cvvecfacetracking.obj \
..\..\_temp\cvaux$(DR)_icl\cvvideo.obj ..\..\_temp\cvaux$(DR)_icl\precomp.obj 



INC = ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h \
../include/cvaux.h ../include/cvaux.hpp 


CXXFLAGS2 = /I"../../cv/include" /I"../include" /I"../../cxcore/include" /I"../../cv/src" /I"." /nologo /GX /G6 /W4 "/Qwd68,171,424,444,869,981,522,9" /c /Fo 
LINKFLAGS2 = /libpath:"../../lib" /nologo /subsystem:windows /dll /pdb:none /machine:I386 /out:$(OUTBIN) /implib:$(OUTLIB) /nodefaultlib:libm /nodefaultlib:libirc 

!ifdef debug

CXXFLAGS = /D"CVAPI_EXPORTS" /D"WIN32" /D"_WINDOWS" /D"_DEBUG" /MDd /Gm /Zi /Od /FD /GZ $(CXXFLAGS2)
LIBS = cvd.lib cxcored.lib kernel32.lib user32.lib gdi32.lib 
LINKFLAGS = $(LINKFLAGS2) /debug

!else

CXXFLAGS = /D"CVAPI_EXPORTS" /D"NDEBUG" /D"WIN32" /D"_WINDOWS" /MD /O3 /Ob2 $(CXXFLAGS2)
LIBS = cv.lib cxcore.lib kernel32.lib user32.lib gdi32.lib 
LINKFLAGS = $(LINKFLAGS2) 

!endif


$(OUTBIN): $(OBJS)
	-mkdir ..\..\bin 2> nul
	-mkdir ..\..\lib 2> nul
	$(LINK) $(LINKFLAGS) $** $(LIBS) 
	

all: $(OUTBIN)

..\..\_temp\cvaux$(DR)_icl\camshift.obj: .\camshift.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	@-mkdir ..\..\_temp\cvaux$(DR)_icl 2>nul
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\camshift.obj .\camshift.cpp
..\..\_temp\cvaux$(DR)_icl\cv3dtracker.obj: .\cv3dtracker.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cv3dtracker.obj .\cv3dtracker.cpp
..\..\_temp\cvaux$(DR)_icl\cvaux.obj: .\cvaux.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvaux.obj .\cvaux.cpp
..\..\_temp\cvaux$(DR)_icl\cvauxutils.obj: .\cvauxutils.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvauxutils.obj .\cvauxutils.cpp
..\..\_temp\cvaux$(DR)_icl\cvcalibfilter.obj: .\cvcalibfilter.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvcalibfilter.obj .\cvcalibfilter.cpp
..\..\_temp\cvaux$(DR)_icl\cvcorrespond.obj: .\cvcorrespond.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvcorrespond.obj .\cvcorrespond.cpp
..\..\_temp\cvaux$(DR)_icl\cvcorrimages.obj: .\cvcorrimages.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvcorrimages.obj .\cvcorrimages.cpp
..\..\_temp\cvaux$(DR)_icl\cvcreatehandmask.obj: .\cvcreatehandmask.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvcreatehandmask.obj .\cvcreatehandmask.cpp
..\..\_temp\cvaux$(DR)_icl\cvdpstereo.obj: .\cvdpstereo.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvdpstereo.obj .\cvdpstereo.cpp
..\..\_temp\cvaux$(DR)_icl\cveigenobjects.obj: .\cveigenobjects.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cveigenobjects.obj .\cveigenobjects.cpp
..\..\_temp\cvaux$(DR)_icl\cvepilines.obj: .\cvepilines.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvepilines.obj .\cvepilines.cpp
..\..\_temp\cvaux$(DR)_icl\cvface.obj: .\cvface.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvface.obj .\cvface.cpp
..\..\_temp\cvaux$(DR)_icl\cvfacedetection.obj: .\cvfacedetection.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvfacedetection.obj .\cvfacedetection.cpp
..\..\_temp\cvaux$(DR)_icl\cvfacetemplate.obj: .\cvfacetemplate.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvfacetemplate.obj .\cvfacetemplate.cpp
..\..\_temp\cvaux$(DR)_icl\cvfindface.obj: .\cvfindface.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvfindface.obj .\cvfindface.cpp
..\..\_temp\cvaux$(DR)_icl\cvfindhandregion.obj: .\cvfindhandregion.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvfindhandregion.obj .\cvfindhandregion.cpp
..\..\_temp\cvaux$(DR)_icl\cvhmm.obj: .\cvhmm.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvhmm.obj .\cvhmm.cpp
..\..\_temp\cvaux$(DR)_icl\cvhmm1d.obj: .\cvhmm1d.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvhmm1d.obj .\cvhmm1d.cpp
..\..\_temp\cvaux$(DR)_icl\cvhmmobs.obj: .\cvhmmobs.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvhmmobs.obj .\cvhmmobs.cpp
..\..\_temp\cvaux$(DR)_icl\cvlcm.obj: .\cvlcm.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvlcm.obj .\cvlcm.cpp
..\..\_temp\cvaux$(DR)_icl\cvlee.obj: .\cvlee.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvlee.obj .\cvlee.cpp
..\..\_temp\cvaux$(DR)_icl\cvlevmar.obj: .\cvlevmar.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvlevmar.obj .\cvlevmar.cpp
..\..\_temp\cvaux$(DR)_icl\cvlevmarprojbandle.obj: .\cvlevmarprojbandle.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvlevmarprojbandle.obj .\cvlevmarprojbandle.cpp
..\..\_temp\cvaux$(DR)_icl\cvlevmartrif.obj: .\cvlevmartrif.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvlevmartrif.obj .\cvlevmartrif.cpp
..\..\_temp\cvaux$(DR)_icl\cvlines.obj: .\cvlines.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvlines.obj .\cvlines.cpp
..\..\_temp\cvaux$(DR)_icl\cvlmeds.obj: .\cvlmeds.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvlmeds.obj .\cvlmeds.cpp
..\..\_temp\cvaux$(DR)_icl\cvmorphcontours.obj: .\cvmorphcontours.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvmorphcontours.obj .\cvmorphcontours.cpp
..\..\_temp\cvaux$(DR)_icl\cvmorphing.obj: .\cvmorphing.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvmorphing.obj .\cvmorphing.cpp
..\..\_temp\cvaux$(DR)_icl\cvprewarp.obj: .\cvprewarp.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvprewarp.obj .\cvprewarp.cpp
..\..\_temp\cvaux$(DR)_icl\cvscanlines.obj: .\cvscanlines.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvscanlines.obj .\cvscanlines.cpp
..\..\_temp\cvaux$(DR)_icl\cvsegment.obj: .\cvsegment.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvsegment.obj .\cvsegment.cpp
..\..\_temp\cvaux$(DR)_icl\cvsubdiv2.obj: .\cvsubdiv2.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvsubdiv2.obj .\cvsubdiv2.cpp
..\..\_temp\cvaux$(DR)_icl\cvtexture.obj: .\cvtexture.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvtexture.obj .\cvtexture.cpp
..\..\_temp\cvaux$(DR)_icl\cvtrifocal.obj: .\cvtrifocal.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvtrifocal.obj .\cvtrifocal.cpp
..\..\_temp\cvaux$(DR)_icl\cvvecfacetracking.obj: .\cvvecfacetracking.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvvecfacetracking.obj .\cvvecfacetracking.cpp
..\..\_temp\cvaux$(DR)_icl\cvvideo.obj: .\cvvideo.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\cvvideo.obj .\cvvideo.cpp
..\..\_temp\cvaux$(DR)_icl\precomp.obj: .\precomp.cpp ./_cvaux.h ./_cvfacedetection.h ./_cvvectrack.h ./_cvvm.h ./resource.h ../include/cvaux.h ../include/cvaux.hpp
	-$(CXX) $(CXXFLAGS)..\..\_temp\cvaux$(DR)_icl\precomp.obj .\precomp.cpp
