#!/bin/bash

echo -e "\033[32m------------ make meojson conformance test ------------\033[0m"
cd ../../
make clean
make debug "SAMPLE_FILE=test/parse/conformance_test.cpp"

cd build
if [ ! -d "nativejson-benchmark" ];then
    echo -e "\033[32m------------ git clone nativejson-benchmark ------------\033[0m"
    git clone https://github.com/miloyip/nativejson-benchmark.git
else
    echo -e "\033[32m------------ git pull nativejson-benchmark ------------\033[0m"
    cd nativejson-benchmark
    git pull
    cd ..
fi

echo -e "\033[32m------------ testing meojson conformance------------\033[0m"
./conformance_test.out `ls nativejson-benchmark/data/*/*`
