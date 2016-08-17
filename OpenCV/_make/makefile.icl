# Intel Proton Compiler >=5.0

!ifdef debug
OPT= debug=1
!else
OPT= 
!endif

all:
	cd ..\cxcore\src
	nmake -f makefile.icl $(OPT)
	cd ..\..\_make
	cd ..\cv\src
	nmake -f makefile.icl $(OPT)
	cd ..\..\_make
	cd ..\cvaux\src
	nmake -f makefile.icl $(OPT)
	cd ..\..\_make
	cd ..\otherlibs\highgui
	nmake -f makefile.icl $(OPT)
	cd ..\..\_make
	cd ..\tests\cxts
	nmake -f makefile.icl $(OPT)
	cd ..\..\_make
	cd ..\tests\cxcore\src
	nmake -f makefile.icl $(OPT)
	cd ..\..\..\_make
	cd ..\tests\trs
	nmake -f makefile.icl $(OPT)
	cd ..\..\_make
	cd ..\tests\cv\src
	nmake -f makefile.icl $(OPT)
	cd ..\..\..\_make