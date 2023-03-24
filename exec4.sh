# /bin/bash

cd ./LinuxBuild
cmake ..
make
cp ./main ../LinuxRelease/
cd ../LinuxRelease/
./Robot_gui -m ./4.txt "./main"
