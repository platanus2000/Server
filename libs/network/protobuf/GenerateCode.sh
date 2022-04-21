#!/bin/sh
for file in ./*.proto
do
    # if test -f $file
    # then
    #     echo $file 是文件
    # fi
    # if test -d $file
    # then
    #     echo $file 是目录
    # fi
    echo $file
    protoc --cpp_out=./ $file 
done