#/bin/bash

rm -r build/

mkdir build

cd build

cmake ..

make

./filter_finder