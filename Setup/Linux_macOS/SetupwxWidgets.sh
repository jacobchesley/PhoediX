mkdir build-release
cd build-release

../configure --disable-shared --enable-unicode --prefix="$(pwd)"

sudo make

# Uncomment lines below to enable debug build of wxWidgets in separate directory

#cd ..
#mkdir build-debug
#cd build-debug

#../configure --disable-shared --enable-unicode --enable-debug --prefix="$(pwd)"

#sudo make