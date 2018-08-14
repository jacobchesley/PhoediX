baseDir="$(pwd)"
cd ..
cd ..
phoedixDir="$(pwd)"

cd /Users/jacob/Development/LibRaw-0.19.0

# Clean LibRaw builds
rm -rf build-release_32
rm -rf build-release_64
rm -rf build-debug_32
rm -rf build-debug_64

# Build 32 bit release of LibRaw
mkdir build-release_32
cd build-release_32
../configure CC="clang -arch i386" CXX="clang++ -arch i386" --prefix="$(pwd)" --disable-static &&
sudo make
cp ./lib/.libs/libraw_r.19.dylib $phoedixDir/build/Release_32/lib

cd ..

# Build 64 bit release of LibRaw
mkdir build-release_64
cd build-release_64
../configure CC="clang -arch x86_64" CXX="clang++ -arch x86_64" --prefix="$(pwd)" --disable-static &&
sudo make
cp ./lib/.libs/libraw_r.19.dylib $phoedixDir/build/Release_64/lib

cd ..

cd $baseDir