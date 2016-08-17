#
# The script that creates Makefiles for several compilers from DSP files
#
#   Usage: [python] gen_make.py
#
import re, sys, copy, glob

def long_list( cur_str, prefix, suffix, bare_list ):
    total_str = ""
    for i in bare_list:
        s = prefix + i + suffix
        len_delta = len(s);
        if len(cur_str) + len_delta > 85:
            total_str = total_str + cur_str + "\\\n"
            cur_str = ""
        cur_str = cur_str + s + " "
    return total_str + cur_str + "\n"

class CXX_Compiler:
    def __init__(self,name):
        self.name = name
        self.obj = ".obj"
        self.lib_prefix = ""
        self.lib_ext = ".lib"
        self.lib_link = ""
        self.lib_link_ext = ".lib"
        self.inline_start = ""
        self.inline_end = ""
        self.linkflags_debug = ""
        self.linkflags_release = ""
        self.cxx_switch = "-"
        self.if_prefix = "!"
        self.linkpoststep = ""
        # hack to remove MIL
        self.lib_synonyms = {}
        # self.lib_synonyms = { "mil" : "kernel32", "milmet2" : "kernel32" }

vc = CXX_Compiler("vc")
vc.desc = "Microsoft Visual C++ >=6.0"
vc.cxx = "cl"
vc.link = "link"
vc.cxx_switch = "/"
vc.libdir_opt = "/libpath:"
vc.dependents = "$**"
vc.cxxflags_all = '/nologo /GX /GB /W4 /Zm200 /c /Fo '
vc.cxxflags_release = "/MD /Ox /Ob2 "
vc.cxxflags_debug = "/MDd /Gm /Zi /Od /FD /GZ "
vc.linkflags_all = "/nologo /subsystem:windows /dll /pdb:none " + \
                   "/machine:I386 /out:$(OUTBIN) /implib:$(OUTLIB) "
vc.exeflags_all = "/nologo /subsystem:console /pdb:none " + \
                   "/machine:I386 /out:$(OUTBIN) "
vc.linkflags_debug = "/debug"
vc.make = "nmake"

icl = copy.copy(vc)
icl.name = "icl"
icl.cxx = "icl"
icl.desc = "Intel Proton Compiler >=5.0"
icl.cxxflags_all = '/nologo /GX /G6 /W4 "/Qwd68,171,424,444,869,981,522,9" /c /Fo '
icl.cxxflags_release = "/MD /O3 /Ob2 "
icl.cxxflags_debug = "/MDd /Gm /Zi /Od /FD /GZ "
icl.linkflags_all = vc.linkflags_all + "/nodefaultlib:libmmd /nodefaultlib:libmmdd /nodefaultlib:libm /nodefaultlib:libirc "

bcc = CXX_Compiler("bcc")
bcc.desc = "Borland C++ >=5.5"
bcc.cxx = bcc.link = "bcc32"
bcc.inline_start = "@&&|\n"
bcc.inline_end = "\n|"
bcc.dependents = "$**"
bcc.cxxflags_all = "-3 -a8 -b- -c -d -RT- -tWM -w-inl -w-aus -j200 -o "
bcc.cxxflags_release = "-O2 "
bcc.cxxflags_debug = "-Od -v "
bcc.libdir_opt = "-L"
bcc.linkflags_all = " -tWD -e$(OUTBIN) "
bcc.exeflags_all = " -tWC -e$(OUTBIN) "
bcc.linkpoststep = "implib $(OUTLIB) $(OUTBIN)"
bcc.make = "make"
bcc.lib_synonyms.update({ "zlib" : "zlib_bcc", "libpng" : "libpng_bcc",
                     "libjpeg" : "libjpeg_bcc", "libtiff" : "libtiff_bcc" })

gcc = CXX_Compiler("gcc")
gcc.desc = "GNU C/C++ 3.x or later"
gcc.cxx = gcc.link = "g++"
gcc.obj = ".o"
gcc.lib_prefix = "lib"
gcc.lib_ext = ".a"
gcc.lib_link_ext = ""
gcc.lib_link = "-l"
gcc.if_prefix = ""
gcc.dependents = ""
gcc.cxxflags_all = "-Wall -c -o "
gcc.cxxflags_release = "-O3 -g -fomit-frame-pointer -march=i686 -ffast-math "
#gcc.cxxflags_release = "-O0 -s "
gcc.cxxflags_debug = "-O0 -ggdb3 "
gcc.libdir_opt = "-L"
gcc.linkflags_all = "-o$(OUTBIN) -shared -Wl,-call_shared,--out-implib,$(OUTLIB) "
gcc.exeflags_all = "-o$(OUTBIN) -Wl,-call_shared "
gcc.make = "mingw32-make"
gcc.lib_synonyms.update( { "zlib" : "z", "vfw32" : "vfw_avi32 -lvfw_cap32 -lvfw_ms32" })

