#!/bin/bash

echo -e "\033[32m------------ make meojson test ------------\033[0m"
make clean -C ../../
make debug "SAMPLE_FILE=test/parse/meojson_test.cpp" -C ../../

if [ ! -d "rapidjson" ];then
    echo -e "\033[32m------------ git clone rapidjson ------------\033[0m"
    git clone https://github.com/Tencent/rapidjson.git
else
    echo -e "\033[32m------------ git pull rapidjson ------------\033[0m"
    cd rapidjson
    git pull
    cd ..
fi
echo -e "\033[32m------------ make rapidjson test ------------\033[0m"
mkdir rapidjson_build
cd rapidjson_build
cmake ..
make
cd ..

echo -e "\033[32m------------ cp rapidjson_test to cpp json build directory ------------\033[0m"
cp rapidjson_build/rapidjson_test.out ../../build

echo -e "\033[32m------------ generate a random json file ------------\033[0m"
make debug -j5 "SAMPLE_FILE=test/generate/generate_test.cpp" -C ../../
cd ../../build
./generate_test.out 2 256 > rand.json

echo -e "\033[32m------------ all ready, start testing ------------\033[0m"
echo -e "\033[32m------------ testing meojson ------------\033[0m"
./meojson_test.out rand.json 10
echo -e "\033[32m------------ testing rapidjson ------------\033[0m"
./rapidjson_test.out rand.json 10