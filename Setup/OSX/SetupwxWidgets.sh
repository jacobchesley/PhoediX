mkdir build-release
cd build-release

../configure --disable-shared --enable-unicode --disable-mediactrl --prefix="$(pwd)"

sudo make