def create_makefile( dsppath, level, cxx, outfilename ):

    projname = re.findall(r"([^/\\]+)\.dsp$", dsppath.lower() )[0]
    top="..\\" * level;

    temppath = top + "_temp\\" + projname + "$(DR)_" + cxx.name
    libpath = top + "lib\\" + cxx.lib_prefix + projname + "$(SUFFIX)" + cxx.lib_ext

    dspfile_handle = open( dsppath, "r" )
    dspfile = dspfile_handle.readlines()
    dspfile_handle.close()

    outfile = open( outfilename, "w" )

    exe_flag = 0

    # find out if it is DLL or EXE
    for line in dspfile:
        if re.match( r"^# ADD LINK32.+", line ):
            n = re.findall( r'/out:"(.+?)d\.(dll|exe)"', line )
            if n and n[0]:
                dllpath = n[0][0] + "$(SUFFIX)." + n[0][1]
                if dllpath.endswith('.exe'):
                    exe_flag = 1
    outfile.write( \
           "# Makefile for " + cxx.desc + \
           "\n\nCXX = " + cxx.cxx + \
           "\nLINK = " + cxx.link + \
           "\n\n" + cxx.if_prefix + "ifdef debug" + \
           "\n\nSUFFIX = d" + \
           "\nDR = _Dbg" + \
           "\n\n" + cxx.if_prefix + "else" + \
           "\n\nSUFFIX = " + \
           "\nDR = _Rls" + \
           "\n\n" + cxx.if_prefix + "endif" + \
           "\n\nOUTBIN = " + dllpath + \
           "\nOUTLIB = " + libpath + "\n\n" )

    filelist = []
    barelist = []
    inclist = []
    incpaths = {}
    libpaths = {}
    defines_debug = {}
    defines_release = {}
    libs_debug = ""
    libs_release = ""

    src_file_prog = re.compile( r'^SOURCE=(.*?\.(c|cpp))\n$', re.I )
    hdr_file_prog = re.compile( r'^SOURCE=(.*?\.(h|hpp|inc))\n$', re.I )
    barename_prog = re.compile( r'.*?(\w+)\.(c|cpp)$', re.I )
    incpath_prog = re.compile( r'(.*?)/\w+\.(h|hpp|inc)', re.I )

    for line in dspfile:
        # update list of source (object) files
        src_file = src_file_prog.findall( line )
        if src_file:
            # [0][0] is used because there are 2 groups in the pattern
            src_file = src_file[0][0].lower()
            filelist.append( src_file )
            barename = barename_prog.findall( src_file )
            barelist.append( barename[0][0] )
            continue

        # update list of headers (for dependency lists) and include paths
        hdr_file = hdr_file_prog.findall( line )
        if hdr_file:
            hdr_file = re.sub( r'\\', r'/', hdr_file[0][0].lower() )
            inclist.append( hdr_file )
            incpath = incpath_prog.findall( hdr_file )
            if incpath:
                incpaths[incpath[0][0].lower()] = 1
            continue

        # compiler options
        if re.match( r"^# ADD CPP.+", line ):
            # fetch a list of include directories
            incs = re.findall( r'/I\s*"(\S+)\s+', line )
            for inc in incs:
                inc = re.sub( r"\\", r"/", inc[:-1] )
                incpaths[inc.lower()] = 1

            if re.search( r"_DEBUG", line ):
                # debug options
                define_dict = defines_debug
            else:
                define_dict = defines_release

            # update list of defines
            define_list = re.findall( r'/D\s*"(\S+)\s+', line )
            for define_elem in define_list:
                define_dict[define_elem[:-1]] = 1

            continue

        # linker options
        if re.match( r"# ADD LINK32.+", line ):
            # fetch list of libraries
            libs_str = ""
            lib_list = re.findall( r"(\w+)\.lib ", line )

            for lib_link in lib_list:
                lib_link = cxx.lib_synonyms.get( lib_link.lower(), lib_link )
                libs_str = libs_str + cxx.lib_link + lib_link.lower() + cxx.lib_link_ext + " "

            if re.search( r'/out:".+?d\.(dll|exe)"', line ):
                libs_debug= libs_str
            else:
                libs_release = libs_str

            # fetch a list of lib directories
            libs = re.findall( r'/libpath:\s*"(\S+)\s+', line )
            for l in libs:
                l = re.sub( r"\\", r"/", l[:-1] )
                libpaths[l.lower()] = 1

            continue
    # end of .dsp processing loop

    outfile.write( long_list( "OBJS = ", temppath + "\\", cxx.obj, barelist ) + "\n" )
    if cxx.name == "gcc":
        outfile.write( long_list( "OBJ_SHRT = ", "", cxx.obj, barelist ) + "\n" )
    outfile.write( long_list( "\n\nINC = ", "", "", inclist ) + "\n" )

    cxxflags = ""
    cxxflags_debug = ""
    cxxflags_release = ""

    for inc_elem in incpaths.keys():
        cxxflags = cxxflags + cxx.cxx_switch + 'I"' + inc_elem + '" '

    for define_elem in defines_debug.keys():
        cxxflags_debug = cxxflags_debug + cxx.cxx_switch + 'D"' + define_elem + '" '

    for define_elem in defines_release.keys():
        cxxflags_release = cxxflags_release + cxx.cxx_switch + 'D"' + define_elem + '" '

    linkflags = ""
    for lib_elem in libpaths.keys():
        linkflags = linkflags + cxx.libdir_opt + '"' + lib_elem + '" '

    if exe_flag:
        linkflags_all = cxx.exeflags_all
    else:
        linkflags_all = cxx.linkflags_all

    # print middle part of makefile
    outfile.write( "\nCXXFLAGS2 = " + cxxflags + cxx.cxxflags_all + \
                   "\nLINKFLAGS2 = " + linkflags + linkflags_all + \
                   "\n\n" + cxx.if_prefix + "ifdef debug" + \
                   "\n\nCXXFLAGS = " + cxxflags_debug + cxx.cxxflags_debug + "$(CXXFLAGS2)" + \
                   "\nLIBS = " + libs_debug + \
                   "\nLINKFLAGS = $(LINKFLAGS2) " + cxx.linkflags_debug + \
                   "\n\n" + cxx.if_prefix + "else" + \
                   "\n\nCXXFLAGS = " + cxxflags_release + cxx.cxxflags_release + "$(CXXFLAGS2)" + \
                   "\nLIBS = " + libs_release + \
                   "\nLINKFLAGS = $(LINKFLAGS2) " + cxx.linkflags_release + \
                   "\n\n" + cxx.if_prefix + "endif" + \
                   "\n\n\n$(OUTBIN): $(OBJS)" + \
                   "\n\t-mkdir " + top + "bin 2> nul" + \
                   "\n\t-mkdir " + top + "lib 2> nul" )

    if cxx.name != "gcc":
        outfile.write( "\n\t$(LINK) " + cxx.inline_start + "$(LINKFLAGS) " + \
                       cxx.dependents + " $(LIBS) " + cxx.inline_end + \
                       "\n\t" + cxx.linkpoststep )
    else:
        outfile.write( '\n\t-move "' + temppath + '\\*.o" .' + \
                       '\n\t$(LINK) $(LINKFLAGS) $(OBJ_SHRT) $(LIBS)' + \
                       '\n\t-move *.o "' + temppath + '"' )
    outfile.write( "\n\nall: $(OUTBIN)\n\n" )

    inc_str = " " + " ".join( inclist ) + "\n";

    # print dependencies
    for i in range(len(barelist)):
        outfile.write( temppath + "\\" + barelist[i] + cxx.obj + \
                       ": " + filelist[i] + inc_str )
        if i == 0:
            outfile.write( "\t@-mkdir " + temppath + " 2>nul\n" )

        outfile.write( "\t-$(CXX) $(CXXFLAGS)" + temppath + "\\" + barelist[i] + \
                       cxx.obj + " " + filelist[i] + "\n" )

    outfile.close()
