baseDir="$(pwd)"

cd /home/ne/Development/wxWidgets-3.0.4

# Clean wxWidgets builds
rm -rf build-release_64

# Build 64 bit release of wxWidgets
mkdir build-release_64
cd build-release_64

../configure --disable-shared --enable-unicode --prefix="$(pwd)"
sudo make

cd $baseDir


