# Microsoft Developer Studio Project File - Name="BarcodeReader" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=BarcodeReader - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "BarcodeReader.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "BarcodeReader.mak" CFG="BarcodeReader - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "BarcodeReader - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "BarcodeReader - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "BarcodeReader - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d:/opencv/lib/cxcore.lib d:/opencv/lib/cv.lib d:/opencv/lib/highgui.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "BarcodeReader - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_DLL" /D "_AFXDLL" /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 d:/opencv/lib/cxcored.lib d:/opencv/lib/cvd.lib d:/opencv/lib/highguid.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "BarcodeReader - Win32 Release"
# Name "BarcodeReader - Win32 Debug"
# Begin Group "qvcore"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\qvcore\AreaVarianceDetector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\qvcore\AreaVarianceDetector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\qvcore\BarcodeDecoder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\qvcore\BarcodeDecoder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\qvcore\BarcodeReader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\qvcore\BarcodeReader.h
# End Source File
# Begin Source File

SOURCE=..\..\..\qvcore\Camera.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\qvcore\Camera.h
# End Source File
# Begin Source File

SOURCE=..\..\..\qvcore\common.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\qvcore\common.h
# End Source File
# Begin Source File

SOURCE=..\..\..\qvcore\ContainerAreaFinder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\qvcore\ContainerAreaFinder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\qvcore\ContainerOutlineFinder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\qvcore\ContainerOutlineFinder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\qvcore\coord_geom.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\qvcore\coord_geom.h
# End Source File
# Begin Source File

SOURCE=..\..\..\qvcore\EAN13BarcodeDecoder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\qvcore\EAN13BarcodeDecoder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\qvcore\framework.h
# End Source File
# Begin Source File

SOURCE=..\..\..\qvcore\graphics.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\qvcore\graphics.h
# End Source File
# Begin Source File

SOURCE=..\..\..\qvcore\HighTexturedAreaFinder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\qvcore\HighTexturedAreaFinder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\qvcore\NumberedFileCamera.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\qvcore\NumberedFileCamera.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# End Target
# End Project