# end of create_makefile function

###################### Main Loop ############################

folderlist = [
    "cxcore\\src",
    "cv\\src",
    "cvaux\\src",
    "otherlibs\\highgui",
    "tests\\cxts",
    "tests\\cxcore\\src",
    "tests\\trs",
    "tests\\cv\\src",
]

for cxx in [vc,icl,bcc,gcc]:
    makefile = open( "..\\_make\\makefile." + cxx.name, "w" );

    makefile.write( "# " + cxx.desc + \
                    "\n\n" + cxx.if_prefix + "ifdef debug" + \
                    "\nOPT= debug=1" + \
                    "\n" + cxx.if_prefix + "else" + \
                    "\nOPT= " + \
                    "\n" + cxx.if_prefix + "endif" + \
                    "\n\nall:" )

    for folder in folderlist:
        level = len(re.findall(r'[/\\]', folder )) + 1

        dsp_list = glob.glob("..\\" + folder + "\\*.dsp")
        if not dsp_list: # if there is no dsp, just put a reference to makefile
            dsp_list = [""]
        for dsppath in dsp_list:
            if dsppath != "":
                create_makefile( dsppath, level, cxx, "..\\" + folder + "\\makefile." + cxx.name )
            if cxx.name != "gcc":
                makefile.write( "\n\tcd ..\\" + folder + \
                                "\n\t" + cxx.make + " -f makefile." + \
                                cxx.name + " $(OPT)" + \
                                "\n\tcd " + "..\\" * level + "_make" )
            else:
                makefile.write( "\n\t" + cxx.make + " -C ..\\" + folder + \
                                " -f makefile." + cxx.name )

    makefile.close()

