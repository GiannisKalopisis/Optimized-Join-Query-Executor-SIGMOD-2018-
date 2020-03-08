#!/bin/bash

relationPath=$1

FILE="./small.init"


i=0
while [ $i -lt 14 ]
do
    if [ $i -eq 0 ]
    then
	echo "$relationPath/r$i" > $FILE 
    else
    	echo "$relationPath/r$i" >> $FILE
    fi
    i=$[$i+1]
done

make clean
make

if [ "$2" = "-v" ]
then
    time valgrind --leak-check=full --show-leak-kinds=all ./part2_main 0<small.init 3<small.work
else
    time ./part2_main 0<small.init 3<small.work
fi

