#!/bin/bash

EMULATION_DEBUG=2
EMULATION_RELEASE=0

BUILDDIR=./application_build

build_x86_debug=$BUILDDIR/x86-debug
build_x86_release=$BUILDDIR/x86-release

CMAKEDIR=$PWD

set -x #echo on

mkdir -p $build_x86_debug
mkdir -p $build_x86_release

cd $CMAKEDIR/$build_x86_debug
cmake -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug $CMAKEDIR

cd $CMAKEDIR/$build_x86_release
cmake -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Release $CMAKEDIR

