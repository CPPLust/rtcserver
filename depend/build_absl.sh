
rm build -rf
mkdir -p build
cd build

cmake .. -DCMAKE_INSTALL_PREFIX=`pwd`/out
make -j$(nproc)
make install
