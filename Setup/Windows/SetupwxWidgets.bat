@echo off

SET VCVARSALL="C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\vcvarsall.bat"


REM -----------------------------------------------------------
REM           DO NOT MODIFY ANYTHING BELOW THIS POINT
REM -----------------------------------------------------------

cd %wxWidgets%
cd build
cd msw

call %VCVARSALL% x86

echo " "
echo "********************************************"
echo "     Cleaning debug 32 bit wxWidgets     "
echo "********************************************"
echo " "
nmake.exe -f makefile.vc BUILD=debug clean

echo " "
echo "********************************************"
echo "     Cleaning release 32 bit wxWidgets     "
echo "********************************************"
echo " "
nmake.exe -f makefile.vc BUILD=release clean

call %VCVARSALL% x64

echo " "
echo "********************************************"
echo "     Cleaning debug 64 bit wxWidgets     "
echo "********************************************"
echo " "
nmake.exe -f makefile.vc BUILD=debug TARGET_CPU=x64 clean

echo " "
echo "********************************************"
echo "     Cleaning release 64 bit wxWidgets     "
echo "********************************************"
echo " "
nmake.exe -f makefile.vc BUILD=release TARGET_CPU=x64 clean


echo " "
echo "Setting up 32 bit build enviornment for wxWidgets"
echo " "
call %VCVARSALL% x86

echo " "
echo "********************************************"
echo "     Building debug 32 bit wxWidgets     "
echo "********************************************"
echo " "
nmake.exe -f makefile.vc SHARED=0 BUILD=debug RUNTIME_LIBS=static

echo " "
echo "********************************************"
echo "     Building release 32 bit wxWidgets     "
echo "********************************************"
echo " "
nmake.exe -f makefile.vc SHARED=0 BUILD=release RUNTIME_LIBS=static

echo " "
echo "Setting up 64 bit build enviornment for wxWidgets"
echo " "
call %VCVARSALL% x64

echo " "
echo "********************************************"
echo "     Building debug 64 bit wxWidgets     "
echo "********************************************"
echo " "

nmake.exe -f makefile.vc SHARED=0 BUILD=debug RUNTIME_LIBS=static TARGET_CPU=x64

echo " "
echo "********************************************"

echo "     Building release 64 bit wxWidgets     "
echo "********************************************"
echo " "
nmake.exe -f makefile.vc SHARED=0 BUILD=release RUNTIME_LIBS=static TARGET_CPU=x64

pause