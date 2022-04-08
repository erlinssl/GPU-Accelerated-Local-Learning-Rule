#!/bin/bash

if [ "$#" -ne 7 ]; then
	echo "too few arguments"
	exit 1
fi

mkdir stats

grep -qxF 'stats/' .gitignore || echo 'stats/' >> .gitignore 

declare -a branches=("main" "threads-v1" "compute")

for i in "${branches[@]}"
do
	
	echo hei > times"$i".txt
done
cd build

cmake ..

make

./filter_finder 2> ../times.txt
