# /bin/bash

cd ./build
make
cp ./main ../../LinuxRelease/
cd ../../LinuxRelease/
./Robot_gui -m ./map.txt "./main"
