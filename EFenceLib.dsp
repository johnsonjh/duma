# Microsoft Developer Studio Project File - Name="EFenceLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=EFenceLib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "EFenceLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "EFenceLib.mak" CFG="EFenceLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "EFenceLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "EFenceLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "EFenceLib - Win32 Release"

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
# ADD CPP -MD /W3 /I "$(QTDIR)\include" /I "$(QTDIR)\mkspecs\win32-msvc" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D UNICODE /D QT_THREAD_SUPPORT /D QT_DLL /D QT_THREAD_SUPPORT /FD /c -nologo -Zm200 -GX -O1 -MT
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD LIB32 /nologo 

!ELSEIF  "$(CFG)" == "EFenceLib - Win32 Debug"

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
# ADD CPP -MDd /W3 /Gm /GZ /ZI /Od /I "$(QTDIR)\include" /I "$(QTDIR)\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D UNICODE /D QT_THREAD_SUPPORT /D QT_DLL /D QT_THREAD_SUPPORT /FD /c -nologo -Zm200 -GX  -Z7 -MTd
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD LIB32 /nologo 

!ENDIF 

# Begin Target

# Name "EFenceLib - Win32 Release"
# Name "EFenceLib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=efence.c
# End Source File
# Begin Source File

SOURCE=page.c
# End Source File
# Begin Source File

SOURCE=print.c
# End Source File
# Begin Source File

SOURCE=efencepp.cpp
# End Source File

# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=efence.h

# End Source File
# Begin Source File

SOURCE=efencepp.h

# End Source File
# Begin Source File

SOURCE=efenceint.h

# End Source File

# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group







# Begin Group "Generated"





# Prop Default_Filter "moc"
# End Group
# End Target
# End Project

