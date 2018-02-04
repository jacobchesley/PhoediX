cd /Users/jacob/Development/wxWidgets-3.1.0

# Clean wxWidgets builds
rm -rf build-release_32
rm -rf build-release_64
rm -rf build-debug_32
rm -rf build-debug_64

# Build 32 bit release of wxWidgets
mkdir build-release_32
cd build-release_32

../configure CC="clang -arch i386" CXX="clang++ -arch i386" --disable-shared --enable-unicode --disable-mediactrl --prefix="$(pwd)"

sudo make -j 4

cd ..

# Build 64 bit release of wxWidgets
mkdir build-release_64
cd build-release_64

../configure CC="clang -arch x86_64" CXX="clang++ -arch x86_64" --disable-shared --enable-unicode --disable-mediactrl --prefix="$(pwd)"

sudo make -j 4

cd ..
