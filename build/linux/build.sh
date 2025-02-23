#!/bin/bash
if [ ! -d "./out" ];then
    mkdir out
fi

cd out && cmake -DCMAKE_C_COMPILER=/opt/gcc-10.2/bin/gcc \
      -DCMAKE_CXX_COMPILER=/opt/gcc-10.2/bin/g++  .. -DCMAKE_VERBOSE_MAKEFILE=ON

if test $# -gt 0 && test $1 = "clean"
then
    echo "make clean"
    make clean
# elif test $# -gt 0 && test $1 = "install"
# then
#     echo "make install"
#     make install
else
    echo "make"
    make -j5
fi


cd -
mv out/xrtcserver .
