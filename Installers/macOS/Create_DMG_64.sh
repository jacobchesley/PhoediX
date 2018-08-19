dmgDir="$(pwd)"
cd ..
cd ..
phoedixDir="$(pwd)"
cd Build
cd Release_64
mkdir ForInstaller
cd ForInstaller
forInstaller="$(pwd)"

cp -rf ../PhoediX.app ./
cd PhoediX.app
cp -rf "../../../../3rd Party Licenses" ./
cd ..
mkdir .background
cp "../../../Graphics/DMG Installer Background/dmg_installer_background.png" ./.background

cd $dmgDir
rm -f PhoediX_x64.dmg
rm -f pack.temp.dmg

hdiutil create -srcfolder "$forInstaller" -volname "PhoediX_x64" -fs HFS+ -format UDRW "pack.temp.dmg"
device=$(hdiutil attach -readwrite -noverify -noautoopen "pack.temp.dmg" | egrep '^/dev/' | sed 1q | awk '{print $1}')
sleep 3
echo '
   tell application "Finder"
     tell disk "'PhoediX_x64'"
           open
           set current view of container window to icon view
           set toolbar visible of container window to false
           set statusbar visible of container window to false
           set the bounds of container window to {100, 100, 660, 485}
           set theViewOptions to the icon view options of container window
           set arrangement of theViewOptions to not arranged
           set icon size of theViewOptions to 90
           set background picture of theViewOptions to file ".background:'dmg_installer_background.png'"
           make new alias file at container window to POSIX file "/Applications" with properties {name:"Applications"}
           set position of item "'PhoediX'" of container window to {140, 200}
           set position of item "Applications" of container window to {400, 200}
           update without registering applications
           delay 5
           close
     end tell
   end tell
' | osascript


chmod -Rf go-w /Volumes/PhoediX_x64
sync
sync
hdiutil detach ${device}
hdiutil convert "pack.temp.dmg" -format UDZO -imagekey zlib-level=9 -o "PhoediX_x64.dmg"

rm -f pack.temp.dmg 
rm -rf $forInstaller
