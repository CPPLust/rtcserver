#!/bin/bash
#https://www.openssl.org/source/openssl-1.1.1d.tar.gz

OPENSSL_VERSION="1.1.1m"
SOURCE="openssl-$OPENSSL_VERSION"
SHELL_PATH=`pwd`
OPENSSL_PATH=$SHELL_PATH/$SOURCE

PREFIX=$SHELL_PATH/build


#rm -rf "$PREFIX"
#rm -rf "$SOURCE"
if [ ! -r $SOURCE ]
then
    OPENSSL_TAR_NAME="$SOURCE.tar.gz"
    if [ ! -f "$SHELL_PATH/$OPENSSL_TAR_NAME" ]
    then
        echo "$SHELL_PATH/$OPENSSL_TAR_NAME source not found, Trying to download..."
        curl -O https://www.openssl.org/source/$OPENSSL_TAR_NAME
    fi
    mkdir $OPENSSL_PATH
    tar zxvf $SHELL_PATH/$OPENSSL_TAR_NAME --strip-components 1 -C $OPENSSL_PATH || exit 1
fi

cd $OPENSSL_PATH

./config \
    --prefix=$PREFIX no-shared no-asm  no-tests no-zlib  || exit 1
 make -j3 && make -j3 install || exit 1
 make clean




echo "mac openssl bulid success!"


