#!/bin/sh

# use this script to package Firmata for distribution
# to do: make an ant script or something else that is cross platform

# create distribution package for Arduino 1.0.x and Arduino 1.5.x
mkdir -p temp/Firmata
cp -r examples temp/Firmata
cp -r extras temp/Firmata
cp -r utility temp/Firmata
cp Boards.h temp/Firmata
cp Firmata.cpp temp/Firmata
cp Firmata.h temp/Firmata
cp keywords.txt temp/Firmata
cp readme.md temp/Firmata
cd temp
find . -name "*.DS_Store" -type f -delete
zip -r Firmata.zip ./Firmata/
cd ..
mv ./temp/Firmata.zip Firmata-2.6.0.zip
rm -r ./temp
