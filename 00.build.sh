#!/bin/bash

if [ -z "${APP_PREFIX}" ]; then
    APP_PREFIX=${HOME}/_local
fi
if [ -z "${PKG_PREFIX}" ]; then
    PKG_PREFIX=${HOME}/_pkg
fi

xBUILD=${1}
if [ "${xBUILD}" = "gcc" ]; then
    if [ -n "${GCC_HOME}" ] && [ -e "${GCC_HOME}/bin/g++" ] && [ -e "${GCC_HOME}/bin/gcc" ] ; then
        vOPT_CXX="-DCMAKE_CXX_COMPILER=${GCC_HOME}/bin/g++"
        vOPT_C="-DCMAKE_C_COMPILER=${GCC_HOME}/bin/gcc"

        echo ${vOPT_CXX}
        echo ${vOPT_C}
    fi
fi
if [ "${xBUILD}" = "llvm" ]; then
    if [ -n "${LLVM_HOME}" ] && [ -e "${LLVM_HOME}/bin/clang++" ] && [ -e "${LLVM_HOME}/bin/clang" ] ; then
        vOPT_CXX="-DCMAKE_CXX_COMPILER=${LLVM_HOME}/bin/clang++"
        vOPT_C="-DCMAKE_C_COMPILER=${LLVM_HOME}/bin/clang"

        echo ${vOPT_CXX}
        echo ${vOPT_C}
    fi
fi

for vPATH in bin lib build 
do
    if [ -e "./${vPATH}" ]; then
        echo "rm -rf ./${vPATH}/*"
        rm -rf ./${vPATH}/*
    else
        echo "mkdir -p ./${vPATH}"
        mkdir -p ./${vPATH}
    fi
done

if [ -e "./build" ]; then

    if [ -n "$2" ] && [ "$2" = "release" ]; then
        (cd ./build; cmake .. ${vOPT_CXX} ${vOPT_C} -DCMAKE_BUILD_TYPE=Release ; make -j)
    else
        (cd ./build; cmake .. ${vOPT_CXX} ${vOPT_C} ; make -j)
    fi
fi
date
