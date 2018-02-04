baseDir ="$(pwd)"
cd /Users/jacob/Development/tiff-4.0.9

# Clean LibTiff builds
rm -rf build-release_32
rm -rf build-release_64
rm -rf build-debug_32
rm -rf build-debug_64

# Build 32 bit release of LibTiff
mkdir build-release_32
cd build-release_32
../configure CC="clang -arch i386" CXX="clang++ -arch i386" --prefix="$(pwd)" --disable-static &&
sudo make
cp ./libtiff/tiffconf.h ../libtiff

cd ..

# Build 64 bit release of LibTiff
mkdir build-release_64
cd build-release_64
../configure CC="clang -arch x86_64" CXX="clang++ -arch x86_64" --prefix="$(pwd)" --disable-static &&
sudo make
cp ./libtiff/tiffconf.h ../libtiff

cd ..

cd $baseDir
