baseDir ="$(pwd)"cd ..
cd ..
cd ..
phoedixDir="$(pwd)"

cd /Users/jacob/Development/wxWidgets-3.0.4

# Clean wxWidgets builds
rm -rf build-release_64

# Build 64 bit release of wxWidgets
mkdir build-release_64
cd build-release_64

../configure CC="clang -arch x86_64" CXX="clang++ -arch x86_64" --disable-shared --enable-unicode --prefix="$(pwd)"

sudo make -j 4

cd ..

cd $baseDir