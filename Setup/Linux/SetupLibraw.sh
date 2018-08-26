baseDir="$(pwd)"

cd /home/ne/Development/LibRaw-0.19.0

# Clean LibRaw builds
rm -rf build-release_64

# Build 64 bit release of LibRaw
mkdir build-release_64
cd build-release_64

../configure --prefix="$(pwd)" --disable-static &&
sudo make

cd $baseDir
