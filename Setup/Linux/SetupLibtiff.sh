baseDir="$(pwd)"

cd /home/ne/Development/tiff-4.0.9

# Clean LibTiff builds
rm -rf build-release_64

# Build 64 bit release of LibTiff
mkdir build-release_64
cd build-release_64

../configure --prefix="$(pwd)" --disable-static &&
sudo make

cp ./libtiff/tiffconf.h ../libtiff

cd $baseDir
