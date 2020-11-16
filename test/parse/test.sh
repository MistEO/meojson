#!/bin/bash

echo "------------ make meojson test ------------"
make clean -C ../../
make debug "SAMPLE_FILE=test/parse/meojson_test.cpp" -C ../../

if [ ! -d "rapidjson" ];then
    echo "------------ git clone rapidjson ------------"
    git clone https://github.com/Tencent/rapidjson.git
else
    echo "------------ git pull rapidjson ------------"
    cd rapidjson
    git pull
    cd ..
fi
echo "------------ make rapidjson test ------------"
mkdir rapidjson_build
cd rapidjson_build
cmake ..
make
cd ..

echo "------------ cp rapidjson_test to cpp json build directory ------------"
cp rapidjson_build/rapidjson_test.out ../../build

echo "------------ generate a random json file ------------"
make debug -j5 "SAMPLE_FILE=test/generate/generate_test.cpp" -C ../../
cd ../../build
./generate_test.out > rand.json

echo "------------ all ready, start testing ------------"
echo "------------ testing meojson ------------"
./meojson_test.out
echo "------------ testing rapidjson ------------"
./rapidjson_test.out