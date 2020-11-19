#!/bin/bash

#rm -rf build/
#mkdir build/
#cp Sicretmono.ttf build/Sicretmono.ttf
#cp station.png build/station.png
#cp scores.json build/scores.json
clear
cd build/
cmake ..
if eval cmake --build .; then
    ./MazeWreckers
    cd ..
else
    cd ..
fi
    
