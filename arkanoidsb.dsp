# Microsoft Developer Studio Project File - Name="ArkanoidSB" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ArkanoidSB - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "arkanoidsb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "arkanoidsb.mak" CFG="ArkanoidSB - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ArkanoidSB - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ArkanoidSB - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ArkanoidSB - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "release"
# PROP Intermediate_Dir "release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 SDL_mixer.lib SDL_image.lib SDL.lib SDLmain.lib opengl32.lib /nologo /subsystem:windows /machine:I386 /out:"arkanoidsb.exe"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "ArkanoidSB - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "debug"
# PROP Intermediate_Dir "debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MD /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 SDLmain.lib SDL.lib SDL_image.lib SDL_mixer.lib opengl32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"arkanoidsb.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "ArkanoidSB - Win32 Release"
# Name "ArkanoidSB - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\arkanoidsb.cpp
# End Source File
# Begin Source File

SOURCE=.\src\arkanoidsbgame.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ball.cpp
# End Source File
# Begin Source File

SOURCE=.\src\bonus.cpp
# End Source File
# Begin Source File

SOURCE=.\src\bullet.cpp
# End Source File
# Begin Source File

SOURCE=.\src\coolstring.cpp
# End Source File
# Begin Source File

SOURCE=.\src\energyhole.cpp
# End Source File
# Begin Source File

SOURCE=.\src\exploision.cpp
# End Source File
# Begin Source File

SOURCE=.\src\generatelevel.cpp
# End Source File
# Begin Source File

SOURCE=.\src\glSDL.c
# End Source File
# Begin Source File

SOURCE=.\src\mainmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\src\monster.cpp
# End Source File
# Begin Source File

SOURCE=.\src\mystring.cpp
# End Source File
# Begin Source File

SOURCE=.\src\resource.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\arkanoidsb.h
# End Source File
# Begin Source File

SOURCE=.\src\arkanoidsbgame.h
# End Source File
# Begin Source File

SOURCE=.\src\ball.h
# End Source File
# Begin Source File

SOURCE=.\src\bonus.h
# End Source File
# Begin Source File

SOURCE=.\src\bullet.h
# End Source File
# Begin Source File

SOURCE=.\src\coolstring.h
# End Source File
# Begin Source File

SOURCE=.\src\energyhole.h
# End Source File
# Begin Source File

SOURCE=.\src\exploision.h
# End Source File
# Begin Source File

SOURCE=.\src\generatelevel.h
# End Source File
# Begin Source File

SOURCE=.\src\glsdl.h
# End Source File
# Begin Source File

SOURCE=.\src\mainmenu.h
# End Source File
# Begin Source File

SOURCE=.\src\monster.h
# End Source File
# Begin Source File

SOURCE=.\src\mystring.h
# End Source File
# Begin Source File

SOURCE=.\src\random.h
# End Source File
# Begin Source File

SOURCE=.\src\resource.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\krakout.ico
# End Source File
# End Group
# End Target
# End Project
