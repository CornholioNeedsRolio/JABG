#!/bin/bash

mkdir -p Binary
cd Binary

mkdir -p Release
mkdir -p Debug

if  [[ $1 = debug ]]; then
    cd Debug
    cmake -DCMAKE_BUILD_TYPE=Debug ../..
    make
elif [[ $1 = release ]]; then
    cd Release
    cmake -DCMAKE_BUILD_TYPE=Release ../..
    make
fi

if [[ $2 == run ]]; then
    mkdir -p bin
   mv -f ./JABG ./bin/JABG
    rm -f -r ./bin/res
    cp  -R --copy-contents ../../res ./bin/res
    cd bin

    if [[ $1 == debug ]]; then
        gdb ./JABG
    else
        ./JABG
    fi
fi
