#!/bin/bash

echo -e "\033[32m------------ make meojson performance test ------------\033[0m"
make clean -C ../../
make debug "SAMPLE_FILE=test/parse/performance_test.cpp" -C ../../

if [ ! -d "rapidjson" ];then
    echo -e "\033[32m------------ git clone rapidjson ------------\033[0m"
    git clone https://github.com/Tencent/rapidjson.git
else
    echo -e "\033[32m------------ git pull rapidjson ------------\033[0m"
    cd rapidjson
    git pull
    cd ..
fi
echo -e "\033[32m------------ make rapidjson performance test ------------\033[0m"
mkdir rapidjson_build
cd rapidjson_build
cmake ..
make
cd ..

echo -e "\033[32m------------ copy rapidjson_performance_test.out to meojson build directory ------------\033[0m"
cp rapidjson_build/rapidjson_performance_test.out ../../build

echo -e "\033[32m------------ generate a random json file ------------\033[0m"
make debug -j5 "SAMPLE_FILE=test/generate/generate_test.cpp" -C ../../
cd ../../build
./generate_test.out 2 256 > rand.json

echo -e "\033[32m\n------------ all ready, start testing ------------\n\033[0m"
echo -e "\033[32m------------ testing meojson performance------------\033[0m"
./performance_test.out rand.json 100
echo -e "\033[32m------------ testing rapidjson performance------------\033[0m"
./rapidjson_performance_test.out rand.json 100