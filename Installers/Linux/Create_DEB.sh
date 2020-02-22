debDir="$(pwd)"
cd ..
cd ..
phoedixDir="$(pwd)"

rm -rf $debDir/PhoediX

mkdir -p $debDir/PhoediX
mkdir -p $debDir/PhoediX/usr/bin
mkdir -p $debDir/PhoediX/usr/lib
mkdir -p $debDir/PhoediX/usr/share/applications
mkdir -p $debDir/PhoediX/usr/share/icons
mkdir -p $debDir/PhoediX/usr/share/doc/phoedix/
mkdir -p $debDir/PhoediX/DEBIAN

cp $phoedixDir/Build/Release_64/phoedix $debDir/PhoediX/usr/bin
cp -a $phoedixDir/Build/Release_64/lib/. $debDir/PhoediX/usr/lib
cp -a $phoedixDir/Icon/Linux/. $debDir/PhoediX/usr/share/
cp -a "$phoedixDir/3rd Party Licenses" $debDir/PhoediX/usr/share/doc/phoedix/

cd $debDir/PhoediX/DEBIAN

touch control
echo Package: PhoediX >> control
echo Version: 1.0.0-alpha.1 >> control
echo Section: base >> control
echo Priority: optional >> control
echo Architecture: amd64 >> control
echo Maintainer: Jacob Chesley >> control
echo Description: PhoediX >> control
echo " A non destructuve image editing and raw processing program." >> control
echo " Handles RAW Files, offers 16 bits per channel processing," >> control
echo " and customizable edit order." >> control
echo Homepage: http://www.phoedix.com >> control

touch copyright
echo Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/ >> copyright
echo Upstream-Name: PhoediX >> copyright
echo Upstream-Contact: Jacob Chesley >> copyright
echo Source: http://www.github.com/jacobchesley/phoedix >> copyright
echo >> copyright
echo Files: * >> copyright
echo Copyright: 2020 Jacob Chesley >> copyright
echo License: MIT >> copyright
echo " For more information, see /usr/share/doc/phoedix/Copyrights.txt" >> copyright
echo >> copyright
echo Files: usr/lib/libraw_r.so.19 >> copyright
echo Copyright: 2008-2020 LibRaw LLC >> copyright
echo License: LGPL >> copyright
echo " For more information, see /usr/share/doc/phoedix/Copyrights.txt" >> copyright
echo >> copyright
echo Files: usr/lib/libtiff.so.5 >> copyright
echo Copyright: 1988-1997 Sam Leffler, 1991-1997 Silicon Graphics, Inc. >> copyright
echo License: BSD-Like >> copyright
echo " For more information, see /usr/share/doc/phoedix/Copyrights.txt" >> copyright

cp copyright $debDir/PhoediX/usr/share/doc/phoedix

cd $debDir/PhoediX/usr/share/applications
touch phoedix.desktop
echo [Desktop Entry] >> phoedix.desktop
echo Type=Application >> phoedix.desktop
echo Encoding=UTF-8 >> phoedix.desktop
echo Name=PhoediX >> phoedix.desktop
echo Exec=phoedix >> phoedix.desktop
echo Icon=phoedix >> phoedix.desktop

cd $debDIr

dpkg-deb --build $debDir/PhoediX
