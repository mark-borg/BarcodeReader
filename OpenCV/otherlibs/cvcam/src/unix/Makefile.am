lib_LTLIBRARIES = libcvcam.la

libcvcam_la_SOURCES = convert.cpp cvvideo.cpp dialogs.cpp render.cpp


EXTRA_DIST = 

SOMAJOR = @MMAJOR@
SOMINOR = @MMINOR@
SOSUBMINOR = @MSUBMINOR@

# here you should add all header files, which contains the functionality
# of your shared lib (published for the users)
include_HEADERS =

# here you should add all header files, which shouldn
noinst_HEADERS = convert.h cvvidtypes.h dialogs.h icvvideo.h render.h videodev.h

libcvcam_la_LDFLAGS = -no-undefined -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) -release $(LT_RELEASE) @LDFLAGS@ $(X_LIBS) -L../../../highgui/.libs -L../../../../cv/src/.libs

# add here the libraries which have to be include
# for building the shared lib
#
#  please consider also the usage of the make environment variables
#  like
#  $(LIB_KDEUI), $(LIB_QT), $(LIB_HTML), etc.
#  instead of the hardcoded entries, which have placed
#  in here.
#  The following line is only a suggestion!
#
libcvcam_la_LIBADD = -lX11 -lXext -lXaw

INCLUDES = $(all_includes) -I../../include -I../../../../cv/include -I../../../highgui

AM_CXXFLAGS = @CXXFLAGS@ @DEBUG@ -D_XOPEN_SOURCE=500
