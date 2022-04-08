#!/bin/bash

if [ "$#" -ne 7 ]; then
	echo "too few arguments"
	exit 1
fi

mkdir stats

grep -qxF 'stats/' .gitignore || echo 'stats/' >> .gitignore 

declare -a branches=("compute")


file_name="./stats/"$(date +'%Y-%m-%d %T')".txt"

touch "$file_name"
for i in "${branches[@]}"
do
	echo "$i" >> "$file_name"
	git checkout "$i"

	cd build

	cmake ..

	make

	printf "$s" ./filter_finder 2>> ../"$file_name"
done
