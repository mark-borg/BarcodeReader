EXTRA_DIST = performance.cpp createsamples.cpp

bin_PROGRAMS = haartraining

haartraining_SOURCES = cvboost.cpp cvhaarclassifier.cpp cvhaartraining.cpp cvcommon.cpp haartraining.cpp

noinst_HEADERS = cvclassifier.h _cvhaartraining.h _cvcommon.h

INCLUDES = -I../../../cxcore/include -I../../../cv/include -I../../../cv/src -I../../../otherlibs/highgui -I./ -I../include
LIBS = -L../../../cxcore/src -lcxcore -L../../../cv/src -lopencv -L../../../otherlibs/highgui -lhighgui

CXX=@CXXOPENMP@
