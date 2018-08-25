@echo off

SET VCVARSALL="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat"


REM -----------------------------------------------------------
REM           DO NOT MODIFY ANYTHING BELOW THIS POINT
REM -----------------------------------------------------------

cd %wxWidgets%
cd build
cd msw

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