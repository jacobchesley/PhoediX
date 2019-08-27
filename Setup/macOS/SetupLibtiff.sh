baseDir ="$(pwd)"cd ..
cd ..
cd ..
phoedixDir="$(pwd)"

cd /Users/jacob/Development/tiff-4.0.10

# Clean LibTiff builds
rm -rf build-release_64

# Build 64 bit release of LibTiff
mkdir build-release_64
cd build-release_64
../configure CC="clang -arch x86_64" CXX="clang++ -arch x86_64" --prefix="$(pwd)" --disable-static &&
sudo make
cp ./libtiff/tiffconf.h ../libtiff
cp ./libtiff/.libs/libtiff.5.dylib $phoedixDir/build/Release_64/lib

cd ..

cd $baseDir