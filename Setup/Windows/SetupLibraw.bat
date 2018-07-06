@echo off

SET VCVARSALL="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat"

REM -----------------------------------------------------------
REM           DO NOT MODIFY ANYTHING BELOW THIS POINT
REM -----------------------------------------------------------

cd ..
cd ..

SET PHOEDIX=%CD%

cd %Libraw%
cd lib

echo " "
echo "Removing old library directories"
echo " "
RMDIR /S /Q x86
RMDIR /S /Q x64

cd ..
cd bin
echo " "
echo "Removing old binary directories"
echo " "
RMDIR /S /Q x86
RMDIR /S /Q x64

cd ..

echo " "
echo "Setting up 32 bit build enviornment for LibRaw"
echo " "

call %VCVARSALL% x86

echo " "
echo "********************************************"
echo "        Building 32 bit LibRaw        "
echo "********************************************"
echo " "
nmake.exe -f Makefile.msvc clean
nmake.exe -f Makefile.msvc

echo " "
echo "Copying files in lib to x86"
echo " "
cd lib
MKDIR x86
xcopy %CD% x86
del  %CD%\*.*?

cd ..

echo " "
echo "Copying files in bin to x86"
echo " "
cd bin
MKDIR x86
xcopy %CD% x86
del  %CD%\*.*?

echo " "
echo "Copying libraw.dll to PhoediX\Build\Debug_x86 and PhoediX\Build\Release_x86"
echo " "
cd x86
xcopy libraw.dll %PHOEDIX%\Build\Debug_x86\
xcopy libraw.dll %PHOEDIX%\Build\Release_x86\

cd ..
cd ..

echo " "
echo "Setting up 64 bit build enviornment for LibRaw"
echo " "
call %VCVARSALL% x64

echo " "
echo "********************************************"
echo "        Building 64 bit LibRaw        "
echo "********************************************"
echo " "
nmake.exe -f Makefile.msvc clean
nmake.exe -f Makefile.msvc

echo " "
echo "Copying files in lib to x64"
echo " "
cd lib
MKDIR x64
xcopy %CD% x64
del  %CD%\*.*?

cd ..

echo " "
echo "Copying files in bin to x64"
echo " "
cd bin
MKDIR x64
xcopy %CD% x64
del  %CD%\*.*?

echo " "
echo "Copying libraw.dll to PhoediX\Build\Debug_x64 and PhoediX\Build\Release_x64"
echo " "
cd x64
xcopy libraw.dll %PHOEDIX%\Build\Debug_x64\
xcopy libraw.dll %PHOEDIX%\Build\Release_x64\

cd ..
cd ..

Pause