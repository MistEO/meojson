#!/bin/bash
CurDir=`pwd`
ScriptPath=`realpath $0`
ScriptDir=`dirname $ScriptPath`
ProjectDir=`realpath $ScriptDir/../../`
BuildDir=$ProjectDir/build

echo -e "\033[32m------------ make meojson conformance test ------------\033[0m"
make clean -C $ProjectDir
make debug "SAMPLE_FILE=test/parse/conformance_test.cpp" -C $ProjectDir

NativejsonDir=$BuildDir/nativejson-benchmark
if [ ! -d $NativejsonDir ];then
    echo -e "\033[32m------------ git clone nativejson-benchmark ------------\033[0m"
    git clone https://github.com/miloyip/nativejson-benchmark.git $NativejsonDir
# else
#     echo -e "\033[32m------------ git pull nativejson-benchmark ------------\033[0m"
#     cd $NativejsonDir
#     git pull
#     cd $CurDir
fi

echo -e "\033[32m------------ testing meojson conformance------------\033[0m"
$BuildDir/conformance_test.out `ls $NativejsonDir/data/*/*`
