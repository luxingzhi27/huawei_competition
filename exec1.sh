# /bin/bash

cd ./LinuxBuild
cmake ..
make
cp ./main ../LinuxRelease/
cd ../LinuxRelease/
./Robot_gui -m ./1.txt "./main"
