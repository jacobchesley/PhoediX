@echo off

SET VCVARSALL="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat"

REM -----------------------------------------------------------
REM           DO NOT MODIFY ANYTHING BELOW THIS POINT
REM -----------------------------------------------------------

cd..
cd..

SET PHOEDIX=%CD%

cd %Libtiff%
cd libtiff

echo " "
echo "Removing old library directories"
echo " "
RMDIR /S /Q x86
RMDIR /S /Q x64

echo " "
echo "Setting up 32 bit build enviornment for LibTiff"
echo " "

call %VCVARSALL% x86

echo " "
echo "********************************************"
echo "        Building 32 bit LibTiff        "
echo "********************************************"
echo "

cd ..
cd port
nmake /f Makefile.vc clean
nmake /f Makefile.vc 
cd ..
cd libtiff

nmake.exe /f Makefile.vc clean
nmake.exe /f Makefile.vc all

MKDIR x86
xcopy *.obj x86
xcopy *.ilk x86
xcopy *.exp x86
xcopy libtiff.dll x86

echo " "
echo "Copying libtiff.dll to PhoediX\Build\Debug_x86 and PhoediX\Build\Release_x86"
echo " "
cd x86
xcopy libtiff.dll %PHOEDIX%\Build\Debug_x86\
xcopy libtiff.dll %PHOEDIX%\Build\Release_x86\


echo " "
echo "Setting up 64 bit build enviornment for LibRaw"
echo " "
call %VCVARSALL% x64

echo " "
echo "********************************************"
echo "        Building 64 bit LibTiff        "
echo "********************************************"
echo " "

cd %Libtiff%
cd port
nmake /f Makefile.vc clean
nmake /f Makefile.vc 
cd ..
cd libtiff

nmake.exe /f Makefile.vc clean
nmake.exe /f Makefile.vc all


echo " "

MKDIR x64
xcopy *.obj x64
xcopy *.ilk x64
xcopy *.exp x64
xcopy libtiff.dll x64

echo " "
echo "Copying libtiff.dll to PhoediX\Build\Debug_x64 and PhoediX\Build\Release_x64"
echo " "

xcopy libtiff.dll %PHOEDIX%\Build\Debug_x64\
xcopy libtiff.dll %PHOEDIX%\Build\Release_x64\

nmake.exe /f Makefile.vc clean


if exist tiffconfig.h (
    echo "tiffconfig.h exists."
) else (
    echo "Creating tiffconfig.h"
    type tiffconf.vc.h > tiffconf.h
)

Pause