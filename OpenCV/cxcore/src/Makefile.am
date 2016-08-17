lib_LTLIBRARIES = libcxcore.la

libcxcore_la_SOURCES =

SOMAJOR = @MMAJOR@
SOMINOR = @MMINOR@
SOSUBMINOR = @MSUBMINOR@

# here you should add all header files, which contains the functionality
# of your shared lib (published for the users)
include_HEADERS =

# here you should add all header files, which shouldn't be installed
noinst_HEADERS =

EXTRA_DIST =

#libopencv_la_LDFLAGS = $(all_libraries) -no-undefined -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) -release $(LT_RELEASE) @LDFLAGS@
libcxcore_la_LDFLAGS = -no-undefined -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) -release $(LT_RELEASE)

LIBS = -ldl -lpthread -lm

INCLUDES = -I. -I../include -I../..


