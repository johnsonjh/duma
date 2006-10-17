# Microsoft Developer Studio Project File - Name="stacktrace" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=stacktrace - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "stacktrace.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "stacktrace.mak" CFG="stacktrace - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "stacktrace - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "stacktrace - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "stacktrace - Win32 Detours" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "stacktrace - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\.." /I "..\..\stacktrace" /I "..\..\stacktrace\internal" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "DUMA_NO_CPP_SUPPORT" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "stacktrace - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\.." /I "..\..\stacktrace" /I "..\..\stacktrace\internal" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "DUMA_NO_CPP_SUPPORT" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "stacktrace - Win32 Detours"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Detours"
# PROP BASE Intermediate_Dir "Detours"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Detours"
# PROP Intermediate_Dir "Detours"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\.." /I "..\..\stacktrace" /I "..\..\stacktrace\internal" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "DUMA_NO_CPP_SUPPORT" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "stacktrace - Win32 Release"
# Name "stacktrace - Win32 Debug"
# Name "stacktrace - Win32 Detours"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\stacktrace\internal\MapFile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\stacktrace\internal\MapFileEntry.cpp
# End Source File
# Begin Source File

SOURCE=..\..\stacktrace\printStackTrace.cpp
# End Source File
# Begin Source File

SOURCE=..\..\stacktrace\internal\StackTrace.cpp
# End Source File
# Begin Source File

SOURCE=..\..\stacktrace\internal\TextFile.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\stacktrace\internal\Array.h
# End Source File
# Begin Source File

SOURCE=..\..\stacktrace\internal\MapFile.h
# End Source File
# Begin Source File

SOURCE=..\..\stacktrace\internal\MapFileEntry.h
# End Source File
# Begin Source File

SOURCE=..\..\stacktrace\printStackTrace.h
# End Source File
# Begin Source File

SOURCE=..\..\stacktrace\internal\StackTrace.h
# End Source File
# Begin Source File

SOURCE=..\..\stacktrace\internal\TextFile.h
# End Source File
# End Group
# End Target
# End Project
