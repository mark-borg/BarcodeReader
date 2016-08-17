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

OUTBIN = ..\..\bin/highgui096$(SUFFIX).dll
OUTLIB = ..\..\lib\highgui$(SUFFIX).lib

OBJS = ..\..\_temp\highgui$(DR)_icl\bitstrm.obj \
..\..\_temp\highgui$(DR)_icl\grfmt_base.obj \
..\..\_temp\highgui$(DR)_icl\grfmt_bmp.obj \
..\..\_temp\highgui$(DR)_icl\grfmt_jpeg.obj \
..\..\_temp\highgui$(DR)_icl\grfmt_png.obj ..\..\_temp\highgui$(DR)_icl\grfmt_pxm.obj \
..\..\_temp\highgui$(DR)_icl\grfmt_sunras.obj \
..\..\_temp\highgui$(DR)_icl\grfmt_tiff.obj ..\..\_temp\highgui$(DR)_icl\cvcap.obj \
..\..\_temp\highgui$(DR)_icl\image.obj ..\..\_temp\highgui$(DR)_icl\loadsave.obj \
..\..\_temp\highgui$(DR)_icl\precomp.obj ..\..\_temp\highgui$(DR)_icl\utils.obj \
..\..\_temp\highgui$(DR)_icl\window_lnx.obj \
..\..\_temp\highgui$(DR)_icl\window_w32.obj 



INC = ./bitstrm.h ./grfmt_base.h ./grfmt_bmp.h ./grfmt_jpeg.h ./grfmt_png.h \
./grfmt_pxm.h ./grfmt_sunras.h ./grfmt_tiff.h ./_highgui.h ./grfmts.h ./highgui.h \
./utils.h 


CXXFLAGS2 = /I"../_graphics/include" /I"." /I"../../cxcore/include" /I"../../cv/include" /I".//" /nologo /GX /G6 /W4 "/Qwd68,171,424,444,869,981,522,9" /c /Fo 
LINKFLAGS2 = /libpath:"../../lib" /libpath:"../_graphics/lib" /nologo /subsystem:windows /dll /pdb:none /machine:I386 /out:$(OUTBIN) /implib:$(OUTLIB) /nodefaultlib:libm /nodefaultlib:libirc 

!ifdef debug

CXXFLAGS = /D"HAVE_PNG" /D"_DEBUG" /D"_MBCS" /D"HAVE_JPEG" /D"WIN32" /D"HAVE_TIFF" /D"CVAPI_EXPORTS" /D"_WINDOWS" /MDd /Gm /Zi /Od /FD /GZ $(CXXFLAGS2)
LIBS = cxcored.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib comctl32.lib libjpeg.lib libpng.lib libtiff.lib zlib.lib vfw32.lib 
LINKFLAGS = $(LINKFLAGS2) /debug

!else

CXXFLAGS = /D"HAVE_PNG" /D"NDEBUG" /D"HAVE_JPEG" /D"WIN32" /D"HAVE_TIFF" /D"_MBCS" /D"CVAPI_EXPORTS" /D"_WINDOWS" /MD /O3 /Ob2 $(CXXFLAGS2)
LIBS = cxcore.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib comctl32.lib libjpeg.lib libpng.lib libtiff.lib zlib.lib vfw32.lib 
LINKFLAGS = $(LINKFLAGS2) 

!endif


$(OUTBIN): $(OBJS)
	-mkdir ..\..\bin 2> nul
	-mkdir ..\..\lib 2> nul
	$(LINK) $(LINKFLAGS) $** $(LIBS) 
	

all: $(OUTBIN)

..\..\_temp\highgui$(DR)_icl\bitstrm.obj: .\bitstrm.cpp ./bitstrm.h ./grfmt_base.h ./grfmt_bmp.h ./grfmt_jpeg.h ./grfmt_png.h ./grfmt_pxm.h ./grfmt_sunras.h ./grfmt_tiff.h ./_highgui.h ./grfmts.h ./highgui.h ./utils.h
	@-mkdir ..\..\_temp\highgui$(DR)_icl 2>nul
	-$(CXX) $(CXXFLAGS)..\..\_temp\highgui$(DR)_icl\bitstrm.obj .\bitstrm.cpp
