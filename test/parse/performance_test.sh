#!/bin/sh
CurDir=`pwd`
ScriptPath=`realpath $0`
ScriptDir=`dirname $ScriptPath`
ProjectDir=`realpath $ScriptDir/../../`
BuildDir=$ProjectDir/build

echo "\033[32m------------ make meojson performance test ------------\033[0m"
make clean -C $ProjectDir
make static "SAMPLE_FILE=test/parse/performance_test.cpp" -C $ProjectDir

RapidSrcDir=$ScriptDir/rapidjson
if [ ! -d $RapidSrcDir ];then
    echo "\033[32m------------ git clone rapidjson ------------\033[0m"
    git clone https://github.com/Tencent/rapidjson.git $RapidSrcDir
# else
#     echo "\033[32m------------ git pull rapidjson ------------\033[0m"
#     cd $RapidSrcDir
#     git pull
#     cd $CurDir
fi

echo "\033[32m------------ make rapidjson performance test ------------\033[0m"
RapidBuildDir=$BuildDir/rapidjson_build
mkdir -p $RapidBuildDir
cmake $ScriptDir -B $RapidBuildDir
make -C $RapidBuildDir

echo "\033[32m------------ generate a random json file ------------\033[0m"
make static -j5 "SAMPLE_FILE=test/generate/generate_test.cpp" -C $ProjectDir
RandJsonPath=$BuildDir/rand.json
$BuildDir/generate_test.out 2 256 > $RandJsonPath

echo "\033[32m\n------------ all ready, start testing ------------\n\033[0m"
echo "\033[32m------------ testing meojson performance------------\033[0m"
$BuildDir/performance_test.out $RandJsonPath 100
echo "\033[32m------------ testing rapidjson performance------------\033[0m"
$RapidBuildDir/rapidjson_performance_test.out $RandJsonPath 100