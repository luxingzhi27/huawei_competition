# /bin/bash

cd ./LinuxBuild
cmake ..
make
cp ./main ../LinuxRelease/
cd ../LinuxRelease/
./Robot_gui -m ./map.txt "./main"
