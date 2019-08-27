baseDir ="$(pwd)"cd ..
cd ..
cd ..
phoedixDir="$(pwd)"

cd /Users/jacob/Development/wxWidgets-3.1.2

# Clean wxWidgets builds
rm -rf build-release_64

# Build 64 bit release of wxWidgets
mkdir build-release_64
cd build-release_64

../configure CC="clang -arch x86_64" CXX="clang++ -arch x86_64" --disable-shared --enable-unicode --prefix="$(pwd)" --with-cxx=14 --enable-stl

sudo make -j 4

cd ..

cd $baseDir