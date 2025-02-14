
rm build -rf
mkdir -p build
cd build

cmake -DCMAKE_C_COMPILER=/opt/gcc-10.2/bin/gcc \
      -DCMAKE_CXX_COMPILER=/opt/gcc-10.2/bin/g++  .. -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_INSTALL_PREFIX=`pwd`/out
make -j$(nproc)
make install
