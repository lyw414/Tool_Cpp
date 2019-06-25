#!/usr/bin/sh
Inc=`pwd`/include
Lib=`pwd`/lib
Src=`pwd`/src
Rul=`pwd`/rules
Bin=`pwd`/bin

export Inc
export Lib
export Src
export Rul
export Bin
rm ${Inc}/*
find ${Src} -name *.h | xargs -I {} cp {} ./include
find ${Src} -name *.hpp | xargs -I {} cp {} ./include
#编译按优先级进行排序编译
if [ $1 == "clean" ];then
make -f build_all.mk clean
else
make -f build_all.mk basic
make -f build_all.mk basic_app
make -f build_all.mk app
fi