..\..\_temp\highgui$(DR)_icl\grfmt_base.obj: .\grfmt_base.cpp ./bitstrm.h ./grfmt_base.h ./grfmt_bmp.h ./grfmt_jpeg.h ./grfmt_png.h ./grfmt_pxm.h ./grfmt_sunras.h ./grfmt_tiff.h ./_highgui.h ./grfmts.h ./highgui.h ./utils.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\highgui$(DR)_icl\grfmt_base.obj .\grfmt_base.cpp
..\..\_temp\highgui$(DR)_icl\grfmt_bmp.obj: .\grfmt_bmp.cpp ./bitstrm.h ./grfmt_base.h ./grfmt_bmp.h ./grfmt_jpeg.h ./grfmt_png.h ./grfmt_pxm.h ./grfmt_sunras.h ./grfmt_tiff.h ./_highgui.h ./grfmts.h ./highgui.h ./utils.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\highgui$(DR)_icl\grfmt_bmp.obj .\grfmt_bmp.cpp
..\..\_temp\highgui$(DR)_icl\grfmt_jpeg.obj: .\grfmt_jpeg.cpp ./bitstrm.h ./grfmt_base.h ./grfmt_bmp.h ./grfmt_jpeg.h ./grfmt_png.h ./grfmt_pxm.h ./grfmt_sunras.h ./grfmt_tiff.h ./_highgui.h ./grfmts.h ./highgui.h ./utils.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\highgui$(DR)_icl\grfmt_jpeg.obj .\grfmt_jpeg.cpp
..\..\_temp\highgui$(DR)_icl\grfmt_png.obj: .\grfmt_png.cpp ./bitstrm.h ./grfmt_base.h ./grfmt_bmp.h ./grfmt_jpeg.h ./grfmt_png.h ./grfmt_pxm.h ./grfmt_sunras.h ./grfmt_tiff.h ./_highgui.h ./grfmts.h ./highgui.h ./utils.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\highgui$(DR)_icl\grfmt_png.obj .\grfmt_png.cpp
..\..\_temp\highgui$(DR)_icl\grfmt_pxm.obj: .\grfmt_pxm.cpp ./bitstrm.h ./grfmt_base.h ./grfmt_bmp.h ./grfmt_jpeg.h ./grfmt_png.h ./grfmt_pxm.h ./grfmt_sunras.h ./grfmt_tiff.h ./_highgui.h ./grfmts.h ./highgui.h ./utils.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\highgui$(DR)_icl\grfmt_pxm.obj .\grfmt_pxm.cpp
..\..\_temp\highgui$(DR)_icl\grfmt_sunras.obj: .\grfmt_sunras.cpp ./bitstrm.h ./grfmt_base.h ./grfmt_bmp.h ./grfmt_jpeg.h ./grfmt_png.h ./grfmt_pxm.h ./grfmt_sunras.h ./grfmt_tiff.h ./_highgui.h ./grfmts.h ./highgui.h ./utils.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\highgui$(DR)_icl\grfmt_sunras.obj .\grfmt_sunras.cpp
..\..\_temp\highgui$(DR)_icl\grfmt_tiff.obj: .\grfmt_tiff.cpp ./bitstrm.h ./grfmt_base.h ./grfmt_bmp.h ./grfmt_jpeg.h ./grfmt_png.h ./grfmt_pxm.h ./grfmt_sunras.h ./grfmt_tiff.h ./_highgui.h ./grfmts.h ./highgui.h ./utils.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\highgui$(DR)_icl\grfmt_tiff.obj .\grfmt_tiff.cpp
..\..\_temp\highgui$(DR)_icl\cvcap.obj: .\cvcap.cpp ./bitstrm.h ./grfmt_base.h ./grfmt_bmp.h ./grfmt_jpeg.h ./grfmt_png.h ./grfmt_pxm.h ./grfmt_sunras.h ./grfmt_tiff.h ./_highgui.h ./grfmts.h ./highgui.h ./utils.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\highgui$(DR)_icl\cvcap.obj .\cvcap.cpp
..\..\_temp\highgui$(DR)_icl\image.obj: .\image.cpp ./bitstrm.h ./grfmt_base.h ./grfmt_bmp.h ./grfmt_jpeg.h ./grfmt_png.h ./grfmt_pxm.h ./grfmt_sunras.h ./grfmt_tiff.h ./_highgui.h ./grfmts.h ./highgui.h ./utils.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\highgui$(DR)_icl\image.obj .\image.cpp
..\..\_temp\highgui$(DR)_icl\loadsave.obj: .\loadsave.cpp ./bitstrm.h ./grfmt_base.h ./grfmt_bmp.h ./grfmt_jpeg.h ./grfmt_png.h ./grfmt_pxm.h ./grfmt_sunras.h ./grfmt_tiff.h ./_highgui.h ./grfmts.h ./highgui.h ./utils.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\highgui$(DR)_icl\loadsave.obj .\loadsave.cpp
..\..\_temp\highgui$(DR)_icl\precomp.obj: .\precomp.cpp ./bitstrm.h ./grfmt_base.h ./grfmt_bmp.h ./grfmt_jpeg.h ./grfmt_png.h ./grfmt_pxm.h ./grfmt_sunras.h ./grfmt_tiff.h ./_highgui.h ./grfmts.h ./highgui.h ./utils.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\highgui$(DR)_icl\precomp.obj .\precomp.cpp
..\..\_temp\highgui$(DR)_icl\utils.obj: .\utils.cpp ./bitstrm.h ./grfmt_base.h ./grfmt_bmp.h ./grfmt_jpeg.h ./grfmt_png.h ./grfmt_pxm.h ./grfmt_sunras.h ./grfmt_tiff.h ./_highgui.h ./grfmts.h ./highgui.h ./utils.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\highgui$(DR)_icl\utils.obj .\utils.cpp
..\..\_temp\highgui$(DR)_icl\window_lnx.obj: .\window_lnx.cpp ./bitstrm.h ./grfmt_base.h ./grfmt_bmp.h ./grfmt_jpeg.h ./grfmt_png.h ./grfmt_pxm.h ./grfmt_sunras.h ./grfmt_tiff.h ./_highgui.h ./grfmts.h ./highgui.h ./utils.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\highgui$(DR)_icl\window_lnx.obj .\window_lnx.cpp
..\..\_temp\highgui$(DR)_icl\window_w32.obj: .\window_w32.cpp ./bitstrm.h ./grfmt_base.h ./grfmt_bmp.h ./grfmt_jpeg.h ./grfmt_png.h ./grfmt_pxm.h ./grfmt_sunras.h ./grfmt_tiff.h ./_highgui.h ./grfmts.h ./highgui.h ./utils.h
	-$(CXX) $(CXXFLAGS)..\..\_temp\highgui$(DR)_icl\window_w32.obj .\window_w32.cpp
